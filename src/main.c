#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>

#include "container_process.h"
#include "result.h"
#include "linux_version.h"

struct docky_args {
    uid_t uid;
    gid_t gid;
    char* hostname;
    bool stdin_attached;
    bool wait_for_exit;
    char* workdir;
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
        .hostname = "docky",
        .stdin_attached = false,
        .wait_for_exit = true,
        .workdir = "/",
    };

    must(has_minimum_linux_version(3, 10));

    // socket[0] will be used by the child process to communicate with the parent.
    int sockets[2] = {0};

    must(socketpair(AF_LOCAL, SOCK_SEQPACKET, 0, sockets), "Couldn't create socket pair");
    must(fcntl(sockets[0], F_SETFD, FD_CLOEXEC), "fcntl() failed");

    #define STACK_SIZE 1024 * 1024

    char* stack = malloc(STACK_SIZE);

    must(be_legit(stack), "malloc() failed");

    int clone_flags =
        CLONE_NEWNS |
        CLONE_NEWCGROUP |
        CLONE_NEWPID |
        CLONE_NEWIPC |
        CLONE_NEWNET |
        CLONE_NEWUTS;

    if (docky_args.stdin_attached) {
        // clone_flags |=
    }

    struct container_conf container_conf = {
        .argc = argc - 1,
        .argv = argv + 1,
        .uid = docky_args.uid,
        .gid = docky_args.gid,
        .hostname = docky_args.hostname,
        .socket_fd = sockets[0],
    };

    pid_t container_pid = clone(
        container_process,
        stack + STACK_SIZE,
        clone_flags | SIGCHLD,
        &container_conf
    );

    must(be_non_minus_one(container_pid), "clone() failed: %s", strerror(errno));

    if (docky_args.wait_for_exit) {
        must(waitpid(container_pid, NULL, 0), "couldn't waitpid() on container");
    }

    close(sockets[1]);
    sockets[1] = 0;

    DOCKY_DEBUG("Exiting parent process");

    return 0;
}
