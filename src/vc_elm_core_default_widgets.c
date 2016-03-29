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


#include "vc_elm_main.h"
#include <math.h>
#include <limits.h>
#include <libintl.h>

#include "vc_elm.h"
#include "vc_elm_core.h"
#include "vc_elm_tools.h"
#include "vc_elm_tooltip.h"

#ifdef _
#undef _
#endif
#define _VC_ELM(S) dgettext("voice-control-elm", S)

#define DEGRESS_15 15.0
#define DEGRESS_90 90.0
#define DEGRESS_180 180.0

static const char SCROLL_UP[] = "IDS_SCROLL_UP";
static const char SCROLL_DOWN[] = "IDS_SCROLL_DOWN";
static const char CENTER[] = "IDS_CENTER";
static const char SCROLL_LEFT[] = "IDS_SCROLL_LEFT";
static const char SCROLL_RIGHT[] = "IDS_SCROLL_RIGHT";
static const char UP[] = "IDS_UP";
static const char DOWN[] = "IDS_DOWN";
static const char LEFT[] = "IDS_LEFT";
static const char RIGHT[] = "IDS_RIGHT";
static const char HOME[] = "IDS_HOME";
static const char END[] = "IDS_END";
static const char CLICK[] = "";

enum SCROLL_AMOUNT {
	LESS,
	LITTLE,
	MORE,
	MAXIMAL
};
enum SCROLL_TURN {
	HORIZONTAL,
	VERTICAL
};
enum SCROLL_DIRECTION {
	TO_BEGINNING,
	TO_END
};

static void __set_tooltips_position_on_visible_items(Evas_Object *parent, Elm_Object_Item *(*get_first)(const Evas_Object *obj, void *user_data), Elm_Object_Item *(*get_next)(const Elm_Object_Item *item, void *user_data), void *user_data);

/**
 * @brief Function for scrolling elementary object
 * @param Scrolable Evas_Object
 * @param enumeration value of amount of scroll
 * @param enumeration value of turn
 * @param enumeration value of direction
 */
static void __scroll_object(Evas_Object *scrollable, enum SCROLL_AMOUNT amount, enum SCROLL_TURN turn, enum SCROLL_DIRECTION direction)
{
	int x;
	int y;
	int w;
	int h;
	int mw;
	int mh;
	int pw;
	int ph;
	int *pos;
	int count;
	int sign;
	int maximal;

	elm_scroller_child_size_get(scrollable, &mw, &mh);
	elm_scroller_region_get(scrollable, &x, &y, &w, &h);
	evas_object_geometry_get(scrollable, NULL, NULL, &pw, &ph);
	VC_ELM_LOG_DBG("cur region %d %d %d %d", x, y, w, h);
	VC_ELM_LOG_DBG("max region %d %d", mw, mh);
	VC_ELM_LOG_DBG("page size %d %d", pw, ph);
	if (mw < 0)
		mw = 65535;
	if (mh < 0)
		mh = 65535;

	if (HORIZONTAL == turn) {
		pos = &x;
		count = w;
		maximal = mw - w;
	} else {
		pos = &y;
		count = h;
		maximal = mh - h;
	}

	if (TO_BEGINNING == direction) {
		sign = -1;
		maximal = 0;
	} else {
		sign = 1;
	}

	switch (amount) {
	case LESS:
		*pos += sign * ph / 2;
		break;
	case LITTLE:
		*pos += sign * count / 2;
		break;
	case MORE:
		*pos += sign * count * 2;
		break;
	case MAXIMAL:
		*pos = maximal;
		break;
	default:
		VC_ELM_LOG_ERR("assert failed");
		exit(-1);
	}

	VC_ELM_LOG_DBG("next region %d %d %d %d", x, y, w, h);
	elm_scroller_region_bring_in(scrollable, x, y, w, h);
}

/**
 * @brief Emulate mouse click function on point defined by coordinates.
 * @param Elementary object
 * @param x coordinates
 * @param y coordinates
 */
static void __emulate_mouse_click_full(const Evas_Object *object, char xpercent, char ypercent)
{
	int x;
	int y;
	int w;
	int h;
	int new_x;
	int new_y;
	Evas_Object *obj = __safe_efl_cast(object);
	evas_object_geometry_get(obj, &x, &y, &w, &h);
	new_x = (w * xpercent);
	new_x = div(new_x, 100).quot;
	new_x += x;

	new_y = (h * ypercent);
	new_y = div(new_y, 100).quot;
	new_y += y;
	evas_event_feed_mouse_move(evas_object_evas_get(obj), new_x, new_y, 0, NULL);
	evas_event_feed_mouse_down(evas_object_evas_get(obj), 1, EVAS_BUTTON_NONE, 1, NULL);
	evas_event_feed_mouse_up(evas_object_evas_get(obj), 1, EVAS_BUTTON_NONE, 2, NULL);
}

/**
 * @brief Emulate mouse click function
 */
static void __emulate_mouse_click(const Evas_Object *obj)
{
	__emulate_mouse_click_full(obj, 10, 75);
	/* because items with two line style are clickable on:
	 - text only in first line
	 - the whole second line */
}

/* Button widget ----------------------------------------------------------- */

/**
 * @brief Function that is activated on button action.
 * @param[in] button object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __button_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;
	VC_ELM_LOG_DBG("button activator func");

	if (!action_name || (0 == strcmp(action_name, _VC_ELM(CLICK)))) {
		VC_ELM_LOG_INFO("clicking down %s (%p)", elm_widget_type_get(obj), (void*)obj);
		__emulate_mouse_click(obj);
	} else {
		VC_ELM_LOG_INFO("bad action (%s) for %s", action_name, elm_widget_type_get(obj));
	}
}

/**
 * @brief Function that registers Evas_Object button from Elementary.
 */
static void __register_elm_button(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *button;
	VC_ELM_LOG_INFO("Registering elm_button in vc_elm");
	button = _vc_elm_core_register_widget("Elm_Button", NULL, NULL, NULL);
	if (NULL == button)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(button, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __button_activator_func);
	}
}

/**
 * @brief Function that filter subwidgets voice command of vc_elm_item object.
 * @param[in] vc_elm_item object
 * @param[in] action string that was activated
 */
static Eina_Bool __item_filter_func(Evas_Object *obj, const char *text, void *user_data)
{
	(void)text;
	(void)user_data;

	if (NULL == obj)
		return EINA_TRUE;
	return EINA_FALSE;
}

/**
 * @brief Function that registers Evas_Object vc_elm_item from Elementary.
 */
static void __register_vc_elm_item()
{
	struct vc_elm_core_widget_info *item;
	VC_ELM_LOG_INFO("Registering vc_elm_supported_item in vc_elm");
	item = _vc_elm_core_register_widget("evc-item", NULL, __item_filter_func, NULL);
	if (NULL == item)
		return;

	_vc_elm_core_register_action(item, CLICK, NULL, __button_activator_func);
}

/* Hoversel widget --------------------------------------------------------- */
/**
 * @brief Function that registers Evas_Object hoversel from Elementary.
 */
static void __register_elm_hoversel(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *hoversel;
	VC_ELM_LOG_INFO("Registering elm_hoversel - in VC_ELM");
	hoversel = _vc_elm_core_register_widget("Elm_Hoversel", NULL, NULL, NULL);
	if (NULL == hoversel)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(hoversel, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __button_activator_func);
	}
}

