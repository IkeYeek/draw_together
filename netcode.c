#include "netcode.h"
#include "draw_together.h"
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

Peers* init_peers() {
  Peers* ps = calloc(1, sizeof(Peers));
  if (ps == NULL) {
    fprintf(stderr, "allocation failed in init_peers\n");
    exit(EXIT_FAILURE);
  }
  ps->nb_peers = 0;
  ps->peers = calloc(1, sizeof(Peer*));
  if (ps->peers == NULL) {
    fprintf(stderr, "allocation failed in init_peers (2)\n");
    exit(EXIT_FAILURE);
  }

  ps->tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int opt_val = 1;
  if (ps->tcp_socket_fd == -1) {
    fprintf(stderr, "failed creating tcp socket\n");
    exit(errno);
  }
  setsockopt(ps->tcp_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int));
  setsockopt(ps->tcp_socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(int));

  bzero(&ps->server_addr, sizeof(ps->server_addr));

  ps->server_addr.sin_family = AF_INET;
  ps->server_addr.sin_addr.s_addr = INADDR_ANY;
  ps->server_addr.sin_port = htons(SERVER_PORT);
  memset(ps->server_addr.sin_zero, '\0', sizeof(ps->server_addr.sin_zero));

  if (bind(ps->tcp_socket_fd, (struct sockaddr*)&ps->server_addr, sizeof(ps->server_addr))) {
    fprintf(stderr, "couldn't bind to socket\n");
    exit(errno);
  }

  if (listen(ps->tcp_socket_fd, 3)) {
    fprintf(stderr, "couldn't make socket passive\n");
    exit(errno);
  }

  return ps;
}

void* _check_for_peers(void* peers_void_ptr) {
  Peers* ps = (Peers*) peers_void_ptr;
  while(1) {
    int new_client_socket;
    struct sockaddr_in new_client;
    unsigned int new_client_size = sizeof(new_client);
    bzero(&new_client, sizeof(new_client));
    printf("Waiting for a new peer...\n");
    if ((new_client_socket = accept(ps->tcp_socket_fd, (struct sockaddr*)&new_client, &new_client_size)) == -1) {
      fprintf(stderr, "socket error\n");
      exit(EXIT_FAILURE);
    }
    Peer* p = calloc(1, sizeof(Peer));
    if (p == NULL) {
      fprintf(stderr, "allocation error in check_for_peers thread\n");
      exit(EXIT_FAILURE);
    }
    p->tcp_socket_fd = new_client_socket;
    p->socket = new_client;
    //todo add mutex here
    ps->nb_peers += 1;
    ps->peers = realloc(ps->peers, sizeof(Peer) * ps->nb_peers);
    if (ps->peers == NULL) {
      fprintf(stderr, "reallocation error in check_for_peers thread\n");
      exit(EXIT_FAILURE);
    }
    ps->peers[ps->nb_peers - 1] = p;
    printf("New peer registered\n");
  }  
}

void start_check_for_peers(Peers* ps) {
  int pthread_ret;
  if ((pthread_ret = pthread_create(&ps->pthread_server_fd, NULL, _check_for_peers, (void*)ps))) {
    fprintf(stderr, "error creating server pthread\n");
    exit(pthread_ret);
  }
}

void connect_to_peer(Peers* ps, char* raw_ip, char* raw_port) {
  long port = strtol(raw_port, NULL, 10);
  Peer* p = calloc(1, sizeof(Peer));
  if (p == NULL) {
    fprintf(stderr, "allocation error in connect_to_peer\n");
    exit(EXIT_FAILURE);
  }
  if ((p->tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "couldn't create socket in connect_to_peer\n");
    exit(EXIT_FAILURE);
  }
  bzero(&p->socket, sizeof(p->socket));
  p->socket.sin_family = AF_INET;
  p->socket.sin_addr.s_addr = inet_addr(raw_ip);
  p->socket.sin_port = htons(port);
  //todo add checks for addr and port
  if (connect(p->tcp_socket_fd, (struct sockaddr*)&p->socket, sizeof(p->socket))) {
    fprintf(stderr, "error connecting to peer at %s:%ld", raw_ip, port);
    exit(EXIT_FAILURE);
  }
  printf("connected to peer at %s:%ld\n", raw_ip, port);
}

void send_to_peers(Peers* ps, Vector2 pt) {
  for (int i = 0; i < ps->nb_peers; i += 1) {
    Peer* current_peer = ps->peers[i];
    size_t len = sizeof(pt);
    size_t total_sent = 0;
    ssize_t n;
    while (total_sent < len) {
      n = send(current_peer->tcp_socket_fd, ((char*)&pt)+total_sent, len - total_sent, 0);
      if (n == -1) {
        fprintf(stderr, "send error");
        exit(EXIT_FAILURE);
        break;
      }
      total_sent += n;
    }
  }
}

void free_peers(Peers* ps) {
  pthread_cancel(ps->pthread_server_fd);
  for (int i = 0; i < ps->nb_peers; i += 1) {
    Peer* p = ps->peers[i];
    shutdown(p->tcp_socket_fd, SHUT_RDWR);
    close(p->tcp_socket_fd);
    free(p);
  }
  free(ps->peers);
  shutdown(ps->tcp_socket_fd, SHUT_RDWR);
  free(ps);
}
