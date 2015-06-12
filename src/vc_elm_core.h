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


#ifndef __VC_ELM_CORE_H__
#define __VC_ELM_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <voice_control_elm.h>
/**
 * @file vc_elm_core.h
 * @brief This file contains core functions, that define the behaviour of library
 */

#define VC_ELM_CONFIG_XML 		"/usr/lib/voice/vc-elm/1.0/voice-control-elm-config.xml"
#define VC_ELM_TAG_BASE_NAME		"voice-control-elm-config"

#define VC_ELM_TAG_ACTION_LIST		"action-list"
#define VC_ELM_TAG_ACTION_GROUP		"action"
#define VC_ELM_TAG_ACTION_NAME		"action-name"
#define VC_ELM_TAG_ACTION_COMMAND	"action-command"

#define VC_ELM_TAG_WIDGET_LIST		"widget-list"
#define VC_ELM_TAG_WIDGET_GROUP		"widget"
#define VC_ELM_TAG_WIDGET_NAME		"widget-name"
#define VC_ELM_TAG_WIDGET_ACTIONS	"actions"

#define VC_ELM_TAG_HINT_GROUP		"hint"
#define VC_ELM_TAG_HINT_SHOW		"hint-show"
#define VC_ELM_TAG_HINT_DIRECTION	"default-hint-direction"
#define VC_ELM_TAG_HINT_IMAGE		"default-hint-image"

#define VC_ELM_TAG_BASE_INFO		"voice-control-elm-info"
#define VC_ELM_TAG_TIZEN_VERSION	"tizen-version"
#define VC_ELM_TAG_PROFILE		"profile"
#define VC_ELM_TAG_EFL_VERSION		"efl-version"

#define VC_ELM_HINT_DATA_KEY		"vc_elm_hint_data_key"
#define VC_ELM_CMD_DATA_KEY		"vc_elm_cmd_data_key"
#define VC_ELM_CUSTOM_WIDGET_NAME	"VC_ELM_CUSTOM_WIDGET_NAME"
#define VC_ELM_DIRECTION		"vc_elm_direction"
#define VC_ELM_SUB_ITEM_DIRECTION	"vc_elm_sub_item_direction"

/**
 * @brief This array holds names for autogenerated tooltips
 */
static const char *const autogen_names[] =
	{"IDS_ALPHA", "IDS_BRAVO", "IDS_CHARLIE", "IDS_DELTA", "IDS_ECHO",
	"IDS_FOXTROT", "IDS_GOLF", "IDS_HOTEL", "IDS_INDIA", "IDS_JULIET",
	"IDS_KILO", "IDS_LIMA", "IDS_MIKE", /*"IDS_NOVEMBER",*/"IDS_OSCAR",
	"IDS_PAPA", "IDS_QUEBEC", "IDS_ROMEO", "IDS_SIERRA", "IDS_TANGO",
	"IDS_UNIFORM", "IDS_VICTOR", "IDS_WHISKEY", "IDS_X_RAY", "IDS_YANKEE",
	"IDS_ZULU", "IDS_ZORRO", "IDS_CASTLE", "IDS_ONE", "IDS_TWO",
	"IDS_THREE", "IDS_FOUR", "IDS_FIVE", "IDS_SIX", "IDS_SEVEN",
	"IDS_EIGHT", "IDS_NINE", "IDS_TEN", "IDS_ELEVEN", "IDS_TWELVE"};

/**
 * @brief A handle to the dynamic action info structure
 */
typedef struct vc_elm_core_dynamic_action_info vc_elm_dynamic_action_info;

/**
 * @brief A handle to the widget action info structure
 */
typedef struct vc_elm_core_action_info vc_elm_action_info;

/**
 * @brief A handle to the widget info structure
 */
typedef struct vc_elm_core_widget_info vc_elm_widget_info;

/**
 * @brief A callback function type to be called to get wigdet subobjects list
 */
typedef Eina_List *(*vc_elm_get_subobjects_cb)(const Evas_Object *obj, void *user_data);

/**
 * @brief A callback function type to be called to filter allowed commands to be used for a widget.
 */
typedef Eina_Bool (*vc_elm_command_filter_cb)(Evas_Object *obj, const char *text, void *user_data);

/**
 * @brief A callback function type to be called to process action when an allowed command recognized.
 */
typedef void (*vc_elm_action_activator_cb)(Evas_Object *object, void *data, const char *action_name);

const char *_vc_elm_get_custom_widget_name();

const char *_get_ui_object_name(const Evas_Object *obj);

/**
 * @brief The vc_elm_core_dynamic_action_info struct is responsible for holding dynamic action info
 */
