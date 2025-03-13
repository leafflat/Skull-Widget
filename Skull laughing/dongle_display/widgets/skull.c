/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "skull.h"

#define SRC(array) (const void **)array, sizeof(array) / sizeof(lv_img_dsc_t *)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

LV_IMG_DECLARE(SKULL_NONE1);
LV_IMG_DECLARE(SKULL_NONE2);
LV_IMG_DECLARE(SKULL1);
LV_IMG_DECLARE(SKULL2);
LV_IMG_DECLARE(SKULL3);


#define ANIMATION_SPEED_IDLE 6000
const lv_img_dsc_t *idle_imgs[] = {
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL_NONE2,
};

#define ANIMATION_SPEED_SLOW 3500
const lv_img_dsc_t *slow_imgs[] = {
    &SKULL_NONE2,
};

#define ANIMATION_SPEED_MID 3000
const lv_img_dsc_t *mid_imgs[] = {
    &SKULL_NONE1,
	&SKULL_NONE1,
	&SKULL_NONE1,
    &SKULL1,
    &SKULL2,
    &SKULL3,
    &SKULL_NONE2,
};

#define ANIMATION_SPEED_FAST 1000
const lv_img_dsc_t *fast_imgs[] = {
    &SKULL1,
    &SKULL2,
    &SKULL3,
};

struct skull_wpm_status_state {
    uint8_t wpm;
};

enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_slow,
    anim_state_mid,
    anim_state_fast
} current_anim_state;

static void set_animation(lv_obj_t *animing, struct skull_wpm_status_state state) {
    if (state.wpm < 5) {
        if (current_anim_state != anim_state_idle) {
            lv_animimg_set_src(animing, SRC(idle_imgs));
            lv_animimg_set_duration(animing, ANIMATION_SPEED_IDLE);
            lv_animimg_set_repeat_count(animing, LV_ANIM_REPEAT_INFINITE);
            lv_animimg_start(animing);
            current_anim_state = anim_state_idle;
        }
    } else if (state.wpm < 30) {
        if (current_anim_state != anim_state_slow) {
            lv_animimg_set_src(animing, SRC(slow_imgs));
            lv_animimg_set_duration(animing, ANIMATION_SPEED_SLOW);
            lv_animimg_set_repeat_count(animing, LV_ANIM_REPEAT_INFINITE);
            lv_animimg_start(animing);
            current_anim_state = anim_state_slow;
        }
    } else if (state.wpm < 70) {
        if (current_anim_state != anim_state_mid) {
            lv_animimg_set_src(animing, SRC(mid_imgs));
            lv_animimg_set_duration(animing, ANIMATION_SPEED_MID);
            lv_animimg_set_repeat_count(animing, LV_ANIM_REPEAT_INFINITE);
            lv_animimg_start(animing);
            current_anim_state = anim_state_mid;
        }
    } else {
        if (current_anim_state != anim_state_fast) {
            lv_animimg_set_src(animing, SRC(fast_imgs));
            lv_animimg_set_duration(animing, ANIMATION_SPEED_FAST);
            lv_animimg_set_repeat_count(animing, LV_ANIM_REPEAT_INFINITE);
            lv_animimg_start(animing);
            current_anim_state = anim_state_fast;
        }
    }
}

struct skull_wpm_status_state skull_wpm_status_get_state(const zmk_event_t *eh) {
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    return (struct skull_wpm_status_state) { .wpm = ev->state };
};

void skull_wpm_status_update_cb(struct skull_wpm_status_state state) {
    struct zmk_widget_skull *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_animation(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_skull, struct skull_wpm_status_state,
                            skull_wpm_status_update_cb, skull_wpm_status_get_state)

ZMK_SUBSCRIPTION(widget_skull, zmk_wpm_state_changed);

int zmk_widget_skull_init(struct zmk_widget_skull *widget, lv_obj_t *parent) {
    widget->obj = lv_animimg_create(parent);
    lv_obj_center(widget->obj);

    sys_slist_append(&widgets, &widget->node);

    widget_skull_init();

    return 0;
}

lv_obj_t *zmk_widget_skull_obj(struct zmk_widget_skull *widget) {
    return widget->obj;
}