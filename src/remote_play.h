#ifndef REMOTE_PLAY_H
#define REMOTE_PLAY_H
#include "root.h"

typedef struct {
    struct sockaddr_in peer_addr;
    struct sockaddr_in my_addr;
    int udp_send_socket;
    int udp_rx_socket;
} udp_context;

void sendMove(udp_context *ctx, Move move, Board board);
void initRemotePlay(udp_context *ctx, short peer_port_no, short my_port_no, char *peer_ip);
#endif //REMOTE_PLAY_H
