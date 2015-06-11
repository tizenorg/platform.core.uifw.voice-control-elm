/*
 * Copyright (c) 2011-2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __VC_ELM_TOOLTIP_H__
#define __VC_ELM_TOOLTIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vc_elm_main.h"

typedef enum {
	VC_ELM_FIXED_X = 0, VC_ELM_FIXED_Y = 1,
	VC_ELM_ONCE_FIXED_X = 2, VC_ELM_ONCE_FIXED_Y = 3,
	VC_ELM_HINT_IMAGE_PATH = 4, VC_ELM_POSITION_X = 5,
	VC_ELM_POSITION_Y = 6
} vc_elm_data_key;

/**
 * @brief Function shows a tooltip over an Evas_Object
 * @param[in] pointer to Evas_Object structure that the tooltip belongs to
 * @param[in] text of the tooltip
 */
void _vc_elm_tooltips_show_tooltip(Evas_Object *obj, const char *text);

/**
 * @brief Function that sets the window that the tooltips will be shown on
 * @param[in] pointer to the current window
 */
void _vc_elm_set_tooltips_window(Evas_Object *win);

/**
 * @brief Function that shows all the tooltips that should be visible
 */
void _vc_elm_turn_off_tooltips();

/**
 * @brief Function that adds a tooltip to an Evas_Object
 * @param[in] pointer to Evas_Object structure that the tooltip belongs to
 * @param[in] text of the tooltip
 */
void _vc_elm_add_tooltip(Evas_Object *obj, const char *text);

/**
 * @brief Function that recalculates postitons of all the visible tooltips
 *		and shows them
 */
void _vc_elm_relayout_and_show_tooltips();

const char *_vc_elm_get_data_key(vc_elm_data_key key);

#ifdef __cplusplus
}
#endif

#endif/*__VC_ELM_TOOLTIP_H__*/
