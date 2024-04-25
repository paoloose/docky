#pragma once
#include <sys/types.h>

struct container_limits {
    int memory_mb;
    double cpu_mb;
};

struct container_conf {
    uid_t uid;
    gid_t gid;
    char* hostname;
    int argc;
    char** argv;
    int socket_fd;
    struct container_limits limits;
    char* workdir;
    char* rootfs;
};

int container_process(void* conf);
