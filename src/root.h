#pragma once

#include <stddef.h>

#define TRUE 1
#define FALSE 0

#define ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(arr[0])))

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"

#include "boardAccess.c"
#include "chess.c"
#include "bot.c"
