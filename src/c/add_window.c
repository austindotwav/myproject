#include <pebble.h>
#include "add_window.h"

static Window *s_add_window;

static void window_load(Window *window) {

  window_set_background_color(window, GColorRed); 
}

static void window_unload(Window *window) {
  window_destroy(s_add_window);
}

void add_window_push(void) {
  s_add_window = window_create();

  window_set_window_handlers(s_add_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_add_window, true);
}