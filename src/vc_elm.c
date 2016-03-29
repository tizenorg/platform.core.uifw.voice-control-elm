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

#include <system_info.h>
#include "vc_elm_main.h"
#include <voice_control_elm.h>

#include "vc_elm.h"
#include "vc_elm_core.h"
#include "vc_elm_tools.h"
#include "vc_elm_tooltip.h"
#include "vc_elm_widget_wrapper.h"
#include <voice_control_widget.h>

/**
* @brief Internal variable. It can check whether voice control elm is initilaized or not.
*/
static unsigned is_vc_elm_initialized = false;

/**
 * @brief App domain name of the current app.
 */
static char *g_app_domain = NULL;
static Eina_List *g_handlers_list = NULL;

typedef enum {
	VC_ELM_TYPE_EVAS_OBJECT = 0, VC_ELM_TYPE_ITEM = 1
} vc_elm_type_e;

struct __vc_elm_s {
	vc_elm_type_e type;
	intptr_t *data;
};

typedef struct __vc_elm_s vc_elm_s;

struct __vc_elm_widget_cb_data_s {
	vc_elm_widget_cb callback;
	void *data;
};

typedef struct __vc_elm_widget_cb_data_s vc_elm_widget_cb_data_s;

static int g_feature_enabled = -1;

static int __vc_elm_get_feature_enabled()
{
	if (0 == g_feature_enabled) {
		VC_ELM_LOG_DBG("[ERROR] Voice control feature NOT supported");
		return VC_ELM_ERROR_NOT_SUPPORTED;
	} else if (-1 == g_feature_enabled) {
		bool vc_supported = false;
		bool mic_supported = false;
		if (0 == system_info_get_platform_bool(VC_ELM_FEATURE_PATH, &vc_supported)) {
			if (0 == system_info_get_platform_bool(VC_ELM_MIC_FEATURE_PATH, &mic_supported)) {
				if (false == vc_supported || false == mic_supported) {
					VC_ELM_LOG_DBG("[ERROR] Voice control feature NOT supported");
					g_feature_enabled = 0;
					return VC_ELM_ERROR_NOT_SUPPORTED;
				}

				g_feature_enabled = 1;
			} else {
				VC_ELM_LOG_DBG("[ERROR] Fail to get feature value");
				return VC_ELM_ERROR_NOT_SUPPORTED;
			}
		} else {
			VC_ELM_LOG_DBG("[ERROR] Fail to get feature value");
			return VC_ELM_ERROR_NOT_SUPPORTED;
		}
	}
	return 0;
}

int vc_elm_initialize()
{
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}

	if (true == is_vc_elm_initialized) {
		VC_ELM_LOG_DBG("vc elm is already initialized");
		return VC_ELM_ERROR_INVALID_STATE;
	}

	eina_log_level_set(EINA_LOG_LEVEL_DBG);

	_vc_elm_core_enable_name_autogen(EINA_FALSE);
	_vc_elm_core_init();

	g_handlers_list = NULL;
	_vc_elm_core_load();

	is_vc_elm_initialized = true;
	VC_ELM_LOG_DBG("vc elm is initialized");
	return VC_ELM_ERROR_NONE;
}

int vc_elm_deinitialize()
{
	Eina_List *l;
	vc_elm_h handler = NULL;

	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}

	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_DBG("vc elm is already deinitialized");
		return VC_ELM_ERROR_INVALID_STATE;
	}

	if (NULL != g_handlers_list) {
		EINA_LIST_FOREACH(g_handlers_list, l, handler) {
			vc_elm_unset_command(handler);
			vc_elm_unset_command_hint(handler);
			vc_elm_destroy(handler);
		}
		eina_list_free(g_handlers_list);
	}

	_vc_elm_core_fini();

	if (NULL != g_app_domain)
		free(g_app_domain);

	is_vc_elm_initialized = false;
	VC_ELM_LOG_DBG("shutting down vc_elm");
	return VC_ELM_ERROR_NONE;
}

