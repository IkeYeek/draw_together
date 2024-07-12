#ifndef DRAW_TOGETHER_H
#define DRAW_TOGETHER_H

#define POINT_LIFESPAN 5

#include <time.h>
#include <raylib.h>
#include <pthread.h>

#include "queue.h"

typedef struct DrawTogether_Point {
  Vector2 coords;
  double created_at;
} DrawTogether_Point;

typedef struct DrawTogether {
  Queue* points; 
  int width;
  int height;
  pthread_mutex_t mutex;
} DrawTogether;

DrawTogether* create_draw_together(int width, int height);
void free_draw_together_point(DrawTogether_Point* dtp);
void free_draw_together(DrawTogether* dt);
void insert_point_draw_together(DrawTogether* dt, Vector2 position);
void update_draw_together(DrawTogether* dt);

#endif
