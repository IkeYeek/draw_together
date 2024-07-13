#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>

#include "draw_together.h"
#include "netcode.h"
#include "queue.h"

#define WIN_WIDTH 1720
#define WIN_HEIGHT 1080
#define TARGET_FRAMERATE 300

void init() {
  InitWindow(WIN_WIDTH, WIN_HEIGHT, "Draw Together"); 
  SetTargetFPS(TARGET_FRAMERATE);
}

void cleanup(DrawTogether* dt, Peers* peers) {
  free_peers(peers); 
  free_draw_together(dt);
  CloseWindow();
}

int main() {
  init();
  Peers* peers = init_peers(); 
  start_check_for_peers(peers);

  DrawTogether* dt = create_draw_together(WIN_WIDTH, WIN_HEIGHT);

  start_check_peers_for_data(peers, dt);
  while(!WindowShouldClose()) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      int x, y;
      x = GetMouseX();
      y = GetMouseY();
      Vector2 v2 = {
        .x = x,
        .y = y
      };
      send_to_peers(peers, v2);
      insert_point_draw_together(dt, v2);
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      Vector2 nv = {
        .x = -1,
        .y = -1,
      };
      send_to_peers(peers, nv);
      insert_point_draw_together(dt, nv);
    } else if (IsKeyDown(KEY_C)) {
      printf("Enter peer ip\n");
      char raw_ip[16];
      fgets(raw_ip, 15, stdin);
      printf("Enter peer port\n");
      char raw_port[6];
      fgets(raw_port, 5, stdin);
      connect_to_peer(peers, raw_ip, raw_port);
    }
    BeginDrawing();
    DrawFPS(0, 0);
    ClearBackground(RAYWHITE);
    pthread_mutex_lock(&dt->mutex);
    LinkedList_Node* current_node = dt->points->head;
    Vector2 last_position = {.x = -1, .y = -1};
    while (current_node != NULL) {
      DrawTogether_Point* current_point = (DrawTogether_Point*) current_node->data;
      if (current_point->coords.x < 0) {
        last_position.x = -1;
        last_position.y = -1;
      }
      else {
        if (last_position.x >= 0) {
          DrawLineV(last_position, current_point->coords, BLACK);
        }
        last_position = current_point->coords;
      }
      current_node = current_node->next;
    }
    pthread_mutex_unlock(&dt->mutex);
    EndDrawing();
    update_draw_together(dt);
  }
  cleanup(dt, peers);
  return EXIT_SUCCESS;
}