/* Gengrid widget ---------------------------------------------------------- */
/**
 * @brief Function that is activated on gengrid action.
 * @param[in] gengrid object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __gengrid_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;

	VC_ELM_LOG_INFO("activated gengrid with action %s", action_name);
	if (!action_name) {
		VC_ELM_LOG_WARN("no default action set for gengrid");
	} else if (elm_gengrid_horizontal_get(obj)) {
		if (!strcmp(_VC_ELM(LEFT), action_name))
			__scroll_object(obj, LITTLE, HORIZONTAL, TO_BEGINNING);
		else if (!strcmp(_VC_ELM(RIGHT), action_name))
			__scroll_object(obj, LITTLE, HORIZONTAL, TO_END);
		else if (!strcmp(_VC_ELM(SCROLL_LEFT), action_name))
			__scroll_object(obj, MORE, HORIZONTAL, TO_BEGINNING);
		else if (!strcmp(_VC_ELM(SCROLL_RIGHT), action_name))
			__scroll_object(obj, MORE, HORIZONTAL, TO_END);
		else if (!strcmp(_VC_ELM(HOME), action_name))
			__scroll_object(obj, MAXIMAL, HORIZONTAL, TO_BEGINNING);
		else if (!strcmp(_VC_ELM(END), action_name))
			__scroll_object(obj, MAXIMAL, HORIZONTAL, TO_END);
		else
			VC_ELM_LOG_WARN("unknown action");
	} else {
		if (!strcmp(_VC_ELM(UP), action_name))
			__scroll_object(obj, LITTLE, VERTICAL, TO_BEGINNING);
		else if (!strcmp(_VC_ELM(DOWN), action_name))
			__scroll_object(obj, LITTLE, VERTICAL, TO_END);
		else if (!strcmp(_VC_ELM(SCROLL_UP), action_name))
			__scroll_object(obj, MORE, VERTICAL, TO_BEGINNING);
		else if (!strcmp(_VC_ELM(SCROLL_DOWN), action_name))
			__scroll_object(obj, MORE, VERTICAL, TO_END);
		else if (!strcmp(_VC_ELM(HOME), action_name))
			__scroll_object(obj, MAXIMAL, VERTICAL, TO_BEGINNING);
		else if (!strcmp(_VC_ELM(END), action_name))
			__scroll_object(obj, MAXIMAL, VERTICAL, TO_END);
		else
			VC_ELM_LOG_WARN("unknown action");
	}
}

Elm_Object_Item *__gengrid_filter_first_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_gengrid_first_item_get(obj);
}

Elm_Object_Item *__gengrid_filter_item_next_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_gengrid_item_next_get(it);
}

Elm_Object_Item *__gengrid_filter_last_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_gengrid_last_item_get(obj);
}

Elm_Object_Item *__gengrid_filter_item_prev_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_gengrid_item_prev_get(it);
}

/**
 * @brief Function that filter subwidgets voice command of gengrid object.
 * @param[in] gengrid object
 * @param[in] action string that was activated
 */
static Eina_Bool __gengrid_filter_func(Evas_Object *obj, const char *action, void *user_data)
{
	int horizontal = elm_gengrid_horizontal_get(obj);
	(void)user_data;
	VC_ELM_LOG_DBG("filtering gengrid");
	if (NULL == action) { /* NULL means whole object */
		Elm_Object_Item *first = __gengrid_filter_first_item_get(obj, NULL);
		Elm_Object_Item *last = __gengrid_filter_last_item_get(obj, NULL);
		Elm_Object_Item *first_visible = _vc_elm_core_get_visible_item(obj, __gengrid_filter_first_item_get, __gengrid_filter_item_next_get, NULL);
		Elm_Object_Item *last_visible = _vc_elm_core_get_visible_item(obj, __gengrid_filter_last_item_get, __gengrid_filter_item_prev_get, NULL);

		if (first == first_visible && last == last_visible) {
			VC_ELM_LOG_DBG("filtered out gengrid");
			return EINA_TRUE;
		}
	} else if (!strcmp(action, _VC_ELM(UP)) || !strcmp(action, _VC_ELM(DOWN)) || !strcmp(action, _VC_ELM(SCROLL_UP)) || !strcmp(action, _VC_ELM(SCROLL_DOWN))) {
		if (horizontal)
			return EINA_TRUE;
		else
			return EINA_FALSE;
	} else if (!strcmp(action, _VC_ELM(LEFT)) || !strcmp(action, _VC_ELM(RIGHT)) || !strcmp(action, _VC_ELM(SCROLL_LEFT)) || !strcmp(action, _VC_ELM(SCROLL_RIGHT))) {
		if (horizontal)
			return EINA_FALSE;
		else
			return EINA_TRUE;
	}

	return EINA_FALSE;
}

/**
 * @brief Function that registers Evas_Object gengrid from Elementary.
 */
static void __register_elm_gengrid(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *gengrid;
	VC_ELM_LOG_INFO("Registering elm_gengrid in vc_elm");
	gengrid = _vc_elm_core_register_widget("Elm_Gengrid", NULL, __gengrid_filter_func, NULL);

	if (NULL == gengrid)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(gengrid, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __gengrid_activator_func);
	}
}

/* Genlist widget ---------------------------------------------------------- */
/**
 * @brief Function that is activated on genlist action.
 * @param[in] genlist object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __genlist_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;
	VC_ELM_LOG_INFO("activated genlist with action %s", action_name);

	if (!action_name)
		VC_ELM_LOG_WARN("no default action set for genlist");
	else if (!strcmp(_VC_ELM(UP), action_name))
		__scroll_object(obj, LITTLE, VERTICAL, TO_BEGINNING);
	else if (!strcmp(_VC_ELM(DOWN), action_name))
		__scroll_object(obj, LITTLE, VERTICAL, TO_END);
	else if (!strcmp(_VC_ELM(SCROLL_UP), action_name))
		__scroll_object(obj, MORE, VERTICAL, TO_BEGINNING);
	else if (!strcmp(_VC_ELM(SCROLL_DOWN), action_name))
		__scroll_object(obj, MORE, VERTICAL, TO_END);
	else if (!strcmp(_VC_ELM(HOME), action_name))
		__scroll_object(obj, MAXIMAL, VERTICAL, TO_BEGINNING);
	else if (!strcmp(_VC_ELM(END), action_name))
		__scroll_object(obj, MAXIMAL, VERTICAL, TO_END);
	else
		VC_ELM_LOG_WARN("unknown action");
}

static Elm_Object_Item *__genlist_filter_first_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_genlist_first_item_get(obj);
}

static Elm_Object_Item *__genlist_filter_item_next_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_genlist_item_next_get(it);
}

static Elm_Object_Item *__genlist_filter_last_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_genlist_last_item_get(obj);
}

static Elm_Object_Item *__genlist_filter_item_prev_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_genlist_item_prev_get(it);
}

/**
 * @brief Function that filter subwidgets voice command of genlist object.
 * @param[in] genlist object
 * @param[in] action string that was activated
 */
