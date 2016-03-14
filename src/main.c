
#include "main.h"

#include "pebble.h"

static Window *s_window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer; 

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static GFont s_date_font;

static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];

static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorBlack);
    for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    const int x_offset = PBL_IF_ROUND_ELSE(18, 0);
    const int y_offset = PBL_IF_ROUND_ELSE(6, 0);
    gpath_move_to(s_tick_paths[i], GPoint(x_offset, y_offset));
    gpath_draw_filled(ctx, s_tick_paths[i]);
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  //const int16_t second_hand_length = PBL_IF_ROUND_ELSE((bounds.size.w / 2) - 19, bounds.size.w / 2);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  //int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  //GPoint second_hand = {
  //  .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
  //  .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  // };

  // second hand
  //graphics_draw_line(ctx, second_hand, center);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);

  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow); 
  gpath_draw_outline(ctx, s_minute_arrow);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  PBL_IF_ROUND_ELSE((graphics_fill_circle (ctx, GPoint(90, 90), 3)), (graphics_fill_circle (ctx, GPoint(72, 84), 3)));
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Buffer Month
  //static char mon_buffer[16];
  //strftime(mon_buffer, sizeof(mon_buffer), "%b", tick_time);
  
  // Buffer Day
  //static char day_buffer[16];
  //strftime(day_buffer, sizeof(day_buffer), "%d", tick_time);
           
  // Show the date
  //text_layer_set_text(s_month_layer, mon_buffer);
  //text_layer_set_text(s_day_layer, day_buffer);
  
  
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  update_time();

}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
   GRect bounds = layer_get_bounds(window_layer);
  
  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);  
  layer_add_child(window_layer, s_simple_bg_layer);

  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_zelda14));
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_hexa);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  //Month
  //s_month_layer = text_layer_create PBL_IF_ROUND_ELSE((GRect(37, 72, 164, 40)), (GRect(16, 70, 164, 40)));
  //text_layer_set_text_color(s_month_layer, GColorBlack);
  //text_layer_set_background_color(s_month_layer, GColorClear);
  //text_layer_set_font(s_month_layer, s_date_font); 
  //text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_layer));
  
  //Day
  //s_day_layer = text_layer_create PBL_IF_ROUND_ELSE((GRect(37, 88, 164, 40)), (GRect(16,81,164,40)));
  //text_layer_set_text_color(s_day_layer, GColorBlack);
  //text_layer_set_background_color(s_day_layer, GColorClear);
  //text_layer_set_font(s_day_layer, s_date_font);
  //text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
   
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
  

  
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);

  layer_destroy(s_hands_layer);
  
  //text_layer_destroy(s_day_layer);
  
  //text_layer_destroy(s_month_layer);
  
  fonts_unload_custom_font(s_date_font);
  
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  s_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

    for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }
  
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  
  update_time();
  
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(s_tick_paths[i]);
  }

  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}





