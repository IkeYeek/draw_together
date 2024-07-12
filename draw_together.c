#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "draw_together.h"
#include "queue.h"

DrawTogether* create_draw_together(int width, int height) {
  if (width <= 0 || height <= 0) {
    fprintf(stderr, "can't instantiate draw together with width %d and height %d\n", width, height);
    exit(EXIT_FAILURE);
  }
  DrawTogether* dt = calloc(1, sizeof(DrawTogether));
  if (dt == NULL) {
    fprintf(stderr, "allocation error in create_draw_together");
    exit(EXIT_FAILURE);
  }
  dt->width = width;
  dt->height = height;
  dt->points = create_queue();
  return dt;
}

void free_draw_together_point(DrawTogether_Point* dtp) {
  if (dtp == NULL) {
    fprintf(stderr, "cannot free NULL pointer in free_draw_together_point\n");
    exit(EXIT_FAILURE);
  }
  free(dtp);
}

void free_draw_together(DrawTogether* dt) {
  if (dt == NULL) {
    fprintf(stderr, "cannot free NULL pointer in free_draw_together\n");
    exit(EXIT_FAILURE);
  }
  
  free_queue(dt->points, (void (*)(void*))free_draw_together_point);
  free(dt);
}

void insert_point_draw_together(DrawTogether* dt, Vector2 position) {
  if (position.x >= -1 && position.x < dt->width && position.y >= -1 && position.y < dt->height) {
    DrawTogether_Point* dtp = calloc(1, sizeof(DrawTogether_Point));
    if (dtp == NULL) {
      fprintf(stderr, "allocation error in insert_point_draw_together");
      exit(EXIT_FAILURE);
    }
    dtp->coords = position;
    struct timespec time_poll;
    if (clock_gettime(CLOCK_REALTIME, &time_poll) != 0) {
      fprintf(stderr, "couldn't get time\n");
      exit(EXIT_FAILURE);
    }
    dtp->created_at = time_poll.tv_sec + (time_poll.tv_nsec * 1e-9);
    enqueue(dt->points, dtp);
  }
}

void update_draw_together(DrawTogether* dt) {
  struct timespec time_poll;
  if (clock_gettime(CLOCK_REALTIME, &time_poll) != 0) {
    fprintf(stderr, "couldn't get time\n");
    exit(EXIT_FAILURE);
  }
  while(dt->points->head != NULL && dt->points->head->data != NULL 
    && ((DrawTogether_Point*)(dt->points->head->data))->created_at + POINT_LIFESPAN <= time_poll.tv_sec + (time_poll.tv_nsec * 1e-9)) {
    DrawTogether_Point* to_delete = dequeue(dt->points);
    free_draw_together_point(to_delete);
  }
}

