#pragma once
#include <sys/types.h>

struct container_conf {
    uid_t uid;
    gid_t gid;
    char* hostname;
    int argc;
    char** argv;
    int socket_fd;
};

int container_process(void* conf);