static Eina_Bool __genlist_filter_func(Evas_Object *obj, const char *action, void *user_data)
{
	Elm_Object_Item *first;
	Elm_Object_Item *last;
	Elm_Object_Item *first_visible;
	Elm_Object_Item *last_visible;
	(void)action;
	(void)user_data;

	__set_tooltips_position_on_visible_items(obj, __genlist_filter_first_item_get, __genlist_filter_item_next_get, NULL);

	first = __genlist_filter_first_item_get(obj, NULL);
	last = __genlist_filter_last_item_get(obj, NULL);
	first_visible = _vc_elm_core_get_visible_item(obj, __genlist_filter_first_item_get, __genlist_filter_item_next_get, NULL);

	last_visible = _vc_elm_core_get_visible_item(obj, __genlist_filter_last_item_get, __genlist_filter_item_prev_get, NULL);

	VC_ELM_LOG_WARN("--%p %p--------%p %p--------", (void*)first, (void*)first_visible, (void*)last, (void*)last_visible);
#define MyDBG(name)								\
	{									\
		int x;								\
		int y;								\
		int w;								\
		int h;								\
		if (name) {							\
			evas_object_geometry_get(_vc_elm_core_get_evas_object(name), &x, &y, &w, &h);	\
			VC_ELM_LOG_WARN("%s -> %d %d / %d %d", #name, x, y, w, h);	\
		}								\
	}

	MyDBG(first);
	MyDBG(first_visible);
	MyDBG(last);
	MyDBG(last_visible);
#undef MyDBG

	if (first == first_visible && last == last_visible) {
		VC_ELM_LOG_DBG("hiding genlist tooltip");
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

/**
 * @brief Function that registers Evas_Object genlist from Elementary.
 */
static void __register_elm_genlist(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *genlist;
	VC_ELM_LOG_INFO("Registering elm_genlist in vc_elm");
	genlist = _vc_elm_core_register_widget("Elm_Genlist", NULL, __genlist_filter_func, NULL);

	if (NULL == genlist)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(genlist, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __genlist_activator_func);
	}
}

/* radiobutton widget------------------------------------------------------- */
/**
 * @brief Function that registers Evas_Object radiobutton from Elementary.
 */
static void __register_elm_radiobutton(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *radiobutton;
	VC_ELM_LOG_INFO("Registering elm_radiobutton in vc_elm");
	radiobutton = _vc_elm_core_register_widget("Elm_Radio", NULL, NULL, NULL);

	if (NULL == radiobutton)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(radiobutton, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __button_activator_func);
	}
}

/* bubble widget ----------------------------------------------------------- */
/**
 * @brief Function that is activated on bubble action.
 * @param[in] bubble object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __bubble_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;
	if (!action_name || (0 == strcmp(action_name, _VC_ELM(CLICK)))) {
		VC_ELM_LOG_INFO("Activated bubble with action %s", action_name);
		evas_object_smart_callback_call(obj, "clicked", NULL);
	}
}

/**
 * @brief Function that registers Evas_Object bubble from Elementary.
 */
static void __register_elm_bubble(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *bubble;
	VC_ELM_LOG_INFO("Registering bubble in VC_ELM");
	bubble = _vc_elm_core_register_widget("Elm_Bubble", NULL, NULL, NULL);
	if (NULL == bubble)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(bubble, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __bubble_activator_func);
	}
}

/* list widget ------------------------------------------------------------- */
/**
 * @brief Function that is activated on list action.
 * @param[in] list object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __list_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;
	VC_ELM_LOG_INFO("Activated list with action %s", action_name);

	if (!action_name)
		VC_ELM_LOG_WARN("No default action set for list");
	else if (!strcmp(_VC_ELM(UP), action_name))
		__scroll_object(obj, LITTLE, VERTICAL, TO_BEGINNING);
	else if (!strcmp(_VC_ELM(DOWN), action_name))
		__scroll_object(obj, LITTLE, VERTICAL, TO_END);
	else if (!strcmp(_VC_ELM(SCROLL_UP), action_name))
		__scroll_object(obj, MORE, VERTICAL, TO_BEGINNING);
	else if (!strcmp(_VC_ELM(SCROLL_DOWN), action_name))
		__scroll_object(obj, MORE, VERTICAL, TO_END);
	else if (!strcmp(_VC_ELM(HOME), action_name))
		__scroll_object(obj, MAXIMAL, VERTICAL, TO_BEGINNING);
	else if (!strcmp(_VC_ELM(END), action_name))
		__scroll_object(obj, MAXIMAL, VERTICAL, TO_END);
	else
		VC_ELM_LOG_WARN("unknown action");
}

/**
 * @brief Function found and set voice tooltips on list
 */
static void __set_tooltips_position_on_visible_items(Evas_Object *parent, Elm_Object_Item *(*get_first)(const Evas_Object *obj, void *user_data), Elm_Object_Item *(*get_next)(const Elm_Object_Item *item, void *user_data), void *user_data)
{
	Elm_Object_Item *item = get_first(parent, user_data);
	vc_elm_direction_e direction = VC_ELM_DIRECTION_CENTER;
	int x;
	int y;
	int w;
	int h;
	if (item == NULL)
		return;

	_vc_elm_core_get_sub_item_hint_direction(parent, &direction);

	evas_object_geometry_get(parent, &x, &y, &w, &h);
	do {
		int x_1;
		int y_1;
		int w_1;
		int h_1;
		Evas_Object *obj = _vc_elm_core_get_evas_object(item);
		if (!obj)
			continue;
		_vc_elm_core_set_sub_item_hint_direction(obj, direction);
		evas_object_geometry_get(obj, &x_1, &y_1, &w_1, &h_1);

		if (x_1 + (w_1 / 2) >= x && y_1 + (h_1 / 2) >= y && (x_1 + (w_1 / 2)) <= (x + w) && (y_1 + (h_1 / 2)) <= (y + h)) {
			const char *name = elm_widget_type_get(obj);
			if (name == NULL) {
				char *item_string = NULL;
				if (NULL != (item_string = evas_object_data_get(obj, VC_ELM_HINT_DATA_KEY)))
					_vc_elm_core_set_object_hint(obj, item_string);
				else
					continue;
			}
		}
	} while ((item = get_next(item, user_data)) != NULL);

	return;
}

Elm_Object_Item *__list_filter_first_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_list_first_item_get(obj);
}

Elm_Object_Item *list_filter_item_next_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_list_item_next(it);
}

Elm_Object_Item *list_filter_last_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_list_last_item_get(obj);
}

Elm_Object_Item *list_filter_item_prev_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_list_item_prev(it);
}

/**
 * @brief Function that filter subwidgets voice command of filter object.
 * @param[in] filter object
 * @param[in] action string that was activated
 */
