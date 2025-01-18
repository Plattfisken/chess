#pragma once
#include "root.h"

void initRemotePlay(UdpContext *ctx, short peer_port_no, short my_port_no, const char *peer_ip) {
    ctx->peer_addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(peer_port_no),
    };

    ctx->my_addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(my_port_no)
    };

    assert(inet_pton(AF_INET, peer_ip, &(ctx->peer_addr.sin_addr)) > 0 && "Failed to convert ip-address");

    ctx->udp_send_socket = socket(PF_INET, SOCK_DGRAM, 0);
    assert(ctx->udp_send_socket > 0 && "failed to create send socket");

    ctx->udp_rx_socket = socket(PF_INET, SOCK_DGRAM, 0);
    assert(ctx->udp_send_socket > 0 && "failed to create recieved socket");

    int r = bind(ctx->udp_rx_socket, (struct sockaddr *)&(ctx->my_addr), sizeof ctx->my_addr);
    assert(r >= 0 && "Failed to bind recieve socket");
}

Move deserializeMove(char *move_in_long_algebraic_notation) {
    if(strncmp(move_in_long_algebraic_notation, "0-0", sizeof "0-0") == 0)
        return (Move){ .type = SHORT_CASTLE };
    if(strncmp(move_in_long_algebraic_notation, "0-0-0", sizeof "0-0-0") == 0)
        return (Move){ .type = LONG_CASTLE };

    // If the move isn't a castle or a pawn then the first char represents the piece type and can be ignored for this
    int pos = 0;
    if(move_in_long_algebraic_notation[pos] == 'N' ||
       move_in_long_algebraic_notation[pos] == 'B' ||
       move_in_long_algebraic_notation[pos] == 'R' ||
       move_in_long_algebraic_notation[pos] == 'Q' ||
       move_in_long_algebraic_notation[pos] == 'K') {
        ++pos;
    }

    assert(move_in_long_algebraic_notation[pos] >= 'a' &&
           move_in_long_algebraic_notation[pos] <= 'h' &&
           "Failed to deserialize recieved move\nstarting file was not a character between a-h");
    int starting_file = move_in_long_algebraic_notation[pos++] - 'a';

    assert(move_in_long_algebraic_notation[pos] >= '1' &&
           move_in_long_algebraic_notation[pos] <= '8' &&
           "Failed to deserialize recieved move\nstarting file was not a number between 1-8");
    int starting_rank = (move_in_long_algebraic_notation[pos++] - '8') * -1;

    if(move_in_long_algebraic_notation[pos] == 'x')
        ++pos;

    Move move_to_return = {
        .startIndex = IDX_2D(starting_file, starting_rank)
    };

    assert(move_in_long_algebraic_notation[pos] >= 'a' &&
           move_in_long_algebraic_notation[pos] <= 'h' &&
           "Failed to deserialize recieved move\nstarting file was not a character between a-h");
    int ending_file = move_in_long_algebraic_notation[pos++] - 'a';

    assert(move_in_long_algebraic_notation[pos] >= '1' &&
           move_in_long_algebraic_notation[pos] <= '8' &&
           "Failed to deserialize recieved move\nstarting file was not a number between 1-8");
    int ending_rank = (move_in_long_algebraic_notation[pos] - '8') * -1;

    move_to_return.destIndex = IDX_2D(ending_file, ending_rank);
    move_to_return.type = NO_MOVE;
    return move_to_return;
}

char *serialize_move_long_alg(Board board, Move move, char *buf) {
    int pos = 0;
    if(move.type == SHORT_CASTLE) {
        memcpy(buf, "0-0", sizeof("0-0"));
        return buf;
    }
    if(move.type == LONG_CASTLE) {
        memcpy(buf, "0-0-0", sizeof("0-0-0"));
        return buf;
    }

    // piece
    switch(board.position[move.startIndex]) {
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            buf[pos++] = 'N';
            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            buf[pos++] = 'B';
            break;
        case WHITE_ROOK:
        case BLACK_ROOK:
            buf[pos++] = 'R';
            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            buf[pos++] = 'Q';
            break;
        case WHITE_KING:
        case BLACK_KING:
            buf[pos++] = 'K';
            break;
        default:
            break;
    }

    // starting square
    // file
    buf[pos++] = FILE(move.startIndex) + 'a';//_getFileLetterFromIndex(FILE(move.startIndex));

    // rank
    buf[pos++] = '0' + (COL_SIZE - RANK(move.startIndex));

    // capture
    if(board.position[move.destIndex] != EMPTY) {
        buf[pos++] = 'x';
    }

    // endeing square
    // file
    buf[pos++] = FILE(move.destIndex) + 'a';//_getFileLetterFromIndex(FILE(move.destIndex));

    // rank
    buf[pos++] = '0' + (COL_SIZE - RANK(move.destIndex));

    // null-terminate
    buf[pos] = 0;
    return buf;
}

void sendMove(UdpContext *ctx, Move move, Board board) {
    char serializedMove[16];
    serialize_move_long_alg(board, move, serializedMove);

    assert(sendto(ctx->udp_send_socket, serializedMove,
                   8, 0, (struct sockaddr *)&(ctx->peer_addr),
                   sizeof ctx->peer_addr) >= 0 && "Failed to send move");
}

Move _getRemotePlayerMove(UdpContext *ctx, Move *legal_moves) {
    // clear buffer
    char buf[16];
    memset(buf, 0, sizeof buf);
    struct pollfd poll_fd = {
        .fd = ctx->udp_rx_socket,
        .events = POLLRDNORM | POLLRDBAND | POLLPRI
    };
    int result = poll(&poll_fd, 1, 0);
    assert(result != -1);
    if(result > 0) {
        int bytes_recieved = recvfrom(ctx->udp_rx_socket, buf, sizeof buf, 0, NULL, NULL);
        assert(bytes_recieved >= 0 && "Error recieving packet");
        Move move = deserializeMove(buf);

        if(move.type == SHORT_CASTLE || move.type == LONG_CASTLE) {
            for(int i = 0; i < arrlen(legal_moves); ++i)
                if(legal_moves[i].type == move.type)
                    return legal_moves[i];
        }
        else {
            for(int i = 0; i < arrlen(legal_moves); ++i)
                if(legal_moves[i].startIndex == move.startIndex && legal_moves[i].destIndex == move.destIndex)
                    return legal_moves[i];
        }
    }
    return (Move){ .type = NO_MOVE };
}
