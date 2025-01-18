// This project uses the unity build method.
// root.h is included before anything and includes all system headers, libraries, project headers as well as all source files.
#pragma once

#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define USEFUL_THINGS_IMPLEMENTATION
#include <useful_things.h>

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"
#include "../include/raylib.h"

#include "common.h"
#include "input_handling.h"
#include "remote_play.h"
#include "renderer.h"
#include "chess.h"

#include "renderer.c"
#include "board_access.c"
#include "input_handling.c"
#include "remote_play.c"
#include "bot.c"
#include "chess.c"
