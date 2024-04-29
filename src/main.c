#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>

#include "container_process.h"
#include "result.h"
#include "config.h"
#include "linux_version.h"

struct docky_args {
    uid_t uid;
    gid_t gid;
    char* hostname;
    bool stdin_attached;
    bool wait_for_exit;
    int limit_memory_mb;
    double limit_cpu_mb;
    char* workdir;
    char* rootfs;
};

#include <sys/socket.h> // socketpair
#include <fcntl.h> // fcntl
#include <sched.h> // clone and CLONE_ flags
#include <unistd.h> // close()
#include <signal.h> // SIGCHILD
#include <sys/wait.h> // waitpid

int main(int argc, char** argv) {
    (void)argv;
    (void)argc;

    struct docky_args docky_args = {
        .uid = 0,
        .gid = 0,
        .limit_memory_mb = 1000,
        .limit_cpu_mb = 1,
        .hostname = "docky",
        .stdin_attached = false,
        .wait_for_exit = true,
        .workdir = "/",
        .rootfs = "./rootfs",
    };

    // Linux 5.8 is needed for cgroups v2
    // <https://kubernetes.io/docs/concepts/architecture/cgroups/#requirements>
    must(has_minimum_linux_version(5, 8));

    // socket[1] will be used by the child process to communicate with the parent.
    int sockets[2] = {0};

    must(socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets), "Couldn't create socket pair");
    must(fcntl(sockets[0], F_SETFD, FD_CLOEXEC), "fcntl() failed");

    char* stack = malloc(CONTAINER_STACK_SIZE);

    must(be_legit(stack), "malloc() failed");

    int clone_flags =
        // Do not share "mount namespace", this way mounts made by the children won't be shown to the parent
        // https://stackoverflow.com/q/22889241/18114046
        CLONE_NEWNS |
        CLONE_NEWUSER |
        // CLONE_NEWCGROUP |
        CLONE_NEWPID |
        // CLONE_NEWIPC |
        // CLONE_NEWNET |
        // CLONE_PARENT | // will share the parent... (not what we want)
        CLONE_NEWUTS; // UTS namespace isolation for hostname

    if (docky_args.wait_for_exit) {
        // So the parent is able to wait for the child process to exit
        clone_flags = clone_flags | SIGCHLD;
    }

    if (docky_args.stdin_attached) {
        // clone_flags |=
    }

    struct container_conf container_conf = {
        .argc = argc - 1,
        .argv = argv + 1,
        .uid = docky_args.uid,
        .gid = docky_args.gid,
        .hostname = docky_args.hostname,
        .limits = {
            .cpu_mb = docky_args.limit_memory_mb,
            .memory_mb = docky_args.limit_memory_mb,
        },
        .rootfs = docky_args.rootfs,
        .workdir = docky_args.workdir,
        .socket_fd = sockets[1],
    };

    // close and zero out the child socket if something breaks
    close(sockets[1]);
    sockets[1] = 0;

    pid_t container_pid = clone(
        container_process,
        stack + CONTAINER_STACK_SIZE,
        clone_flags,
        &container_conf
    );

    must(be_non_minus_one(container_pid), "clone() failed: %s", strerror(errno));

    if (docky_args.wait_for_exit) {
        printf("waiting for child...");
        must(waitpid(container_pid, NULL, 0), "couldn't waitpid() on container");
    }

    DOCKY_DEBUG("Exiting parent process");

    return 0;
}
