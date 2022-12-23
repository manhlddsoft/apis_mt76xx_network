#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include <string.h>
#include <ctype.h>

#define LOG_I(a, x...)                                  \
    printf("[\x1b[33m%s\t \x1b[34m%d] \x1b[0m", __FUNCTION__, __LINE__); \
    printf(x);\
    printf("\n");

