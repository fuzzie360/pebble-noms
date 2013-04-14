// Noms Pebble Watch Face
// Copyright (C) 2013  Fazli Sapuan
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You may contact the author at fazli@sapuan.org
//
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x32, 0x69, 0x5B, 0x0B, 0x47, 0x9B, 0x4D, 0x19, 0x98, 0x0C, 0xD4, 0xC4, 0x5F, 0x1D, 0x34, 0x47 }
PBL_APP_INFO(MY_UUID,
			 "Noms", "Fuzzie",
			 1, 1, /* App version */
			 DEFAULT_MENU_ICON,
			 APP_INFO_WATCH_FACE);

Window window;

TextLayer text_time_layer;

Layer background_layer;
Layer mouth_layer;
Layer jaw_layer;

BmpContainer face;
BmpContainer mouth;
BmpContainer jaw;

PropertyAnimation mouth_animation_beg;
PropertyAnimation mouth_animation_end;
PropertyAnimation jaw_animation_beg;
PropertyAnimation jaw_animation_end;

GRect mouth_from_rect;
GRect mouth_to_rect;
GRect jaw_from_rect;
GRect jaw_to_rect;

static char time_text[] = "00:00";

void face_update_callback(Layer *me, GContext* ctx) {
	(void)me;
	(void)ctx;
	
	GRect destination = layer_get_frame(&face.layer.layer);
	
	destination.origin.x = 0;
	destination.origin.y = 0;
	
	graphics_draw_bitmap_in_rect(ctx, &face.bmp, destination);
}

void mouth_update_callback(Layer *me, GContext* ctx) {
	(void)me;
	(void)ctx;
	
	GRect destination = layer_get_frame(&mouth.layer.layer);
	
	destination.origin.x = 0;
	destination.origin.y = 0;
	
	graphics_draw_bitmap_in_rect(ctx, &mouth.bmp, destination);
}

void jaw_update_callback(Layer *me, GContext* ctx) {
	(void)me;
	(void)ctx;
	
	GRect destination = layer_get_frame(&jaw.layer.layer);
	
	destination.origin.x = 0;
	destination.origin.y = 0;
	
	graphics_draw_bitmap_in_rect(ctx, &jaw.bmp, destination);
}

void animation_started(Animation *animation, void *data) {
	
}

void animation_stopped(Animation *animation, void *data) {
	text_layer_set_text(&text_time_layer, time_text);
	
	property_animation_init_layer_frame(&mouth_animation_end, &mouth_layer, &mouth_to_rect, &mouth_from_rect);
	property_animation_init_layer_frame(&jaw_animation_end, &jaw_layer, &jaw_to_rect, &jaw_from_rect);
	
	animation_schedule(&mouth_animation_end.animation);
	animation_schedule(&jaw_animation_end.animation);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)t;
	(void)ctx;
	
	property_animation_init_layer_frame(&mouth_animation_beg, &mouth_layer, &mouth_from_rect, &mouth_to_rect);
	property_animation_init_layer_frame(&jaw_animation_beg, &jaw_layer, &jaw_from_rect, &jaw_to_rect);
	
	animation_set_handlers(&mouth_animation_beg.animation, (AnimationHandlers) {
		.started = (AnimationStartedHandler) animation_started,
		.stopped = (AnimationStoppedHandler) animation_stopped
	}, &ctx);
	
	// section based on Simplicity by Pebble Team begins
	char *time_format;
	if (clock_is_24h_style()) {
		time_format = "%R";
	} else {
		time_format = "%I:%M";
	}
	
	string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);
	
	if (!clock_is_24h_style() && (time_text[0] == '0')) {
		memmove(time_text, &time_text[1], sizeof(time_text) - 1);
	}
	// section ends
	
	animation_schedule(&mouth_animation_beg.animation);
	animation_schedule(&jaw_animation_beg.animation);
}

void handle_init(AppContextRef ctx) {
	(void)ctx;
	
	window_init(&window, "Noms");
	window_stack_push(&window, true /* Animated */);
	
	resource_init_current_app(&APP_RESOURCES);
	
	layer_init(&background_layer, window.layer.frame);
	background_layer.update_proc = &face_update_callback;
	layer_add_child(&window.layer, &background_layer);
	
	mouth_from_rect = GRect(17, 45, 110, 107);
	mouth_to_rect = GRect(17, 88, 110, 107);
	jaw_from_rect = GRect(17, 142, 110, 107);
	jaw_to_rect = GRect(17, 97, 110, 107);
	
	layer_init(&mouth_layer, mouth_from_rect);
	mouth_layer.update_proc = &mouth_update_callback;
	layer_add_child(&window.layer, &mouth_layer);
	
	text_layer_init(&text_time_layer, GRect(0, 19, 110, 107));
	text_layer_set_text_color(&text_time_layer, GColorWhite);
	text_layer_set_background_color(&text_time_layer, GColorClear);
	text_time_layer.text_alignment = GTextAlignmentCenter;
	text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BIG_NOODLE_TITLING_55)));
	layer_add_child(&mouth_layer, &text_time_layer.layer);
	
	layer_init(&jaw_layer, jaw_from_rect);
	jaw_layer.update_proc = &jaw_update_callback;
	layer_add_child(&window.layer, &jaw_layer);
   	
	bmp_init_container(RESOURCE_ID_IMAGE_FACE,  &face);
	bmp_init_container(RESOURCE_ID_IMAGE_MOUTH, &mouth);
	bmp_init_container(RESOURCE_ID_IMAGE_JAW,   &jaw);
}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;
	
	bmp_deinit_container(&face);
	bmp_deinit_container(&mouth);
	bmp_deinit_container(&jaw);
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	app_event_loop(params, &handlers);
}
