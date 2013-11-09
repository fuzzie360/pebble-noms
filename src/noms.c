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
#include <pebble.h>
#include <pebble_app_info.h>
#include <pebble_fonts.h>

static Window *window;

static TextLayer *text_time_layer;

static BitmapLayer *background_layer;
static BitmapLayer *mouth_layer;
static BitmapLayer *jaw_layer;

static GFont *time_font;

static GBitmap *face;
static GBitmap *mouth;
static GBitmap *jaw;

static PropertyAnimation *mouth_animation_beg = NULL;
static PropertyAnimation *mouth_animation_end = NULL;
static PropertyAnimation *jaw_animation_beg = NULL;
static PropertyAnimation *jaw_animation_end = NULL;

static GRect mouth_from_rect;
static GRect mouth_to_rect;
static GRect jaw_from_rect;
static GRect jaw_to_rect;

static char time_text[] = "00:00";
static char time_text_buffer[] = "00:00";

void destroy_property_animation(PropertyAnimation **prop_animation) {
    if (*prop_animation == NULL) {
        return;
    }

    if (animation_is_scheduled((Animation*) *prop_animation)) {
        animation_unschedule((Animation*) *prop_animation);
    }

    property_animation_destroy(*prop_animation);
    *prop_animation = NULL;
}

void animation_started(Animation *animation, void *data) {
    (void)animation;
    (void)data;
}

void animation_stopped(Animation *animation, void *data) {
    (void)animation;
    (void)data;

    memcpy(time_text, time_text_buffer, strlen(time_text)+1);
    text_layer_set_text(text_time_layer, time_text);

    destroy_property_animation(&mouth_animation_end);
    destroy_property_animation(&jaw_animation_end);

    mouth_animation_end = property_animation_create_layer_frame(bitmap_layer_get_layer(mouth_layer), &mouth_to_rect, &mouth_from_rect);
    jaw_animation_end = property_animation_create_layer_frame(bitmap_layer_get_layer(jaw_layer),  &jaw_to_rect, &jaw_from_rect);

    animation_schedule((Animation*) mouth_animation_end);
    animation_schedule((Animation*) jaw_animation_end);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
    destroy_property_animation(&mouth_animation_beg);
    destroy_property_animation(&jaw_animation_beg);

    mouth_animation_beg = property_animation_create_layer_frame(bitmap_layer_get_layer(mouth_layer), &mouth_from_rect, &mouth_to_rect);
    jaw_animation_beg = property_animation_create_layer_frame(bitmap_layer_get_layer(jaw_layer), &jaw_from_rect, &jaw_to_rect);

    animation_set_handlers((Animation*) mouth_animation_beg, (AnimationHandlers) {
        .started = (AnimationStartedHandler) animation_started,
        .stopped = (AnimationStoppedHandler) animation_stopped
    }, 0);

    // section based on Simplicity by Pebble Team begins
    char *time_format;

    if (clock_is_24h_style()) {
        time_format = "%R";
    } else {
        time_format = "%I:%M";
    }

    strftime(time_text_buffer, sizeof(time_text_buffer), time_format, tick_time);

    if (!clock_is_24h_style() && (time_text_buffer[0] == '0')) {
        memmove(time_text_buffer, &time_text_buffer[1], sizeof(time_text_buffer) - 1);
    }
    // section ends

    animation_schedule((Animation*) mouth_animation_beg);
    animation_schedule((Animation*) jaw_animation_beg);
}

static void window_load(Window *window) {
    Layer *root_layer = window_get_root_layer(window);

    face = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FACE);
    mouth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOUTH);
    jaw = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_JAW);

    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BIG_NOODLE_TITLING_55));

    background_layer = bitmap_layer_create(layer_get_frame(root_layer));
    bitmap_layer_set_bitmap(background_layer, face);
    layer_add_child(root_layer, bitmap_layer_get_layer(background_layer));

    mouth_from_rect = GRect(17, 45, 110, 107);
    mouth_to_rect = GRect(17, 88, 110, 107);
    jaw_from_rect = GRect(17, 142, 110, 107);
    jaw_to_rect = GRect(17, 97, 110, 107);

    mouth_layer = bitmap_layer_create(mouth_from_rect);
    bitmap_layer_set_bitmap(mouth_layer, mouth);
    layer_add_child(root_layer, bitmap_layer_get_layer(mouth_layer));

    text_time_layer = text_layer_create(GRect(0, 19, 110, 107));
    text_layer_set_text_color(text_time_layer, GColorWhite);
    text_layer_set_background_color(text_time_layer, GColorClear);
    text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
    text_layer_set_font(text_time_layer, time_font);
    layer_add_child(bitmap_layer_get_layer(mouth_layer), text_layer_get_layer(text_time_layer));

    jaw_layer = bitmap_layer_create(jaw_from_rect);
    bitmap_layer_set_bitmap(jaw_layer, jaw);
    layer_add_child(root_layer, bitmap_layer_get_layer(jaw_layer));
}

static void window_unload(Window *window) {
    destroy_property_animation(&mouth_animation_beg);
    destroy_property_animation(&jaw_animation_beg);
    destroy_property_animation(&mouth_animation_end);
    destroy_property_animation(&jaw_animation_end);

    bitmap_layer_destroy(jaw_layer);
    text_layer_destroy(text_time_layer);
    bitmap_layer_destroy(mouth_layer);
    bitmap_layer_destroy(background_layer);

    fonts_unload_custom_font(time_font);

    gbitmap_destroy(jaw);
    gbitmap_destroy(mouth);
    gbitmap_destroy(face);
}

static void init(void) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(window, animated);
    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    window_stack_remove(window, false);
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
