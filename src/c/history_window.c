#include <pebble.h>
#include "history_window.h"
#include "storage.h"

#define STORAGE_KEY_HISTORY 1
#define MAX_HISTORY_ENTRIES 10

static char s_menu_item_buffers[MAX_HISTORY_ENTRIES][16];

static Window *s_menu_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[1];
static SimpleMenuItem s_menu_items[MAX_HISTORY_ENTRIES];


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  SpendingHistory history = {0};
  if (persist_exists(STORAGE_KEY_HISTORY)) {
    persist_read_data(STORAGE_KEY_HISTORY, &history, sizeof(SpendingHistory));
  }

  int items_to_display;

  int num_items = (history.entry_count > 0) ? history.entry_count : 1;

  if (history.entry_count == 0) {
    items_to_display = 1;
    s_menu_items[0] = (SimpleMenuItem) {
      .title = "No history available",
    };
  } else {
    items_to_display = history.entry_count;

    for (int i = 0; i < history.entry_count; i++) {
      snprintf(s_menu_item_buffers[i], sizeof(s_menu_item_buffers[i]), "$%d", history.entries[i]);
      s_menu_items[i] = (SimpleMenuItem) {
        .title = s_menu_item_buffers[i],
      };
    }
  }

  s_menu_sections[0] = (SimpleMenuSection) {
    .title = "History",
    .items = s_menu_items,
    .num_items = items_to_display
  };

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
  window_destroy(s_menu_window);
  s_menu_window = NULL;
}

void history_window_push(void) {
  s_menu_window = window_create();

  window_set_window_handlers(s_menu_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_menu_window, true);
}