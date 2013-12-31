#include <pebble.h>
#include <xprintf.h>

Window *window;
TextLayer *dogecoin_layer;
TextLayer *legend_layer_units;
TextLayer *legend_layer_doge;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *wow_layer;
TextLayer *amaze_layer;
BitmapLayer *image_layer;
GBitmap *image;
Layer *line_layer;
int satoshiPerDoge;

void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void drawSPD(int satoshiPerDoge){
	static char tickerText[] = "Xxxxxxxxxx";
	static char unitsText[] = "satoshi";
	static char dogeText[] = "loading";
	if (satoshiPerDoge == -2) {
		xsprintf(tickerText, "wow");
		xsprintf(unitsText, "such");
		xsprintf(dogeText, "loading");
		text_layer_set_font(dogecoin_layer,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_BOLD_21)));
	} else if (satoshiPerDoge < 0) {
		xsprintf(tickerText, "wow");
		xsprintf(unitsText, "so");
		xsprintf(dogeText, "error");
		text_layer_set_font(dogecoin_layer,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_BOLD_21)));
	} else {
		xsprintf(tickerText, "%d", satoshiPerDoge);
		xsprintf(unitsText, "satoshi");
		xsprintf(dogeText, "DOGE");
		text_layer_set_font(dogecoin_layer,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_BOLD_36)));
	}
	// TODO store last values, don't update if they didn't change
	// (to save battery life)
	text_layer_set_text(dogecoin_layer, tickerText);
	text_layer_set_text(legend_layer_units, unitsText);
	text_layer_set_text(legend_layer_doge, dogeText);
	//layer_mark_dirty(bitmap_layer_get_layer(image_layer));
}

void drawTime(struct tm *tick_time, TimeUnits units_changed){
	static char time_text[20];
	strftime(time_text, sizeof(time_text), "%R %p", tick_time);
	// strip leading 0
	if (time_text[0] == '0'){
		for (int i=0; i < (int) sizeof(time_text)-1; i++){
			time_text[i] = time_text[i+1];
			if (time_text[i] == 0){
				break;
			}
		}
	}
	text_layer_set_text(time_layer, time_text);
}

void drawDate(struct tm *tick_time, TimeUnits units_changed){
	static char date_text[40];
	strftime(date_text, sizeof(date_text), "%A %b %d", tick_time);
	text_layer_set_text(date_layer, date_text);
}

void inbox_recieved_callback(DictionaryIterator *iterator, void *context){
	bool updated = 0;
	Tuple* tuple = dict_find(iterator, 0);
	if (tuple != 0){
		if (tuple->type == TUPLE_INT && tuple->length == 4){
			satoshiPerDoge = tuple->value->int32;
		} else {
			satoshiPerDoge = -1;
		}
		updated = 1;
	}
	if (updated){
		drawSPD(satoshiPerDoge);
	}
}

void handle_init(void) {
	window = window_create();
	window_stack_push(window, true /* Animated */);
	
	Layer *window_layer = window_get_root_layer(window);
	
	GRect bounds = layer_get_frame(window_layer);

	
	legend_layer_units = text_layer_create(GRect(40, 3, 144-40-10, 20));
	text_layer_set_font(legend_layer_units,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_18)));
	text_layer_set_text_color(legend_layer_units, GColorBlack);
	text_layer_set_text_alignment(legend_layer_units, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(legend_layer_units));
	
	legend_layer_doge = text_layer_create(GRect(40, 24, 144-40-10, 20));
	text_layer_set_font(legend_layer_doge,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_18)));
	text_layer_set_text_color(legend_layer_doge, GColorBlack);
	text_layer_set_text_alignment(legend_layer_doge, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(legend_layer_doge));
	
	dogecoin_layer = text_layer_create(GRect(7, 3, 55-7, 168-3));
	text_layer_set_font(dogecoin_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_BOLD_36)));
	text_layer_set_text_color(dogecoin_layer, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(dogecoin_layer));
	
	wow_layer = text_layer_create(GRect(95, 70, 144-95, 168-70));
	text_layer_set_font(wow_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_16)));
	text_layer_set_text_color(wow_layer, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(wow_layer));
	static char wowText[] = "wow";
	text_layer_set_text(wow_layer, wowText);
	
	amaze_layer = text_layer_create(GRect(72, 92, 144-72, 168-92));
	text_layer_set_font(amaze_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_16)));
	text_layer_set_text_color(amaze_layer, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(amaze_layer));
	static char amazeText[] = "amaze";
	text_layer_set_text(amaze_layer, amazeText);
	
	time_layer = text_layer_create(GRect(0, 119, 144, 30));
	text_layer_set_font(time_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_BOLD_21)));
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	
	date_layer = text_layer_create(GRect(0, 145, 144, 30));
	text_layer_set_font(date_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_SANS_16)));
	text_layer_set_text_color(date_layer, GColorBlack);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(date_layer));
	
	GRect line_frame = GRect(58, 23, 115-58, 2);
    line_layer = layer_create(line_frame);
    layer_set_update_proc(line_layer, line_layer_update_callback);
    layer_add_child(window_layer, line_layer);
	
	image = gbitmap_create_with_resource(RESOURCE_ID_WOW_SUCH_FACE);
	image_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(image_layer, image);
	bitmap_layer_set_alignment(image_layer, GAlignCenter);
	bitmap_layer_set_compositing_mode(image_layer, GCompOpAnd);
	layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
	
	satoshiPerDoge = -1;
	
	app_message_register_inbox_received(inbox_recieved_callback);
	app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
	
	drawSPD(-2);
	
	tick_timer_service_subscribe(MINUTE_UNIT, drawTime);
	tick_timer_service_subscribe(DAY_UNIT, drawDate);
	
	time_t init_time = time(0);
	struct tm *init_local = localtime(&init_time);
	
	drawTime(init_local, 0);
	drawDate(init_local, 0);
	

}

void handle_deinit(void) {
	text_layer_destroy(dogecoin_layer);
	text_layer_destroy(time_layer);
	window_destroy(window);
}


int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
