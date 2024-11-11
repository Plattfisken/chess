#pragma once

#include <stddef.h>

#define TRUE 1
#define FALSE 0

#define ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(arr[0])))

//inlcude stb_ds
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "boardAccess.c"
#include "chess.h"
#include "chess.c"
#include "tree.c"
#include "bot.c"
