#pragma once

// CGroup name that will be created in host's /sys/fs/cgroup/
// This CGroup "slice" will contain all the cgroups that docky will assign
// to each of its container processes
// *Note that "slices" are a systemd concept on how it manages resource limit
// scopes for services, users and systemd itself.
// <https://www.freedesktop.org/software/systemd/man/latest/systemd.slice.html>
#define CGROUP_NAME "docky"

// Size of the stack passed to the clone(2) call
#define CONTAINER_STACK_SIZE 1024 * 1024
