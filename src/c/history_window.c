#include <pebble.h>
#include "history_window.h"

static Window *s_history_window;

static void window_load(Window *window) {
  window_set_background_color(window, GColorCyan); 
}

static void window_unload(Window *window) {
  window_destroy(s_history_window);
}

void history_window_push(void) {
  s_history_window = window_create();

  window_set_window_handlers(s_history_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_history_window, true);
}