struct vc_elm_core_dynamic_action_info {
	void *data;
	void **items;
	unsigned int items_count;
	void (*activate_item_tooltip_func)(void *item, void *data);
	void (*show_item_tooltip_func)(const char *hint, void *item, void *data);
	void (*hide_item_tooltip_func)(void *item, void *data);
	void (*free_dynamic_action_info)(struct vc_elm_core_dynamic_action_info *dinfo, void *data);
};

/**
 * @brief The vc_elm_core_action_info struct is responsible for holding action info
 */
struct vc_elm_core_action_info {
	const char *name;
	void *data;
	vc_elm_action_activator_cb action_activator_func;
};

/**
 * @brief The vc_elm_core_widget_info struct is responsible for holding widget info
 */
struct vc_elm_core_widget_info {
	const char *name;
	struct vc_elm_core_action_info *actions;
	unsigned int actions_count;
	vc_elm_get_subobjects_cb get_subobjects_func;
	vc_elm_command_filter_cb is_filtered_func;
	void *user_data;
};

/**
 * @brief vc_elm_core_init function is responsible for initializing voice-control-elm module
 *
 * @see vc-elm-initialize
 * @see vc-elm-shutdown
 * @see vc_elm_core_fini
 */
void _vc_elm_core_init();
/**
 * @brief vc_elm_core_fini function is responsible for deinitializing voice-control-elm module
 *
 * @see vc-elm-initialize
 * @see vc-elm-shutdown
 * @see vc_elm_core_init
 */
void _vc_elm_core_fini();
void _vc_elm_core_load();
#if 0
unsigned int _vc_elm_core_get_window();
#endif
Evas_Object *_vc_elm_core_get_evas_object(Elm_Object_Item *item);

#ifdef SRPOL_DEBUG
/**
 * @brief _show_tooltips function runs steps necessary to show tooltips on screen
 * @details This function may be invoked from outside only for test purposes
 *
 * @see _hide_tooltips
 */
void _show_tooltips();
/**
 * @brief _hide_tooltips function runs steps necessary to hide tooltips from screen
 * @details This function may be invoked from outside only for test purposes
 */
void _hide_tooltips();
#endif

/**
 * @brief vc_elm_core_find_all_visible_objects function searches for visible objects on screen
 * @return Eina_List *list of visible objects or NULL if no default window
 */
Eina_List *_vc_elm_core_find_all_visible_objects();

/**
 * @brief vc_elm_core_register_widget function registers widget in voice-control-elm module, to enable voice
 *	commands on it
 * @param widget_name the name of the widget
 * @param widget_hint_name the hint do be displayed over the widget
 * @param focus_enabled_func pointer to function invoked when widget gets focused
 * @param get_subobjects_func pointer to function for a widget with subwidgets
 * @param is_filtered_func pointer to function enabling filtering widget
 * @return vc_elm_core_widget_info struct pointer or null if already registered
 */
struct vc_elm_core_widget_info *_vc_elm_core_register_widget(const char *widget_name, vc_elm_get_subobjects_cb get_subobjects_func, vc_elm_command_filter_cb is_filtered_func, void *user_data);

/**
 * @brief vc_elm_core_register_action function registers action to be performed on widget in voice-control-elm
 *	module
 * @param widget Pointer to vc_elm_core_action_info object
 * @param action_name Name of action
 * @param data Additional data to be passed to action_activator_func
 * @param action_activator_func function to be invoked when action is activated
 */
void _vc_elm_core_register_action(struct vc_elm_core_widget_info *widget, const char *action_name, void *data, vc_elm_action_activator_cb action_activator_func);

/**
 * @brief vc_elm_core_default_widgets_register function, registers all supported default widgets in
 *	voice-control-elm module
 */
void _vc_elm_core_register_default_widgets();

/**
 * @brief vc_elm_core_default_widgets_register function, registers selected widget in
 *	voice-control-elm module
 */
Eina_Bool _vc_elm_core_register_default_widget(const char *widget_name, Eina_Bool register_widget, Eina_List *actions);

/**
 * @brief vc_elm_actions_equal function checks is two given actions are the same
 * @param action1 name of first action
 * @param action2 name of second action
 * @return EINA_TRUE if equal or EINA_FALSE if not equal
 */
Eina_Bool _vc_elm_core_equal_actions(const char *action1, const char *action2);

/**
 * @brief vc_elm_core_object_hint_set function sets custom hint for object
 * @param obj pointer to desired widget
 * @param hint hint to be displayed
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_unset_object_hint
 */
Eina_Bool _vc_elm_core_set_object_hint(Evas_Object *obj, const char *hint);

/**
 * @brief vc_elm_core_object_hint_unset function unsets custom hint from object
 * @param obj pointer to desired widget
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_set_object_hint
 */
