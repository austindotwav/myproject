#include <pebble.h>
#include "storage.h"

// Example; unimplemented
#include "fruit_backend.h"
#include "history_window.h"

#include "add_window.h"
#include "menu_window.h"

#define NUM_DOLLARS_PKEY 1
#define LAST_DAY_PKEY 2

static Window *s_main_window;
static ActionBarLayer *s_action_bar;
static TextLayer *s_text_layer;
static TextLayer *s_total_layer;
static GBitmap *s_icon_history, *s_icon_add, *s_icon_menu;


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

static void check_daily_reset() {
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  int current_day = tick_time->tm_yday;

  int last_saved_day = -1;
  if (persist_exists(LAST_DAY_PKEY)) {
    last_saved_day = persist_read_int(LAST_DAY_PKEY);
}

if (last_saved_day != current_day) {
    persist_write_int(LAST_DAY_PKEY, current_day);
    persist_write_int(NUM_DOLLARS_PKEY, 0);

    APP_LOG(APP_LOG_LEVEL_INFO, "Reset daily total to 0");
  }
}

static void update_window() {
  int saved_total = persist_read_int(NUM_DOLLARS_PKEY);

  static char s_buffer[32];

  snprintf(s_buffer, sizeof(s_buffer), "$%d", saved_total);

  text_layer_set_text(s_total_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  check_daily_reset();

  if(layer_get_window(text_layer_get_layer(s_total_layer)) == s_main_window) {
    update_window();
  }
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  int width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;

  s_text_layer = text_layer_create(GRect(4, PBL_IF_RECT_ELSE(10, 15), width, 100));
  GFont heading_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  text_layer_set_font(s_text_layer, heading_font);
  text_layer_set_text(s_text_layer, "Today");
  text_layer_set_text_alignment(s_text_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  static char s_buffer[32];



  s_total_layer = text_layer_create(GRect(10, 44, 124, 100));
  GFont total_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  text_layer_set_font(s_total_layer, total_font);

  layer_add_child(window_layer, text_layer_get_layer(s_total_layer));
  update_window();


  s_icon_history = gbitmap_create_with_resource(RESOURCE_ID_HISTORY);
  s_icon_add = gbitmap_create_with_resource(RESOURCE_ID_ADD);
  s_icon_menu = gbitmap_create_with_resource(RESOURCE_ID_MENU);


  s_action_bar = action_bar_layer_create();
  action_bar_layer_set_background_color(s_action_bar, GColorVividCerulean);
  action_bar_layer_add_to_window(s_action_bar, window);

  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_icon_history);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_add);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_icon_menu);

  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
}


static void main_window_appear(Window *window) {
  update_window();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  text_layer_destroy(s_total_layer);
  action_bar_layer_destroy(s_action_bar);

  gbitmap_destroy(s_icon_history);
  gbitmap_destroy(s_icon_add);
  gbitmap_destroy(s_icon_menu);
}

void init(void) {
  s_main_window = window_create();

  check_daily_reset();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
    .appear = main_window_appear,
  });
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Starting App");
  init();
  app_event_loop();
  deinit();
}