static Eina_Bool __list_filter_func(Evas_Object *obj, const char *action, void *user_data)
{
	Elm_Object_Item *first;
	Elm_Object_Item *last;
	Elm_Object_Item *first_visible;
	Elm_Object_Item *last_visible;
	(void)action;
	(void)user_data;

	__set_tooltips_position_on_visible_items(obj, __list_filter_first_item_get, list_filter_item_next_get, NULL);
	first = __list_filter_first_item_get(obj, NULL);
	last = list_filter_last_item_get(obj, NULL);
	first_visible = _vc_elm_core_get_visible_item(obj, __list_filter_first_item_get, list_filter_item_next_get, NULL);
	last_visible = _vc_elm_core_get_visible_item(obj, list_filter_last_item_get, list_filter_item_prev_get, NULL);
	if (first == first_visible && last == last_visible) {
		VC_ELM_LOG_DBG("hiding list tooltip");
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

/**
 * @brief Function that registers Evas_Object list from Elementary.
 */
static void __register_elm_list(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *list;
	VC_ELM_LOG_DBG("Registering list in vc_elm");
	list = _vc_elm_core_register_widget("Elm_List", NULL, __list_filter_func, NULL);

	if (NULL == list)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(list, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __list_activator_func);
	}
}

/* slider ------------------------------------------------------------------ */
/**
 * @brief Function that is activated on slider action.
 * @param[in] slider object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __slider_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	/* separated horizontal and vertical slider */

	double new_value;
	double min;
	double max;
	double value = elm_slider_value_get(obj);
	Eina_Bool horizontal = elm_slider_horizontal_get(obj);
	(void)data;

	elm_slider_min_max_get(obj, &min, &max);

	VC_ELM_LOG_INFO("Activated slider with action %s", action_name);

	if (action_name == NULL) {
		VC_ELM_LOG_WARN("Action is null. Doing nothing.");
	} else if (horizontal == EINA_TRUE && !strcmp(action_name, _VC_ELM(LEFT))) {
		new_value = value - 0.1 * (max - min);
		if (new_value <= min)
			elm_slider_value_set(obj, min);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: left");
	} else if (horizontal == EINA_TRUE && !strcmp(action_name, _VC_ELM(RIGHT))) {
		new_value = value + 0.1 * (max - min);
		if (new_value >= max)
			elm_slider_value_set(obj, max);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: right");
	} else if (horizontal == EINA_TRUE && !strcmp(action_name, _VC_ELM(SCROLL_LEFT))) {
		new_value = value - 0.2 * (max - min);
		if (new_value <= min)
			elm_slider_value_set(obj, min);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: scroll left");
	} else if (horizontal == EINA_TRUE && !strcmp(action_name, _VC_ELM(SCROLL_RIGHT))) {
		new_value = value + 0.2 * (max - min);
		if (new_value >= max)
			elm_slider_value_set(obj, max);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: scroll right");
	} else if (horizontal == EINA_FALSE && !strcmp(action_name, _VC_ELM(UP))) {
		new_value = value - 0.1 * (max - min);
		if (new_value <= min)
			elm_slider_value_set(obj, min);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: up");
	} else if (horizontal == EINA_FALSE && !strcmp(action_name, _VC_ELM(DOWN))) {
		new_value = value + 0.1 * (max - min);
		if (new_value >= max)
			elm_slider_value_set(obj, max);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: down");
	} else if (horizontal == EINA_FALSE && !strcmp(action_name, _VC_ELM(SCROLL_UP))) {
		new_value = value - 0.2 * (max - min);
		if (new_value <= min)
			elm_slider_value_set(obj, min);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: scroll up");
	} else if (horizontal == EINA_FALSE && !strcmp(action_name, _VC_ELM(SCROLL_DOWN))) {
		new_value = value + 0.2 * (max - min);
		if (new_value >= max)
			elm_slider_value_set(obj, max);
		else
			elm_slider_value_set(obj, new_value);

		VC_ELM_LOG_INFO("Slider: scroll down");
	} else if (!strcmp(action_name, _VC_ELM(HOME))) {
		elm_slider_value_set(obj, min);

		VC_ELM_LOG_INFO("Slider: home");
	} else if (!strcmp(action_name, _VC_ELM(END))) {
		elm_slider_value_set(obj, max);

		VC_ELM_LOG_INFO("Slider: end");
	} else if (!strcmp(action_name, _VC_ELM(CENTER))) {
		elm_slider_value_set(obj, 0.5 * (max - min));

		VC_ELM_LOG_INFO("Slider: center");
	} else {
		VC_ELM_LOG_WARN("unknown action");
	}
}

/**
 * @brief Function that filter subwidgets voice command of slider object.
 * @param[in] slider object
 * @param[in] action string that was activated
 */
static Eina_Bool __slider_cmd_filter_func(Evas_Object *obj, const char *text, void *user_data)
{
	Eina_Bool horizontal = elm_slider_horizontal_get(obj);
	(void)user_data;

	if (text == NULL)
		return EINA_FALSE;

	if (!strcmp(text, _VC_ELM(LEFT)) || !strcmp(text, _VC_ELM(RIGHT)) || !strcmp(text, _VC_ELM(SCROLL_LEFT)) || !strcmp(text, _VC_ELM(SCROLL_RIGHT)) || !strcmp(text, _VC_ELM(HOME)) || !strcmp(text, _VC_ELM(END)) || !strcmp(text, _VC_ELM(CENTER))) {
		if (horizontal)
			return EINA_FALSE;
	}
	if (!strcmp(text, _VC_ELM(UP)) || !strcmp(text, _VC_ELM(DOWN)) || !strcmp(text, _VC_ELM(SCROLL_UP)) || !strcmp(text, _VC_ELM(SCROLL_DOWN)) || !strcmp(text, _VC_ELM(HOME)) || !strcmp(text, _VC_ELM(END)) || !strcmp(text, _VC_ELM(CENTER))) {
		if (!horizontal)
			return EINA_FALSE;
	}

	return EINA_TRUE;
}

/**
 * @brief Function that registers Evas_Object spinner from Elementary.
 */
static void __register_elm_slider(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *slider;
	VC_ELM_LOG_INFO("Registering slider in VC_ELM");
	slider = _vc_elm_core_register_widget("Elm_Slider", NULL, __slider_cmd_filter_func, NULL);

	if (NULL == slider)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(slider, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __slider_activator_func);
	}
}

/* colorselector widget -----------------------------------------------------*/

static const char VC_ELM_COLORSELECTOR_PARENT[] = "vc_elm_colorselector_parent";

static Evas_Object *swallowedH;
static Evas_Object *swallowedS;
static Evas_Object *swallowedV;

/**
 * @brief Function that registers voice command for colorselector object.
 * @param[in] colorselector object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __colorselector_hue_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)obj;
	(void)data;

	if (!action_name || !strcmp(action_name, _VC_ELM(LEFT))) {
		edje_object_part_drag_step(swallowedH, "elm.arrow", -0.01, 0);
		edje_object_signal_emit(swallowedH, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(RIGHT))) {
		edje_object_part_drag_step(swallowedH, "elm.arrow", 0.01, 0);
		edje_object_signal_emit(swallowedH, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(SCROLL_LEFT))) {
		edje_object_part_drag_step(swallowedH, "elm.arrow", -0.05, 0);
		edje_object_signal_emit(swallowedH, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(SCROLL_RIGHT))) {
		edje_object_part_drag_step(swallowedH, "elm.arrow", 0.05, 0);
		edje_object_signal_emit(swallowedH, "drag", "*");
	} else {
		VC_ELM_LOG_WARN("unknown action");
		return;
	}
}

/**
 * @brief Function that registers voice command for sat. colorselector object.
 * @param[in] colorselector object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __colorselector_saturation_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)obj;
	(void)data;

	if (!action_name || !strcmp(action_name, _VC_ELM(RIGHT))) {
		edje_object_part_drag_step(swallowedS, "elm.arrow", 0.01, 0);
		edje_object_signal_emit(swallowedS, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(LEFT))) {
		edje_object_part_drag_step(swallowedS, "elm.arrow", -0.01, 0);
		edje_object_signal_emit(swallowedS, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(SCROLL_RIGHT))) {
		edje_object_part_drag_step(swallowedS, "elm.arrow", 0.05, 0);
		edje_object_signal_emit(swallowedS, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(SCROLL_LEFT))) {
		edje_object_part_drag_step(swallowedS, "elm.arrow", -0.05, 0);
		edje_object_signal_emit(swallowedS, "drag", "*");
	} else {
		VC_ELM_LOG_WARN("unknown action");
		return;
	}
}

/**
 * @brief Function that registers voice command for value colorselector object.
 * @param[in] colorselector object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __colorselector_value_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)obj;
	(void)data;

	if (!action_name || !strcmp(action_name, _VC_ELM(LEFT))) {
		edje_object_part_drag_step(swallowedV, "elm.arrow", -0.01, 0);
		edje_object_signal_emit(swallowedV, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(RIGHT))) {
		edje_object_part_drag_step(swallowedV, "elm.arrow", 0.01, 0);
		edje_object_signal_emit(swallowedV, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(SCROLL_LEFT))) {
		edje_object_part_drag_step(swallowedV, "elm.arrow", -0.05, 0);
		edje_object_signal_emit(swallowedV, "drag", "*");
	} else if (!strcmp(action_name, _VC_ELM(SCROLL_RIGHT))) {
		edje_object_part_drag_step(swallowedV, "elm.arrow", 0.05, 0);
		edje_object_signal_emit(swallowedV, "drag", "*");
	} else {
		VC_ELM_LOG_WARN("unknown action");
		return;
	}
}

/**
 * @brief Function that analyse layout for color objects.
 */