Eina_Bool _vc_elm_core_unset_object_hint(Evas_Object *obj);

/**
 * @brief vc_elm_core_object_command_set function sets command for object
 * @param obj pointer to desired widget
 * @param command command to activate widget
 * @return EINA_TRUE if successful of EINA_FALSE otherwise
 *
 * @see vc_elm_core_unset_object_command
 */
Eina_Bool _vc_elm_core_set_object_command(Evas_Object *obj, const char *command);

/**
 * @brief vc_elm_core_object_command_unset function unsets command from object
 * @param obj pointer to desired widget
 * @return EINA_TRUE if successful of EINA_FALSE otherwise
 *
 * @see vc_elm_core_set_object_command
 */
Eina_Bool _vc_elm_core_unset_object_command(Evas_Object *obj);

/**
 * @brief vc_elm_core_item_object_hint_set function sets hint for subitem
 * @param obj pointer to subitem
 * @param hint hint to be set
 * @param list pointer to subitem's parent
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_unset_item_object_hint
 */
Eina_Bool _vc_elm_core_set_item_object_hint(Elm_Object_Item *obj, const char *hint);

/**
 * @brief vc_elm_core_object_custom_hint_set function sets tooltip's image and position of tooltip
 * @param obj pointer to widget
 * @param image_path image path
 * @param pos_x x coordinate
 * @param pos_y y coordinate
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_get_object_custom_hint
 */
Eina_Bool _vc_elm_core_set_object_custom_hint(Evas_Object *obj, const char *image_path, int pos_x, int pos_y);

/**
 * @brief vc_elm_core_get_object_custom_hint function gets tooltip's image and position of tooltip
 * @param obj pointer to widget
 * @param image_path image path
 * @param pos_x x coordinate
 * @param pos_y y coordinate
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_set_item_object_custom_hint
 */
Eina_Bool _vc_elm_core_get_object_custom_hint(Evas_Object *obj, const char **image_path, int *pos_x, int *pos_y);

/**
 * @brief vc_elm_core_item_object_custom_hint_set function sets tooltip's image and position of tooltip
 * @param obj pointer to subitem
 * @param image_path image path
 * @param pos_x x coordinate
 * @param pos_y y coordinate
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_get_item_object_custom_hint
 */
Eina_Bool _vc_elm_core_set_item_object_custom_hint(Elm_Object_Item *obj, const char *image_path, int pos_x, int pos_y);

/**
 * @brief vc_elm_core_get_item_object_custom_hint function gets tooltip's image and position of tooltip
 * @param obj pointer to subitem
 * @param image_path image path
 * @param pos_x x coordinate
 * @param pos_y y coordinate
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_set_item_object_custom_hint
 */
Eina_Bool _vc_elm_core_get_item_object_custom_hint(Elm_Object_Item *obj, const char **image_path, int *pos_x, int *pos_y);

/**
 * @brief vc_elm_core_object_hint_get function gets object's hint
 * @param obj pointer to object
 * @return hint for object
 */
const char *_vc_elm_core_get_object_hint(const Evas_Object *obj);

/**
 * @brief vc_elm_core_object_skip_set function marks a widget that hint is not to be set
 * @param obj pointer to widget
 * @return EINA_TRUE if succesful or EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_set_skip_object(Evas_Object *obj);

/**
 * @brief vc_elm_core_object_skip_get function checking if widget's hint is to be set
 * @param obj pointer to widget
 * @return EINA_TRUE if true, EINA_FALSE if not
 */
Eina_Bool _vc_elm_core_get_skip_object(const Evas_Object *obj);

/**
 * @brief vc_elm_core_item_object_hint_unset function unsets hint from subitem
 * @param obj pointer to subitem
 * @param list pointer to subitem's parent
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_set_item_object_hint
 */
Eina_Bool _vc_elm_core_unset_item_object_hint(Elm_Object_Item *obj);

/**
 * @brief vc_elm_core_item_object_command_set function sets command for subitem
 * @param obj pointer to subitem
 * @param command command to activate subitem
 * @param list pointer to subitem's parent
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_unset_item_object_command
 */
Eina_Bool _vc_elm_core_set_item_object_command(Elm_Object_Item *obj, const char *command);

/**
 * @brief vc_elm_core_item_object_command_unset function unsets command from subitem
 * @param obj pointer to subitem
 * @param list pointer to subitem's parent
 * @return EINA_TRUE if successful or EINA_FALSE otherwise
 *
 * @see vc_elm_core_set_item_object_command
 */
Eina_Bool _vc_elm_core_unset_item_object_command(Elm_Object_Item *obj);

