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


#ifndef __VC_ELM_H__
#define __VC_ELM_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <voice_control_elm.h>
/**
 * @file vc_elm.h
 * @brief This file contains voice-control-elm functions, that define the behaviour of library
 */

int _vc_elm_set_object_command(Evas_Object *obj, const char *command);
int _vc_elm_unset_object_command(Evas_Object *obj);
int _vc_elm_set_object_hint(Evas_Object *obj, const char *hint);
int _vc_elm_unset_object_hint(Evas_Object *obj);
int _vc_elm_set_object_custom_hint(Evas_Object *obj, const char *image_path, int pos_x, int pos_y);
int _vc_elm_get_object_custom_hint(Evas_Object *obj, const char **image_path, int *pos_x, int *pos_y);
int _vc_elm_set_item_object_custom_hint(Elm_Object_Item *obj, const char *image_path, int pos_x, int pos_y);
int _vc_elm_get_item_object_custom_hint(Elm_Object_Item *obj, const char **image_path, int *pos_x, int *pos_y);
int _vc_elm_set_item_object_command(Elm_Object_Item *obj, const char *command);
int _vc_elm_unset_item_object_command(Elm_Object_Item *obj);
int _vc_elm_set_item_object_hint(Elm_Object_Item *obj, const char *hint);
int _vc_elm_unset_item_object_hint(Elm_Object_Item *obj);
int _vc_elm_set_object_hint_direction(Evas_Object *obj, vc_elm_direction_e direction);
int _vc_elm_get_object_hint_direction(Evas_Object *obj, vc_elm_direction_e *direction);
int _vc_elm_set_sub_item_hint_direction(Evas_Object *obj, vc_elm_direction_e direction);
int _vc_elm_get_sub_item_hint_direction(Evas_Object *obj, vc_elm_direction_e *direction);

/**
 * set text domain for multi-language support
 */
int _vc_elm_set_text_domain(const char *domain, const char *locale_dir);

/**
 * get text domain for multi-language support
 */
int _vc_elm_get_text_domain(char **domain);


#ifdef __cplusplus
}
#endif

#endif/*__VC_ELM_H__*/
