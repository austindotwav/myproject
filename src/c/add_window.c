#include <pebble.h>
#include "add_window.h"

#define REPEAT_INTERVAL_MS 50

#define NUM_DOLLARS_PKEY 1
#define NUM_DOLLARS_DEFAULT 0

static Window *s_add_window;
static ActionBarLayer *s_action_bar;
static TextLayer *s_body_layer, *s_label_layer;
static GBitmap *s_icon_plus, *s_icon_minus;

static int s_num_dollars = NUM_DOLLARS_DEFAULT;

static void update_text() {
  static char s_body_text[18];
  snprintf(s_body_text, sizeof(s_body_text), "$%d", s_num_dollars);

  text_layer_set_text(s_body_layer, s_body_text);
}

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_num_dollars++;
  update_text();
}

static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_num_dollars <= 0) {
    return;
  }
  

  s_num_dollars--;
  update_text();
}

static void add_click_handler(ClickRecognizerRef recognizer, void *context) {
  int current_total = persist_read_int(NUM_DOLLARS_PKEY);

  int new_total = current_total + s_num_dollars;

  persist_write_int(NUM_DOLLARS_PKEY, new_total);

  window_stack_pop(true);
}


static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, increment_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, decrement_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, add_click_handler);
}

static void main_window_load(Window *window) {
  s_num_dollars = 0;
  Layer *window_layer = window_get_root_layer(window);

  s_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_ADD); 
  s_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_CROSS);
  
  s_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_action_bar, window);
  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
  action_bar_layer_set_background_color(s_action_bar, GColorVividCerulean);

  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_icon_plus);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_icon_minus);

  int width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;

  s_body_layer = text_layer_create(GRect(4, PBL_IF_RECT_ELSE(0, 30), width, 60));
  text_layer_set_font(s_body_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_background_color(s_body_layer, GColorClear);
  text_layer_set_text_alignment(s_body_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_body_layer));

  s_label_layer = text_layer_create(GRect(4, PBL_IF_RECT_ELSE(44, 60) + 28, width, 60));
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text(s_label_layer, "Dollars");
  text_layer_set_text_alignment(s_label_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  update_text();
}


static void main_window_unload(Window *window) {
  text_layer_destroy(s_body_layer);
  text_layer_destroy(s_label_layer);

  action_bar_layer_destroy(s_action_bar);

  gbitmap_destroy(s_icon_plus);
  gbitmap_destroy(s_icon_minus);
}

int add_window_get_total(void) {
  return s_num_dollars;
}

void add_window_push(void) {
  s_add_window = window_create();

  window_set_window_handlers(s_add_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  window_stack_push(s_add_window, true);
}