/**
 * @brief vc_elm_core_object_hint_fixed_possition_set function sets fixed position of tooltip
 * @param obj pointer to widget
 * @param x x coordinate
 * @param y y coordinate
 * @return EINA_TRUE if successful, EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_set_object_hint_fixed_possition(Evas_Object *obj, int x, int y);

/**
 * @brief vc_elm_core_object_hint_fixed_possition_get function gets fixed position of tooltip
 * @param obj pointer to widget
 * @param x x coordinate
 * @param y y coordinate
 * @return EINA_TRUE if successful, EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_get_object_hint_fixed_possition(Evas_Object *obj, int *x, int *y);

/**
 * @brief vc_elm_core_object_hint_fixed_possition_del function removes tooltip's fixed position
 * @param obj pointer to widget
 * @return EINA_TRUE if successful, EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_del_object_hint_fixed_possition(Evas_Object *obj);

/**
 * @brief vc_elm_core_object_hint_direction_set function sets direction of widget's tooltip
 * @param obj pointer to widget
 * @param direction direction as defined by vc_elm_direction_e
 * @return EINA_TRUE if successful, EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_set_object_hint_direction(Evas_Object *obj, vc_elm_direction_e direction);

/**
 * @brief vc_elm_core_object_hint_direction_get function gets tooltip's direction
 * @param obj pointer to widget
 * @param direction direction of the tooltip
 * @return EINA_TRUE if successfull, EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_get_object_hint_direction(Evas_Object *obj, vc_elm_direction_e *direction);

/**
 * @brief vc_elm_core_object_hint_direction_del function unsets tooltip's direction
 * @param obj pointer to widget
 * @return EINA_TRUE if successfull, EINA_FALSE otherwise
 */
Eina_Bool _vc_elm_core_del_object_hint_direction(Evas_Object *obj);

/**
 * @brief vc_elm_core_name_autogen_enable function enables or disables tooltips autogeneration
 * @param ag EINA_TRUE if enable autogenerate, EINA_FALSE if not
 * @return EINA_TRUE
 */
Eina_Bool _vc_elm_core_enable_name_autogen(Eina_Bool ag);

/**
 * @brief vc_elm_core_get_visible_item function returns first visible subitem of widget
 * @param parent pointer to parent widget
 * @return first visible item
 */
Elm_Object_Item *_vc_elm_core_get_visible_item(Evas_Object *parent, Elm_Object_Item *(*get_first)(const Evas_Object *obj, void *user_data), Elm_Object_Item *(*get_next)(const Elm_Object_Item *item, void *user_data), void *user_data);

/**
 * @brief vc_elm_core_sub_item_hint_direction_set function sets direction of subwidget's tooltips
 * @param obj pointer to parent object
 * @param direction direction of tooltips as defined in vc_elm_sub_item_direction_e
 * @return
 */
Eina_Bool _vc_elm_core_set_sub_item_hint_direction(Evas_Object *obj, vc_elm_direction_e direction);
/**
 * @brief vc_elm_core_sub_item_hint_direction_get function gets direction of subwidget's tooltips
 * @param obj pointer to parent object
 * @param direction pointer to direction object as defined in vc_elm_sub_item_direction_e
 * @return
 */
Eina_Bool _vc_elm_core_get_sub_item_hint_direction(Evas_Object *obj, vc_elm_direction_e *direction);

/**
 * @brief vc_elm_core_read_xml_data function reads and parses configurable xml file
 * @return
 */
int _vc_elm_core_read_xml_data();

/**
 * @brief vc_elm_core_destroy_xml_data function removes configurable xml variables
 * @return
 */
int _vc_elm_core_destroy_xml_data();

/**
 * @brief vc_elm_core_get_tooltip_default_direction function returns tooltip
 * default direction specified in configuration file
 * @return default direction enum vc_elm_direction_e value
 */
int _vc_elm_core_get_tooltip_default_direction();

/**
 * @brief vc_elm_core_get_tooltip_image_path function returns if tooltip should
 * be displayed or not according to configuration file
 * @return EINA_TRUE if tooltips should be displayed, otherwise EINA_FALSE
 */
const char *_vc_elm_core_get_tooltip_image_path();

/**
 * @brief vc_elm_core_get_tooltip_show function returns tooltip
 * char table
 * @return path to the image file
 */
int _vc_elm_core_get_tooltip_show();

/**
 * @brief _vc_elm_core_get_config_action_map function returns hash map
 * of actions registered in library
 * @return path to the image file
 */
const Eina_Hash *_vc_elm_core_get_config_action_map();

/**
 * @brief _vc_elm_core_get_config_widget_map function returns hash map
 * of widgets registered in library
 * @return path to the image file
 */
const Eina_Hash *_vc_elm_core_get_config_widget_map();


#ifdef __cplusplus
}
#endif

#endif/*__VC_ELM_CORE_H__*/