static void __sub_layout_inspect(Eina_List *ret, Evas_Object *widget)
{
	/*Temporary fix for pallete without tooltips*/
	const char *colors[] = {"pink", "violet", "purple", "navy", "blue", "cyan", "green", "lime", "yellow", "red"};
	const Eina_List *l = NULL;
	Evas_Object *obj = NULL;
	const Eina_List *list = elm_widget_can_focus_child_list_get(widget);
	int i = 0;

	(void)ret;
	(void)widget;

	EINA_LIST_FOREACH(list, l, obj) {
		const char *type = elm_widget_type_get(obj);
		if (type && !strcmp(type, "Elm_Layout")) {
			evas_object_data_set(obj, VC_ELM_COLORSELECTOR_PARENT, widget);
			evas_object_data_set(obj, _vc_elm_get_custom_widget_name(), "Elm_Button");
			ret = eina_list_append(ret, obj);
			if (i < 10)
				_vc_elm_set_object_hint(obj, colors[i++]);
		}
	}
}

const char HUE[] = "IDS_HUE";
const char SATURATION[] = "IDS_SATURATION";
const char LIGHTNESS[] = "IDS_LIGHTNESS";
/**
 * @brief Function that registers voice command for colorselector object.
 * @param[in] colorselector object
 */
static Eina_List *__colorselector_get_subwidgets(const Evas_Object *colorselector, void *user_data)
{
	Eina_List *ret = NULL;
	Evas_Object *edje = elm_layout_content_get(colorselector, "selector");
	(void)user_data;

	swallowedH = edje_object_part_swallow_get(edje, "elm.colorbar_0");
	evas_object_data_set(swallowedH, _vc_elm_get_custom_widget_name(), "simple_hue_colorbar_activator");
	evas_object_data_set(swallowedH, VC_ELM_COLORSELECTOR_PARENT, colorselector);

	_vc_elm_core_set_object_command(swallowedH, _VC_ELM(HUE));
	_vc_elm_core_set_object_hint(swallowedH, _VC_ELM(HUE));
	ret = eina_list_append(ret, swallowedH);

	swallowedS = edje_object_part_swallow_get(edje, "elm.colorbar_1");
	evas_object_data_set(swallowedS, _vc_elm_get_custom_widget_name(), "simple_saturation_colorbar_activator");
	evas_object_data_set(swallowedS, VC_ELM_COLORSELECTOR_PARENT, colorselector);
	_vc_elm_core_set_object_command(swallowedS, _VC_ELM(SATURATION));
	_vc_elm_core_set_object_hint(swallowedS, _VC_ELM(SATURATION));
	ret = eina_list_append(ret, swallowedS);

	swallowedV = edje_object_part_swallow_get(edje, "elm.colorbar_2");
	evas_object_data_set(swallowedV, _vc_elm_get_custom_widget_name(), "simple_value_colorbar_activator");
	evas_object_data_set(swallowedV, VC_ELM_COLORSELECTOR_PARENT, colorselector);
	_vc_elm_core_set_object_command(swallowedV, _VC_ELM(LIGHTNESS));
	_vc_elm_core_set_object_hint(swallowedV, _VC_ELM(LIGHTNESS));
	ret = eina_list_append(ret, swallowedV);

	/*Palette*/
	edje = elm_layout_content_get(colorselector, "palette");
	__sub_layout_inspect(ret, edje);
	return ret;
}

/**
 * @brief Function that registers Evas_Object hue_colorselector from Elementary.
 */
static void __register_elm_simple_hue_colorselector_activator(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *colorbar;
	VC_ELM_LOG_INFO("Registering simple_hue_colorselector_activator in VC_ELM");
	colorbar = _vc_elm_core_register_widget("simple_hue_colorbar_activator", NULL, NULL, NULL);

	if (NULL == colorbar)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(colorbar, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __colorselector_hue_activator_func);
	}
}

/**
 * @brief Function that registers Evas_Object saturation colorselector
 * from Elementary.
 */
static void __register_elm_simple_saturation_colorselector_activator(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *colorbar;
	VC_ELM_LOG_INFO("Registering simple_saturation_colorselector_activator in VC_ELM");
	colorbar = _vc_elm_core_register_widget("simple_saturation_colorbar_activator", NULL, NULL, NULL);

	if (NULL == colorbar)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(colorbar, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __colorselector_saturation_activator_func);
	}
}

/**
 * @brief Function that registers Evas_Object value colorselector
 * from Elementary.
 */
static void __register_elm_simple_value_colorselector_activator(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *colorbar;
	VC_ELM_LOG_INFO("Registering simple_value_colorselector_activator in VC_ELM");
	colorbar = _vc_elm_core_register_widget("simple_value_colorbar_activator", NULL, NULL, NULL);

	if (NULL == colorbar)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(colorbar, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __colorselector_value_activator_func);
	}
}

/**
 * @brief Function that registers Evas_Object colorselector from Elementary.
 */
static void __register_elm_colorselector(Eina_List *actions)
{
	(void)actions;
	VC_ELM_LOG_INFO("Registering colorselector in VC_ELM");
	_vc_elm_core_register_widget("elm_colorselector", &__colorselector_get_subwidgets, NULL, NULL);
}

/* Spinner ----------------------------------------------------------------- */
/**
 * @brief Function that is activated on spinner action.
 * @param[in] spinner object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __spinner_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	Eina_Bool vertical = EINA_FALSE;
	double min;
	double max;
	double step;
	double value;
	const char *style = elm_object_style_get(obj);
	(void)data;

	if (style && (0 == strcmp(style, "vertical") || 0 == strcmp(style, "default")))
		vertical = EINA_TRUE;

	elm_spinner_min_max_get(obj, &min, &max);
	value = elm_spinner_value_get(obj);
	step = elm_spinner_step_get(obj);

	VC_ELM_LOG_INFO("activated spinner with action %s", action_name);
	if (!action_name) {
		VC_ELM_LOG_WARN("no default action set for spinner");
		return;
	} else if ((!vertical && !strcmp(action_name, _VC_ELM(LEFT))) || (vertical && !strcmp(action_name, _VC_ELM(DOWN)))) {
		value = value - step;
		/* we don't need to handle min/max and wrap cases,
		 * spinner will do it automatically */
		elm_spinner_value_set(obj, value);
	} else if ((!vertical && !strcmp(action_name, _VC_ELM(RIGHT))) || (vertical && !strcmp(action_name, _VC_ELM(UP)))) {
		value = value + step;
		elm_spinner_value_set(obj, value);
		/* we don't need to handle min/max and wrap cases,
		 * spinner will do it automatically */
	} else if (!strcmp(action_name, _VC_ELM(HOME))) {
		elm_spinner_value_set(obj, min);

		VC_ELM_LOG_INFO("Spinner: home");
	} else if (!strcmp(action_name, _VC_ELM(END))) {
		elm_spinner_value_set(obj, max);

		VC_ELM_LOG_INFO("Spinner: end");
	} else {
		VC_ELM_LOG_WARN("unknown action");
		return;
	}
}

/**
 * @brief Function that filter subwidgets voice command of spinner object.
 * @param[in] spinner object
 * @param[in] action string that was activated
 */
