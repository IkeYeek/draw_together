#ifndef NETCODE_H
#define NETCODE_H

#include "draw_together.h"
#include <netinet/in.h>
#include <raylib.h>

#define SERVER_PORT 1337
#define BUFFER_SIZE 1024

typedef struct Peer {
  struct sockaddr_in socket;
  int tcp_socket_fd;
} Peer;

typedef struct Peers {
  int tcp_socket_fd;
  pthread_t pthread_server_fd;
  pthread_t pthread_client_fd;
  pthread_mutex_t mutex;
  struct sockaddr_in server_addr;
  Peer** peers;
  int nb_peers;
} Peers;

Peers* init_peers();
void connect_to_peer(Peers* ps, char* raw_ip, char* raw_port);
void send_to_peers(Peers* ps, Vector2 pt);
void start_check_for_peers(Peers* ps);
void start_check_peers_for_data(Peers* ps, DrawTogether* dt);
void free_peers(Peers* ps);

#endif