int vc_elm_foreach_supported_languages(vc_elm_supported_language_cb callback, void *user_data)
{
	int ret;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}

	ret = vc_widget_foreach_supported_languages(callback, user_data);
	switch (ret) {
	case VC_ERROR_NONE:
		ret = VC_ELM_ERROR_NONE;
		break;
	case VC_ERROR_INVALID_PARAMETER:
		ret = VC_ELM_ERROR_INVALID_PARAMETER;
		break;
	case VC_ERROR_INVALID_STATE:
		ret = VC_ELM_ERROR_INVALID_STATE;
		break;
	default:
		ret = VC_ELM_ERROR_OPERATION_FAILED;
		break;
	}
	return ret;
}

int vc_elm_get_current_language(char **language)
{
	int ret;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}

	ret = vc_widget_get_current_language(language);
	switch (ret) {
	case VC_ERROR_NONE:
		ret = VC_ELM_ERROR_NONE;
		break;
	case VC_ERROR_INVALID_PARAMETER:
		ret = VC_ELM_ERROR_INVALID_PARAMETER;
		break;
	case VC_ERROR_OUT_OF_MEMORY:
		ret = VC_ELM_ERROR_OUT_OF_MEMORY;
		break;
	case VC_ERROR_INVALID_STATE:
		ret = VC_ELM_ERROR_INVALID_STATE;
		break;
	default:
		ret = VC_ELM_ERROR_OPERATION_FAILED;
		break;
	}
	return ret;
}

static Eina_Bool __hash_fn(const Eina_Hash *hash, const void *key, void *kdata, void *fdata)
{
	vc_elm_widget_cb_data_s *data;
	vc_elm_widget_cb callback;
	(void)hash;
	(void)kdata;
	data = (vc_elm_widget_cb_data_s *)fdata;
	callback = data->callback;
	callback(key, data->data);
	return EINA_TRUE;
}

