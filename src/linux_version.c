#include <stdio.h>
#include "linux_version.h"

result(void) has_minimum_linux_version(unsigned int major, unsigned int minor) {
    result(void) res = {nil};

    struct utsname host = {0};

    if (uname(&host)) {
        res.err = create_error("call to uname() failed.");
        return res;
    }

    unsigned int actual_major, actual_minor;
    sscanf(host.release, "%u.%u.", &actual_major, &actual_minor);

    if (actual_major < major || (actual_major == major && actual_minor < minor)) {
        res.err = create_error("Linux version must be at least %u.%u.", major, minor);
        return res;
    }

    return res;
}