static Eina_Bool __spinner_cmd_filter_func(Evas_Object *obj, const char *action_name, void *user_data)
{
	Eina_Bool vertical = EINA_FALSE;
	const char *style = elm_object_style_get(obj);
	(void)user_data;

	if (style && (0 == strcmp(style, "vertical") || 0 == strcmp(style, "default")))
		vertical = EINA_TRUE;

	if ((!action_name) || (!strcmp(action_name, _VC_ELM(HOME))) || (!strcmp(action_name, _VC_ELM(END))))
		return EINA_FALSE;
	else if ((!vertical && !strcmp(action_name, _VC_ELM(LEFT))) || (vertical && !strcmp(action_name, _VC_ELM(DOWN))))
		return EINA_FALSE;
	else if ((!vertical && !strcmp(action_name, _VC_ELM(RIGHT))) || (vertical && !strcmp(action_name, _VC_ELM(UP))))
		return EINA_FALSE;
	return EINA_TRUE;
}

/**
 * @brief Function that registers Evas_Object spinner from Elementary.
 */
static void __register_elm_spinner(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *spinner;
	VC_ELM_LOG_INFO("Registering photocam in VC_ELM");
	spinner = _vc_elm_core_register_widget("Elm_Spinner", NULL, __spinner_cmd_filter_func, NULL);

	if (NULL == spinner)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(spinner, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __spinner_activator_func);
	}
}

/* entry widget ------------------------------------------------------------ */
/**
 * @brief Function that filter subwidgets voice command of entry object.
 * @param[in] entry object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __entry_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;

	if (!action_name || 0 == strcmp(_VC_ELM(CLICK), action_name)) {
		__emulate_mouse_click(obj);
		elm_entry_select_all(obj);
		elm_entry_select_none(obj);
		elm_entry_input_panel_show(obj);
	} else if (elm_entry_scrollable_get(obj)) {
		if (!strcmp(_VC_ELM(UP), action_name))
			__scroll_object(obj, LESS, VERTICAL, TO_BEGINNING);
		if (!strcmp(_VC_ELM(DOWN), action_name))
			__scroll_object(obj, LESS, VERTICAL, TO_END);
		if (!strcmp(_VC_ELM(SCROLL_UP), action_name))
			__scroll_object(obj, LITTLE, VERTICAL, TO_BEGINNING);
		if (!strcmp(_VC_ELM(SCROLL_DOWN), action_name))
			__scroll_object(obj, LITTLE, VERTICAL, TO_END);
	} else {
		VC_ELM_LOG_ERR("not supported");
	}
}

static Eina_Bool __entry_cmd_filter_func(Evas_Object *obj, const char *action_name, void *user_data)
{
	Eina_Bool scrollable = EINA_FALSE;
	Eina_Bool single_line = EINA_FALSE;
	const char *text;

	(void)user_data;

	scrollable = elm_entry_scrollable_get(obj);
	text = elm_entry_entry_get(obj);
	single_line = elm_entry_single_line_get(obj);

	if (text && scrollable == EINA_TRUE && single_line == EINA_FALSE) {
		if (!strcmp(action_name, _VC_ELM(UP)) || !strcmp(action_name, _VC_ELM(DOWN)) || !strcmp(action_name, _VC_ELM(SCROLL_UP)) || !strcmp(action_name, _VC_ELM(SCROLL_UP)))
			return EINA_FALSE;
	}

	if (!strcmp(action_name, _VC_ELM(CLICK)))
		return EINA_FALSE;

	return EINA_TRUE;
}

/**
 * @brief Function that registers Evas_Object entry from Elementary.
 */
static void __register_elm_entry(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *entry;
	VC_ELM_LOG_INFO("Registering entry in VC_ELM");

	entry = _vc_elm_core_register_widget("Elm_Entry", NULL, __entry_cmd_filter_func, NULL);

	if (NULL == entry)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(entry, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __entry_activator_func);
	}
}

/* multibuttonentry widget ------------------------------------------------- */
/**
 * @brief Function that registers subwidgets voice command of multibuttonentry object.
 * @param[in] multibuttonentry object
 */
static Eina_List *__entry_get_subwidgets(const Evas_Object *entry, void *user_data)
{
	Eina_List *ret = NULL;
	const Eina_List *l = NULL;
	Evas_Object *obj = NULL;
	const Eina_List *list = NULL;
	(void)user_data;
	entry = elm_layout_content_get(entry, "box.swallow");
	list = elm_widget_can_focus_child_list_get(entry);

	EINA_LIST_FOREACH(list, l, obj) {
		const char *type = elm_widget_type_get(obj);
		const char *part = elm_object_part_text_get(obj, "mbe.label");
		if (part)
			continue;
		if (type && !strcmp(type, "Elm_Layout")) {
			evas_object_data_set(obj, VC_ELM_COLORSELECTOR_PARENT, entry);
			evas_object_data_set(obj, _vc_elm_get_custom_widget_name(), "Elm_Button");
			ret = eina_list_append(ret, obj);
		} else if (type && !strcmp(type, "Elm_Entry")) {
			ret = eina_list_append(ret, obj);
		}
	}
	return ret;
}

/**
 * @brief Function that registers Evas_Object multibuttonentry from Elementary.
 */
static void __register_elm_multibuttonentry(Eina_List *actions)
{
	(void)actions;
	VC_ELM_LOG_INFO("Registering entry in VC_ELM");

	_vc_elm_core_register_widget("Elm_Multibuttonentry", __entry_get_subwidgets, NULL, NULL);
}

/* toolbar widget ----------------------------------------------------------*/

static Elm_Object_Item *__toolbar_filter_first_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_toolbar_first_item_get(obj);
}

static Elm_Object_Item *__toolbar_filter_item_next_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_toolbar_item_next_get(it);
}

static Elm_Object_Item *__toolbar_filter_last_item_get(const Evas_Object *obj, void *user_data)
{
	(void)user_data;
	return elm_toolbar_last_item_get(obj);
}

static Elm_Object_Item *__toolbar_filter_item_prev_get(const Elm_Object_Item *it, void *user_data)
{
	(void)user_data;
	return elm_toolbar_item_prev_get(it);
}

/**
 * @brief Function that is activated on toolbar action.
 * @param[in] toolbar object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void __toolbar_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	Elm_Object_Item *item_first;
	Elm_Object_Item *item_last;
	Elm_Object_Item *first_visible_item;
	Elm_Object_Item *last_visible_item;
	(void)data;

	item_first = __toolbar_filter_first_item_get(obj, NULL);
	item_last = __toolbar_filter_last_item_get(obj, NULL);

	first_visible_item = _vc_elm_core_get_visible_item(obj, __toolbar_filter_first_item_get, __toolbar_filter_item_next_get, NULL);
	last_visible_item = _vc_elm_core_get_visible_item(obj, __toolbar_filter_last_item_get, __toolbar_filter_item_prev_get, NULL);

	if (!action_name) {
		VC_ELM_LOG_WARN("No default action set for toolbar");
		return;
	} else if (!strcmp(_VC_ELM(HOME), action_name)) {
		elm_toolbar_item_bring_in(item_first, ELM_TOOLBAR_ITEM_SCROLLTO_IN);
	} else if (!strcmp(_VC_ELM(END), action_name)) {
		elm_toolbar_item_bring_in(item_last, ELM_TOOLBAR_ITEM_SCROLLTO_IN);
	} else if (elm_toolbar_horizontal_get(obj) == EINA_TRUE) {
		if (!strcmp(_VC_ELM(RIGHT), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_next_get(last_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_IN);
		else if (!strcmp(_VC_ELM(SCROLL_RIGHT), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_next_get(last_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_FIRST);
		else if (!strcmp(_VC_ELM(LEFT), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_prev_get(first_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_IN);
		else if (!strcmp(_VC_ELM(SCROLL_LEFT), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_prev_get(first_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_LAST);
	} else if (elm_toolbar_horizontal_get(obj) == EINA_FALSE) {
		if (!strcmp(_VC_ELM(DOWN), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_next_get(last_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_IN);
		else if (!strcmp(_VC_ELM(SCROLL_DOWN), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_next_get(last_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_MIDDLE);
		else if (!strcmp(_VC_ELM(UP), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_prev_get(first_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_IN);
		else if (!strcmp(_VC_ELM(SCROLL_UP), action_name))
			elm_toolbar_item_bring_in(elm_toolbar_item_prev_get(first_visible_item), ELM_TOOLBAR_ITEM_SCROLLTO_MIDDLE);
	} else {
		VC_ELM_LOG_ERR("not supported");
	}
}

/**
 * @brief Function that filter subwidgets voice command of toolbar object.
 * @param[in] toolbar object
 * @param[in] action string that was activated
 */
