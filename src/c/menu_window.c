#include <pebble.h>
#include "menu_window.h"

static Window *s_menu_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[1];
static SimpleMenuItem s_menu_items[3];

static void menu_select_callback(int index, void *ctx) {

  APP_LOG(APP_LOG_LEVEL_INFO, "Item clicked: %d", index);
}

static void window_load(Window *window) {

  s_menu_items[0] = (SimpleMenuItem) {
    .title = "About",
    .subtitle = "© austindotwav",
    .callback = menu_select_callback,
  };
  
  s_menu_items[1] = (SimpleMenuItem) {
    .title = "Settings",
    .callback = menu_select_callback,
  };


  s_menu_sections[0] = (SimpleMenuSection) {
    .title = "Menu",
    .items = s_menu_items,
    .num_items = ARRAY_LENGTH(s_menu_items)
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, 1, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
}

void menu_window_push(void) {
  s_menu_window = window_create();

  window_set_window_handlers(s_menu_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_menu_window, true);
}