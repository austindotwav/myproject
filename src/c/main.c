#include <pebble.h>

// Example; unimplemented
#include "fruit_backend.h"
#include "history_window.h"
#include "add_window.h"
#include "menu_window.h"

static Window *s_main_window;
static ActionBarLayer *s_action_bar;
static TextLayer *s_text_layer;
static TextLayer *s_total_layer;

static void history_click_handler(ClickRecognizerRef recognizer, void *context) {
  history_window_push();
}

static void add_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_window_push();
}

static void menu_click_handler(ClickRecognizerRef recognizer, void *context) {
  menu_window_push();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, history_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, add_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, menu_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(10, 15, 124, 100));
  GFont heading_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  text_layer_set_font(s_text_layer, heading_font);
  text_layer_set_text(s_text_layer, "Today");
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  static char s_buffer[32];

  double total = 55.50;

  int dollars = (int)total;
  int cents = (int)(total * 100) % 100;

  snprintf(s_buffer, sizeof(s_buffer), "%d.%02d", dollars, cents);

  s_total_layer = text_layer_create(GRect(10, 44, 124, 100));
  GFont total_font = fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS);
  text_layer_set_font(s_total_layer, total_font);
  text_layer_set_text(s_total_layer, s_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_total_layer));


  s_action_bar = action_bar_layer_create();
  action_bar_layer_set_background_color(s_action_bar, GColorVividCerulean);
  action_bar_layer_add_to_window(s_action_bar, window);

  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, gbitmap_create_with_resource(RESOURCE_ID_HISTORY));
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, gbitmap_create_with_resource(RESOURCE_ID_ADD));
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, gbitmap_create_with_resource(RESOURCE_ID_MENU));

  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  action_bar_layer_destroy(s_action_bar);
}

void init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  app_event_loop();
  window_destroy(s_main_window);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
