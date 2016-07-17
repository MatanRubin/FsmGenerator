#include <stdlib.h>
#include <stdbool.h>
#include "mr_common.h"

bool debug_mode_enabled = false;

void mr_panic() {
    abort();
}

void mr_set_debug_mode(bool enabled) {
    debug_mode_enabled = enabled;
}