int vc_elm_foreach_supported_widgets(vc_elm_widget_cb callback, void *user_data)
{
	vc_elm_widget_cb_data_s data;
	const Eina_Hash  *hash = NULL;

	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == callback) {
		VC_ELM_LOG_ERR("Invalid parameters detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}
	data.callback = callback;
	data.data = user_data;
	hash = _vc_elm_core_get_config_widget_map();
	if (NULL == hash) {
		VC_ELM_LOG_ERR("Config widget map is NULL!");
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	eina_hash_foreach(hash, __hash_fn, &data);
	return VC_ELM_ERROR_NONE;
}

int vc_elm_foreach_supported_actions(const char *widget, vc_elm_action_cb callback, void *user_data)
{
	const Eina_Hash *hash = NULL;
	Eina_List *actions_list = NULL;
	Eina_List *l = NULL;
	const char *action_tag = NULL;

	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if ((NULL == widget) || (NULL == callback)) {
		VC_ELM_LOG_ERR("Invalid parameters detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}
	hash = _vc_elm_core_get_config_widget_map();
	if (NULL == hash) {
		VC_ELM_LOG_ERR("Config widget map is NULL!");
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	actions_list = (Eina_List *)eina_hash_find(hash, widget);
	if (NULL == actions_list) {
		VC_ELM_LOG_ERR("Action list is NULL!");
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	EINA_LIST_FOREACH(actions_list, l, action_tag) {
		if (NULL != action_tag) {
			callback(action_tag, user_data);
		}
	}
	return VC_ELM_ERROR_NONE;
}

int vc_elm_get_action_command(const char *action, char **command)
{
	const Eina_Hash *hash = NULL;
	const char *command_name = NULL;
	size_t len = 0;

	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if ((NULL == action) || (NULL == command)) {
		VC_ELM_LOG_ERR("Invalid parameters detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}
	hash = _vc_elm_core_get_config_action_map();
	if (NULL == hash) {
		VC_ELM_LOG_ERR("Config action map is NULL!");
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	command_name = eina_hash_find(_vc_elm_core_get_config_action_map(), action);
	if (NULL == command_name) {
		VC_ELM_LOG_ERR("No command for given action name!");
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	len = strlen(command_name) + 1;
	*command = (char *)calloc(len, sizeof(char));
	if (NULL == *command) {
		VC_ELM_LOG_ERR("Can not allocate memory!");
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	memcpy(*command, command_name, len);
	return VC_ELM_ERROR_NONE;
}

int vc_elm_create_object(Evas_Object *object, vc_elm_h *vc_elm)
{
	vc_elm_s *handler = NULL;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}

	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid parameters detected! (vc_elm_h *) poiter is NULL");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if ((NULL == object) || (!elm_object_widget_check(object))) {
		VC_ELM_LOG_ERR("Invalid parameters detected! Incorrect (Evas_Object *)");
		*vc_elm = NULL;
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}
	if (!_vc_elm_core_register_default_widget(_get_ui_object_name(object), EINA_FALSE, NULL) || (NULL == eina_hash_find(_vc_elm_core_get_config_widget_map(), elm_widget_type_get(object)))) {
		VC_ELM_LOG_ERR("Not supported widget");
		*vc_elm = NULL;
		return VC_ELM_ERROR_NOT_SUPPORTED;
	}
	handler = (vc_elm_s *)calloc(1, sizeof(vc_elm_s));
	if (NULL == handler) {
		VC_ELM_LOG_ERR("Calloc function returned NULL");
		return VC_ELM_ERROR_OUT_OF_MEMORY;
	}
	handler->type = VC_ELM_TYPE_EVAS_OBJECT;
	handler->data = (void *)object;
	g_handlers_list = eina_list_append(g_handlers_list, handler);
	*vc_elm = (vc_elm_h)handler;
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_NONE;
}

int vc_elm_create_item(Elm_Object_Item *item, vc_elm_h *vc_elm)
{
	vc_elm_s *handler = NULL;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}

	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid parameters detected! (vc_elm_h *) poiter is NULL");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if (NULL == item) {
		VC_ELM_LOG_ERR("Invalid parameters detected! (Elm_Object_Item *) poiter is NULL");
		*vc_elm = NULL;
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}
	handler = (vc_elm_s *)calloc(1, sizeof(vc_elm_s));
	if (NULL == handler) {
		VC_ELM_LOG_ERR("Calloc function returned NULL");
		*vc_elm = NULL;
		return VC_ELM_ERROR_OUT_OF_MEMORY;
	}
	handler->type = VC_ELM_TYPE_ITEM;
	handler->data = (void *)item;
	g_handlers_list = eina_list_append(g_handlers_list, handler);
	*vc_elm = (vc_elm_h)handler;
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_NONE;
}

int vc_elm_destroy(vc_elm_h vc_elm)
{
	vc_elm_s *handler = NULL;
	Eina_List *list = NULL;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	handler = (vc_elm_s *)vc_elm;
	type = handler->type;
	if (!((VC_ELM_TYPE_EVAS_OBJECT == type) || (VC_ELM_TYPE_ITEM == type))) {
		VC_ELM_LOG_ERR("Invalid type detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}

	list = eina_list_data_find_list(g_handlers_list, handler);
	if (NULL == list) {
		VC_ELM_LOG_ERR("Bad handler pointer detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	g_handlers_list = eina_list_remove_list(g_handlers_list, list);
	free(handler);
	handler = NULL;
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_NONE;
}

int vc_elm_set_command(vc_elm_h vc_elm, const char *command)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_set_object_command(obj, command);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		return _vc_elm_set_item_object_command(it, command);
	}
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_unset_command(vc_elm_h vc_elm)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_unset_object_command(obj);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		return _vc_elm_unset_item_object_command(it);
	}
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_set_command_hint(vc_elm_h vc_elm, const char* hint)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_set_object_hint(obj, hint);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		return _vc_elm_set_item_object_hint(it, hint);
	}
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_unset_command_hint(vc_elm_h vc_elm)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_unset_object_hint(obj);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		return _vc_elm_unset_item_object_hint(it);
	}
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_set_command_hint_direction(vc_elm_h vc_elm, vc_elm_direction_e direction)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_set_object_hint_direction(obj, direction);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		Evas_Object *parent = elm_object_item_widget_get(it);
		return _vc_elm_set_sub_item_hint_direction(parent, direction);
	}
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_get_command_hint_direction(vc_elm_h vc_elm, vc_elm_direction_e *direction)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_get_object_hint_direction(obj, direction);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		Evas_Object *parent = elm_object_item_widget_get(it);
		return _vc_elm_get_sub_item_hint_direction(parent, direction);
	}
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_set_command_hint_offset(vc_elm_h vc_elm, int pos_x, int pos_y)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_set_object_custom_hint(obj, NULL, pos_x, pos_y);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		return _vc_elm_set_item_object_custom_hint(it, NULL, pos_x, pos_y);
	}
	_vc_elm_core_set_view_changed();
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_get_command_hint_offset(vc_elm_h vc_elm, int *pos_x, int *pos_y)
{
	vc_elm_s *handler = (vc_elm_s *)vc_elm;
	const char *path = NULL;
	int type = 0;
	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (NULL == vc_elm) {
		VC_ELM_LOG_ERR("Invalid vc_elm parameter detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	if ((NULL == pos_x) || (NULL == pos_y)) {
		VC_ELM_LOG_ERR("Invalid position poiters detected!");
		return VC_ELM_ERROR_INVALID_PARAMETER;
	}
	type = handler->type;
	if (VC_ELM_TYPE_EVAS_OBJECT == type) {
		Evas_Object *obj = (Evas_Object *)handler->data;
		return _vc_elm_get_object_custom_hint(obj, &path, pos_x, pos_y);
	} else if (VC_ELM_TYPE_ITEM == type) {
		Elm_Object_Item *it = (Elm_Object_Item *)handler->data;
		return _vc_elm_get_item_object_custom_hint(it, &path, pos_x, pos_y);
	}
	return VC_ELM_ERROR_INVALID_PARAMETER;
}

int vc_elm_set_current_language_changed_cb(vc_elm_current_language_changed_cb callback, void *user_data)
{
	int ret = VC_ELM_ERROR_NONE;

	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}

	ret = _vc_elm_widget_wrapper_set_current_language_changed_callback(callback, user_data);
	if (0 != ret) {
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	return VC_ELM_ERROR_NONE;
}

int vc_elm_unset_current_language_changed_cb(void)
{
	int ret = VC_ELM_ERROR_NONE;

	if (0 != __vc_elm_get_feature_enabled()) {
		return VC_ERROR_NOT_SUPPORTED;
	}
	if (false == is_vc_elm_initialized) {
		VC_ELM_LOG_ERR("Invalid state detected! Library not initialized!");
		return VC_ELM_ERROR_INVALID_STATE;
	}

	ret = _vc_elm_widget_wrapper_unset_current_language_changed_callback();
	if (0 != ret) {
		return VC_ELM_ERROR_OPERATION_FAILED;
	}
	return VC_ELM_ERROR_NONE;
}

int _vc_elm_set_object_command(Evas_Object *obj, const char *command)
{
	if (_vc_elm_core_set_object_command(obj, command))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_unset_object_command(Evas_Object *obj)
{
	if (_vc_elm_core_unset_object_command(obj))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_object_hint(Evas_Object *obj, const char *hint)
{
	if (_vc_elm_core_set_object_hint(obj, hint))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_unset_object_hint(Evas_Object *obj)
{
	if (_vc_elm_core_unset_object_hint(obj))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_object_custom_hint(Evas_Object *obj, const char *image_path, int pos_x, int pos_y)
{
	if (_vc_elm_core_set_object_custom_hint(obj, image_path, pos_x, pos_y))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_get_object_custom_hint(Evas_Object *obj, const char **image_path, int *pos_x, int *pos_y)
{
	if (_vc_elm_core_get_object_custom_hint(obj, image_path, pos_x, pos_y))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_item_object_custom_hint(Elm_Object_Item *obj, const char *image_path, int pos_x, int pos_y)
{
	if (_vc_elm_core_set_item_object_custom_hint(obj, image_path, pos_x, pos_y))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_get_item_object_custom_hint(Elm_Object_Item *obj, const char **image_path, int *pos_x, int *pos_y)
{
	if (_vc_elm_core_get_item_object_custom_hint(obj, image_path, pos_x, pos_y))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_item_object_command(Elm_Object_Item *obj, const char *command)
{
	if (_vc_elm_core_set_item_object_command(obj, command))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_unset_item_object_command(Elm_Object_Item *obj)
{
	if (_vc_elm_core_unset_item_object_command(obj))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_item_object_hint(Elm_Object_Item *obj, const char *hint)
{
	if (_vc_elm_core_set_item_object_hint(obj, hint))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_unset_item_object_hint(Elm_Object_Item *obj)
{
	if (_vc_elm_core_unset_item_object_hint(obj))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_enable_name_autogen_enable(Eina_Bool autogen)
{
	if (_vc_elm_core_enable_name_autogen(autogen))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

vc_elm_widget_info*
_vc_elm_register_widget(const char *widget_name, vc_elm_get_subobjects_cb get_subobjects_func, vc_elm_command_filter_cb is_feltered_func, void *user_data)
{
	return _vc_elm_core_register_widget(widget_name, get_subobjects_func, is_feltered_func, user_data);
}

void _vc_elm_register_action(vc_elm_widget_info *info, const char *action_name, void *data, vc_elm_action_activator_cb func)
{
	_vc_elm_core_register_action(info, action_name, data, func);
}

int _vc_elm_set_object_hint_direction(Evas_Object *obj, vc_elm_direction_e direction)
{
	if (_vc_elm_core_set_object_hint_direction(obj, direction))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_get_object_hint_direction(Evas_Object *obj, vc_elm_direction_e *direction)
{
	if (_vc_elm_core_get_object_hint_direction(obj, direction))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_sub_item_hint_direction(Evas_Object *obj, vc_elm_direction_e direction)
{
	if (_vc_elm_core_set_sub_item_hint_direction(obj, direction))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_get_sub_item_hint_direction(Evas_Object *obj, vc_elm_direction_e *direction)
{
	if (_vc_elm_core_get_sub_item_hint_direction(obj, direction))
		return VC_ELM_ERROR_NONE;
	else
		return VC_ELM_ERROR_OPERATION_FAILED;
}

int _vc_elm_set_text_domain(const char *domain, const char *locale_dir)
{

	if (NULL == domain)
		return VC_ELM_ERROR_OPERATION_FAILED;

	if (NULL != g_app_domain)
		free(g_app_domain);

	g_app_domain = strdup(domain);
	bindtextdomain(g_app_domain, locale_dir);

	return VC_ELM_ERROR_NONE;
}

int _vc_elm_get_text_domain(char **domain)
{
	if (NULL == g_app_domain) {
		*domain = NULL;
		return VC_ELM_ERROR_OPERATION_FAILED;
	}

	*domain = strdup(g_app_domain);
	return VC_ELM_ERROR_NONE;
}

#ifdef SRPOL_DEBUG
/**
 * @brief Wrapper for making internal function public - for automated test purposes
 */
void show_tooltip()
{
	_show_tooltips();
}

/**
 * @brief Wrapper for making internal function public - for automated test purposes
 */
void hide_tooltip()
{
	_hide_tooltips();
}
#endif
