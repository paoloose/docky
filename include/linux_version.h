#pragma once

#include <sys/utsname.h>
#include "result.h"

result(void) has_minimum_linux_version(unsigned int major, unsigned int minor);