static Eina_Bool __toolbar_cmd_filter_func(Evas_Object *obj, const char *action_name, void *user_data)
{
	Eina_Bool horizontal = elm_toolbar_horizontal_get(obj);
	(void)user_data;

	{
		Elm_Object_Item *first = __toolbar_filter_first_item_get(obj, NULL);
		Elm_Object_Item *last = __toolbar_filter_last_item_get(obj, NULL);
		Elm_Object_Item *first_visible = _vc_elm_core_get_visible_item(obj, __toolbar_filter_first_item_get, __toolbar_filter_item_next_get, NULL);
		Elm_Object_Item *last_visible = _vc_elm_core_get_visible_item(obj, __toolbar_filter_last_item_get, __toolbar_filter_item_prev_get, NULL);
		if (first == first_visible && last == last_visible) {
			VC_ELM_LOG_DBG("filtered out toolbar");
			return EINA_TRUE;
		}
	}

	if (!action_name)
		return EINA_TRUE;
	else if (!strcmp(_VC_ELM(HOME), action_name) || !strcmp(_VC_ELM(END), action_name))
		return EINA_FALSE;
	else if ((horizontal) && (!strcmp(_VC_ELM(RIGHT), action_name) || !strcmp(_VC_ELM(LEFT), action_name) || !strcmp(_VC_ELM(SCROLL_RIGHT), action_name) || !strcmp(_VC_ELM(SCROLL_LEFT), action_name)))
		return EINA_FALSE;
	else if ((!horizontal) && (!strcmp(_VC_ELM(UP), action_name) || !strcmp(_VC_ELM(DOWN), action_name) || !strcmp(_VC_ELM(SCROLL_UP), action_name) || !strcmp(_VC_ELM(SCROLL_DOWN), action_name)))
		return EINA_FALSE;
	VC_ELM_LOG_DBG("filtered out %s action", action_name);
	return EINA_TRUE;
}

/**
 * @brief Function that registers Evas_Object toolbar from Elementary.
 */
static void __register_elm_toolbar(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *toolbar;
	VC_ELM_LOG_INFO("Registering toolbar in VC_ELM");
	toolbar = _vc_elm_core_register_widget("Elm_Toolbar", NULL, __toolbar_cmd_filter_func, NULL);

	if (NULL == toolbar)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(toolbar, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __toolbar_activator_func);
	}
}

/* checkbox widget ----------------------------------------------------------*/
/**
 * @brief Function that registers Evas_Object checkbox from Elementary.
 */
static void __register_elm_checkbox(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *checkbox;
	VC_ELM_LOG_INFO("Registering elm_checkbox in VC_ELM");
	checkbox = _vc_elm_core_register_widget("Elm_Check", NULL, NULL, NULL);

	if (NULL == checkbox)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(checkbox, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, __button_activator_func);
	}
}

/* panes widget -------------------------------------------------------------- */
/**
 * @brief Function that filter subwidgets voice command of panes object.
 * @param[in] panes object
 * @param[in] action string that was activated
 */
static Eina_Bool panes_cmd_filter_func(Evas_Object *obj, const char *action_name, void *user_data)
{
	Eina_Bool horizontal = elm_panes_horizontal_get(obj);
	double percentage = elm_panes_content_left_size_get(obj);
	int x;
	int y;
	int w;
	int h;
	char *image_path = evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_HINT_IMAGE_PATH));
	(void)user_data;

	if (!action_name) {
		VC_ELM_LOG_WARN("No default function for panes");
		return EINA_TRUE;
	}
	evas_object_geometry_get(obj, &x, &y, &w, &h);

	if (EINA_TRUE == horizontal)
		_vc_elm_set_object_custom_hint(obj, image_path, (int)((1.0 * w * 0.5)), (int)((1.0 * h * percentage)));
	else
		_vc_elm_set_object_custom_hint(obj, image_path, (int)((1.0 * w * percentage)), (int)((1.0 * h * 0.5)));

	if (elm_panes_fixed_get(obj))
		return EINA_FALSE;
	if ((horizontal) && (!strcmp(_VC_ELM(UP), action_name) || !strcmp(_VC_ELM(DOWN), action_name)))
		return EINA_FALSE;
	if ((!horizontal) && (!strcmp(_VC_ELM(LEFT), action_name) || !strcmp(_VC_ELM(RIGHT), action_name)))
		return EINA_FALSE;
	VC_ELM_LOG_INFO("Panes actions filtered");
	return EINA_TRUE;
}

/**
 * @brief Function that is activated on panes action.
 * @param[in] panes object
 * @param[in] user data
 * @param[in] action string that was activated
 */
static void panes_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	double left_size;
	double right_size;
	(void)data;

	left_size = elm_panes_content_left_size_get(obj);
	right_size = elm_panes_content_right_size_get(obj);

	if (!strcmp(_VC_ELM(UP), action_name))
		elm_panes_content_left_size_set(obj, left_size - 0.1);
	else if (!strcmp(_VC_ELM(DOWN), action_name))
		elm_panes_content_right_size_set(obj, right_size - 0.1);
	else if (!strcmp(_VC_ELM(LEFT), action_name))
		elm_panes_content_left_size_set(obj, left_size - 0.1);
	else if (!strcmp(_VC_ELM(RIGHT), action_name))
		elm_panes_content_right_size_set(obj, right_size - 0.1);
}

/**
 * @brief Function that registers Evas_Object panes from Elementary.
 */
static void __register_elm_panes(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *panes;
	VC_ELM_LOG_INFO("Registering panes in VC_ELM");
	panes = _vc_elm_core_register_widget("Elm_Panes", NULL, panes_cmd_filter_func, NULL);

	if (NULL == panes)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(panes, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, panes_activator_func);
	}
}

/* Scroller widget ---------------------------------------------------------*/

