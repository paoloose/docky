#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <grp.h>
#include "container_process.h"
#include "result.h"
#include "debug.h"

result(void) do_mounts() {
    // return 0;
    result(void) r = {0};
    return r;
}

// TODO: how to limit cgroups

result(void) setup_userns(struct container_conf* config) {
    DOCKY_DEBUG("trying a user namespace...");

    // We need to unshare the user namespace so we can drop privileges
    int has_userns = !unshare(CLONE_NEWUSER);

    int write_res = write(
        config->socket_fd,
        &has_userns,
        sizeof(has_userns)
    );
    if (write_res != sizeof(has_userns)) {
        return_err(void, "write() to parent socket failed");
    }
    DOCKY_DEBUG("wrote to parent socket");

    int result = 0;
    int read_res = read(
        config->socket_fd,
        &result,
        sizeof(result)
    );
    if (read_res != sizeof(result)) {
        return_err(void, "read() from parent socket failed");
    }
    if (result != 0) {
        return_err(void, "Failed to unshare user namespace");
    }

    if (has_userns) { DOCKY_DEBUG("success"); }
    else { DOCKY_DEBUG("failed. continuing"); }

    DOCKY_DEBUG("switching to uid=%d, gid=%d", config->uid, config->gid);

    must(setgroups(1, &(gid_t){ config->gid }), "setgroups() failed");
    must(setresgid(config->gid, config->gid, config->gid), "setresgid() failed");
    must(setresuid(config->uid, config->uid, config->uid), "setresuid() failed");

    DOCKY_DEBUG("success");

    return_ok;
}

#include <linux/capability.h>
#include <sys/prctl.h>
#include <sys/capability.h>

result(void) setup_capabilities() {
    DOCKY_DEBUG("Dropping capabilities");

    int drop_caps[] = {
        CAP_AUDIT_CONTROL,
		CAP_AUDIT_READ,
		CAP_AUDIT_WRITE,
        CAP_BLOCK_SUSPEND,
        CAP_DAC_READ_SEARCH,
        CAP_FSETID,
        CAP_IPC_LOCK,
        CAP_MAC_ADMIN,
		CAP_MAC_OVERRIDE,
        CAP_MKNOD,
        CAP_SETFCAP,
        CAP_SYSLOG,
        CAP_SYS_ADMIN,
        CAP_SYS_BOOT,
        CAP_SYS_MODULE,
        CAP_SYS_NICE,
        CAP_SYS_RAWIO,
        CAP_SYS_RESOURCE,
        CAP_SYS_TIME,
        CAP_WAKE_ALARM,
    };
    size_t ncaps = sizeof(drop_caps) / sizeof(*drop_caps);

    for (size_t i = 0; i < ncaps; i++) {
        must(prctl(PR_CAPBSET_DROP, drop_caps[i], 0, 0, 0), "prctl() failed");
    }

    cap_t caps = cap_get_proc();
    must(be_legit(caps), "cap_get_proc() failed");
    must(cap_set_flag(caps, CAP_INHERITABLE, ncaps, drop_caps, CAP_CLEAR), "cap_set_flag() failed");
    // must(cap_set_flag(caps, CAP_EFFECTIVE, ncaps, drop_caps, CAP_CLEAR), "cap_set_flag() failed");
    // must(cap_set_flag(caps, CAP_PERMITTED, ncaps, drop_caps, CAP_CLEAR), "cap_set_flag() failed");
    must(cap_set_proc(caps), "cap_set_proc() failed");
    must(cap_free(caps), "cap_free() failed");

    DOCKY_DEBUG("success");

    return_ok;
}

// TODO
// - understand how to change cgroups programatically instead of using the filesystem interface
// - setup namespaces
// - setup capabilities
// - do i really need sockets?
// - figure out the rest

int container_process(void* __conf) {
    struct container_conf* config = __conf;
    DOCKY_DEBUG("Running child with PID %d", (int)getpid());

    must(sethostname(config->hostname, strlen(config->hostname)), "sethostname() failed");
    // mounts
        //
    // namespaces (kernel objects visible to the process tree)
        // must(setup_userns(config));
    // capabilities (what uid 0 can do)
        // must(setup_capabilities());
    // cgroups (limiting resource use)

    // syscalls
        // must(close(config->socket_fd), "close() failed");

    must(chroot("./rootfs"), "chroot() to rootfs failed");
    must(chdir("/"), "failed to chdir to new rootfs");

    DOCKY_DEBUG("Executing %s\n", config->argv[0]);
    must(not_return(execve(config->argv[0], config->argv, NULL)), "execve() failed");
    return 0;
}
