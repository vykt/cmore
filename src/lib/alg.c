//standard library
#include <stdint.h>

//local headers
#include "cmore.h"
#include "debug.h"


/*
 *  --- [EXTERNAL] ---
 */

long cm_clamp(const long value, const long lower, const long upper) {

    if (value < lower) return lower;
    if (value > upper) return upper;
    return value;
}
