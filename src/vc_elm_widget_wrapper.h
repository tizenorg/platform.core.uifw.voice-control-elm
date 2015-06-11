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


#ifndef __VC_ELM_WIDGET_WRAPPER_H__
#define __VC_ELM_WIDGET_WRAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>

typedef int (*vc_elm_widget_wrapper_result_cb)(const char *cmd, const char *param1, const char *param2, void *data);

typedef void (*vc_elm_widget_wrapper_show_tooltips_callback)(bool show_or_hide);

typedef void (*vc_elm_widget_wrapper_language_changed_callback)(const char *previous, const char * current, void *data);

/**
 * @brief Function that initializes VC_Widget_Wrapper
 */
int _vc_elm_widget_wrapper_initialize();

/**
 * @brief Function that deinitializes VC_Widget_Wrapper
 */
int _vc_elm_widget_wrapper_deinitialize();

/**
 * @brief Function that adds a voice command
 * @param[in] command text
 * @param[in] parameter text
 */
int _vc_elm_widget_wrapper_add_command(const char *cmd, const char *param1);

/**
 * @brief Function shows registers all the added commands in voice control
 * @param[in] result callback for registered commands
 * @param[in] additional data to be passed to the callback
 */
int _vc_elm_widget_wrapper_commit_commands(vc_elm_widget_wrapper_result_cb callback, void *data);

/**
 * @brief Function that sets the show tooltips callback
 * @param[in] callback that will be called when tooltips should be shown/hidden
 */
void _vc_elm_widget_wrapper_set_show_tooltips_callback(vc_elm_widget_wrapper_show_tooltips_callback callback);

int vc_elm_widget_wrapper_set_current_language_changed_callback(vc_elm_widget_wrapper_language_changed_callback callback, void *user_data);

int vc_elm_widget_wrapper_unset_current_language_changed_callback();


#ifdef __cplusplus
}
#endif

#endif/*__VC_ELM_WIDGET_WRAPPER_H__*/
