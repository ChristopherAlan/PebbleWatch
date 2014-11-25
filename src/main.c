#include <pebble.h>
  
static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_day_layer;

static GFont s_time_font;
static GFont s_day_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char time_buffer[] = "00:00";
  static char day_buffer[] = "XXX";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_buffer[0] == '0')) {
    memmove(time_buffer, &time_buffer[1], sizeof(time_buffer) - 1);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, time_buffer);
  
  // Write day of the week into buffer
  strftime(day_buffer, sizeof("XXX"), "%a", tick_time);
  text_layer_set_text(s_day_layer, day_buffer);
}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SOLAR_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 8, 142, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create day TextLayer
  s_day_layer = text_layer_create(GRect(4, 0, 144, 20));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_text(s_day_layer, "XXX");

// Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANFRAN_ULIGHT_48));
  s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANFRAN_D_REG_14));

// Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font); 
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
  
  text_layer_set_font(s_day_layer, s_day_font); 
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentLeft);

// Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
 // Unload GFont
    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_day_font);
  
 // Destroy GBitmap
    gbitmap_destroy(s_background_bitmap);
  
 // Destroy BitmapLayer
    bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_day_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
}

static void init() {
// Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_fullscreen(s_main_window, true);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

 // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
 // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}