static void scroller_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	int h_pagelast;
	int v_pagelast;
	int h_currentpage;
	int v_currentpage;
	int max_w;
	int max_h;
	Evas_Coord pagewidth;
	Evas_Coord pageheight;
	Evas_Coord current_x;
	Evas_Coord current_y;
	Evas_Coord current_w;
	Evas_Coord current_h;
	double h_pagerel;
	double v_pagerel;
	(void)data;

	elm_scroller_last_page_get(obj, &h_pagelast, &v_pagelast);
	elm_scroller_page_size_get(obj, &pagewidth, &pageheight);
	elm_scroller_region_get(obj, &current_x, &current_y, &current_w, &current_h);
	elm_scroller_page_relative_get(obj, &h_pagerel, &v_pagerel);
	elm_scroller_current_page_get(obj, &h_currentpage, &v_currentpage);
	elm_scroller_child_size_get(obj, &max_w, &max_h);

	if (!strcmp(_VC_ELM(HOME), action_name)) {
		elm_scroller_region_show(obj, 0, 0, current_w, current_h);
	} else if (!strcmp(_VC_ELM(END), action_name)) {
		elm_scroller_region_show(obj, max_w, max_h, current_w, current_h);
	} else if (!strcmp(_VC_ELM(UP), action_name)) {
		if (v_pagerel > 0)
			elm_scroller_page_show(obj, h_currentpage, v_currentpage - 1);
		else
			elm_scroller_region_show(obj, current_x, current_y - current_h, current_w, current_h);
	} else if (!strcmp(_VC_ELM(DOWN), action_name)) {
		if (v_pagerel > 0)
			elm_scroller_page_show(obj, h_currentpage, v_currentpage + 1);
		else
			elm_scroller_region_show(obj, current_x, current_y + current_h, current_w, current_h);
	} else if (!strcmp(_VC_ELM(LEFT), action_name)) {
		if (h_pagerel > 0)
			elm_scroller_page_show(obj, h_currentpage - 1, v_currentpage);
		else
			elm_scroller_region_show(obj, current_x - current_w, current_y, current_w, current_h);
	} else if (!strcmp(_VC_ELM(RIGHT), action_name)) {
		if (h_pagerel > 0)
			elm_scroller_page_show(obj, h_currentpage + 1, v_currentpage);
		else
			elm_scroller_region_show(obj, current_x + current_w, current_y, current_w, current_h);
	}
}

static Eina_Bool scroller_filter_func(Evas_Object *obj, const char *text, void *user_data)
{
	Eina_Bool lock_x;
	Eina_Bool lock_y;

	(void)user_data;

	lock_x = elm_object_scroll_lock_x_get(obj);
	lock_y = elm_object_scroll_lock_y_get(obj);

	if (lock_x) {
		if (!strcmp(_VC_ELM(RIGHT), text) || !strcmp(_VC_ELM(LEFT), text))
			return EINA_TRUE;
	}

	if (lock_y) {
		if (!strcmp(_VC_ELM(UP), text) || !strcmp(_VC_ELM(DOWN), text))
			return EINA_TRUE;
	}

	return EINA_FALSE;
}

static void __register_elm_scroller(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *scroller;
	VC_ELM_LOG_INFO("Registering scroller in VC_ELM");
	scroller = _vc_elm_core_register_widget("Elm_Scroller", NULL, scroller_filter_func, NULL);

	if (NULL == scroller)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(scroller, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, scroller_activator_func);
	}
}

static void label_activator_func(Evas_Object *obj, void *data, const char *action_name)
{
	(void)data;
	(void)obj;
	(void)action_name;

}

static void __register_elm_label(Eina_List *actions)
{
	Eina_List *l;
	const char *action_name;
	struct vc_elm_core_widget_info *label;
	VC_ELM_LOG_INFO("Registering label in VC_ELM");
	label = _vc_elm_core_register_widget("Elm_Label", NULL, NULL, NULL);

	if (NULL == label)
		return;

	EINA_LIST_FOREACH(actions, l, action_name) {
		_vc_elm_core_register_action(label, eina_hash_find(_vc_elm_core_get_config_action_map(), action_name), NULL, label_activator_func);
	}
}

/* ------------------------------------------------------------------------- */
/**
 * @brief Function that registers supported widgets.
 */
void _vc_elm_core_register_default_widgets()
{
	__register_elm_genlist(NULL);
	__register_elm_radiobutton(NULL);
	__register_elm_bubble(NULL);
	__register_elm_checkbox(NULL);
	__register_elm_list(NULL);
	__register_elm_slider(NULL);
	__register_elm_colorselector(NULL);
	__register_elm_simple_hue_colorselector_activator(NULL);
	__register_elm_simple_saturation_colorselector_activator(NULL);
	__register_elm_simple_value_colorselector_activator(NULL);
	__register_elm_spinner(NULL);
	__register_elm_toolbar(NULL);
	__register_elm_entry(NULL);
	__register_elm_multibuttonentry(NULL);
	__register_vc_elm_item();
	__register_elm_gengrid(NULL);
	__register_elm_button(NULL);
	__register_elm_panes(NULL);
	__register_elm_scroller(NULL);
	__register_elm_label(NULL);
}

Eina_Bool _vc_elm_core_register_default_widget(const char *widget_name, Eina_Bool register_widget, Eina_List *actions)
{
	VC_ELM_LOG_DBG("REGISTERING A WIDGET %s", widget_name);
	VC_ELM_LOG_DBG("Actions list %u", eina_list_count(actions));
	VC_ELM_LOG_DBG("Actions list pointer %p", (void*)actions);
	(void)actions;
	if (NULL == widget_name)
		return EINA_FALSE;

	if (0 == strcmp(widget_name, "Elm_Genlist")) {
		if (register_widget)
			__register_elm_genlist(actions);
	} else if (0 == strcmp(widget_name, "Elm_Radio")) {
		if (register_widget)
			__register_elm_radiobutton(actions);
	} else if (0 == strcmp(widget_name, "Elm_Bubble")) {
		if (register_widget)
			__register_elm_bubble(actions);
	} else if (0 == strcmp(widget_name, "Elm_Check")) {
		if (register_widget)
			__register_elm_checkbox(actions);
	} else if (0 == strcmp(widget_name, "Elm_List")) {
		if (register_widget)
			__register_elm_list(actions);
	} else if (0 == strcmp(widget_name, "Elm_Slider")) {
		if (register_widget)
			__register_elm_slider(actions);
	} else if (0 == strcmp(widget_name, "Elm_Colorselector")) {
		if (register_widget)
			__register_elm_colorselector(actions);
	} else if (0 == strcmp(widget_name, "simple_hue_colorbar_activator")) {
		if (register_widget)
			__register_elm_simple_hue_colorselector_activator(actions);
	} else if (0 == strcmp(widget_name, "simple_saturation_colorbar_activator")) {
		if (register_widget)
			__register_elm_simple_saturation_colorselector_activator(actions);
	} else if (0 == strcmp(widget_name, "simple_value_colorbar_activator")) {
		if (register_widget)
			__register_elm_simple_value_colorselector_activator(actions);
	} else if (0 == strcmp(widget_name, "Elm_Spinner")) {
		if (register_widget)
			__register_elm_spinner(actions);
	} else if (0 == strcmp(widget_name, "Elm_Panes")) {
		if (register_widget)
			__register_elm_panes(actions);
	} else if (0 == strcmp(widget_name, "Elm_Toolbar")) {
		if (register_widget)
			__register_elm_toolbar(actions);
	} else if (0 == strcmp(widget_name, "Elm_Entry")) {
		if (register_widget)
			__register_elm_entry(actions);
	} else if (0 == strcmp(widget_name, "Elm_Multibuttonentry")) {
		if (register_widget)
			__register_elm_multibuttonentry(actions);
	} else if (0 == strcmp(widget_name, "elm_item")) {
		if (register_widget)
			__register_vc_elm_item();
	} else if (0 == strcmp(widget_name, "Elm_Gengrid")) {
		if (register_widget)
			__register_elm_gengrid(actions);
	} else if (0 == strcmp(widget_name, "Elm_Button")) {
		if (register_widget)
			__register_elm_button(actions);
	} else if (0 == strcmp(widget_name, "Elm_Scroller")) {
		if (register_widget)
			__register_elm_scroller(actions);
	} else if (0 == strcmp(widget_name, "Elm_Hoversel")) {
		if (register_widget)
			__register_elm_hoversel(actions);
	} else if (0 == strcmp(widget_name, "Elm_Label")) {
		if (register_widget)
			__register_elm_label(actions);
	} else {
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
