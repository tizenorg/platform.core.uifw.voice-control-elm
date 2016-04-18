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
#include "vc_elm.h"
#include "vc_elm_core.h"
#include "vc_elm_tooltip.h"
#include "vc_elm_tools.h"
#include "vc_elm_rectangle.h"
#include "Eina.h"
#include <voice_control_elm.h>
#include <stdlib.h>
#include <math.h>

#define _APP(S) dgettext(g_app_domain, S)

#define SCALE 1000

static const char __VC_ELM_SAVED_X[] = "vc_elm_saved_x";
static const char __VC_ELM_SAVED_Y[] = "vc_elm_saved_y";

static const char *__key_table[] = {
	"vc_elm_fixed_x",
	"vc_elm_fixed_y",
	"vc_elm_once_fixed_x",
	"vc_elm_once_fixed_y",
	"vc_elm_hint_image_path",
	"vc_elm_position_x",
	"vc_elm_position_y"
};

static char *g_app_domain = NULL;
static int domain_initialized = 0;

static Evas_Object *g_win = NULL;
static Evas_Object *g_grid = NULL;
static int win_width = -1;
static int win_heigh = -1;

static Eina_List *obj_list = NULL;

const char *_vc_elm_get_data_key(vc_elm_data_key key)
{
	if (key < sizeof(__key_table)/sizeof(__key_table[0])) {
		return __key_table[key];
	}
	return NULL;
}

void _vc_elm_set_tooltips_window(Evas_Object *win)
{
	g_win = win;
	evas_object_geometry_get(win, NULL, NULL, &win_width, &win_heigh);
	VC_ELM_LOG_ERR("init %p %p", (void*)win, (void*)g_grid);
}

static void __turn_on_tooltips()
{
	int x;
	int y;
	int w;
	int h;

	VC_ELM_LOG_DBG("my tool");

	g_grid = elm_grid_add(g_win);
	evas_object_layer_set(g_grid, 10000);

	elm_grid_size_set(g_grid, SCALE, SCALE);
	evas_object_geometry_get(g_win, &x, &y, &w, &h);
	VC_ELM_LOG_DBG("win size: %d %d, %d %d", x, y, w, h);
	evas_object_move(g_grid, x, y);
	evas_object_resize(g_grid, w, h);
	evas_object_show(g_grid);
	evas_object_smart_calculate(g_grid);
}

static int __evas_object_position_comparator_func(const void *a_, const void *b_)
{
	int ax;
	int ay;
	int aw;
	int ah;
	intptr_t tax;
	intptr_t tay;
	int bx;
	int by;
	int bw;
	int bh;
	intptr_t tbx;
	intptr_t tby;
	int dx;
	int dy;
	const Evas_Object *a = (const Evas_Object *)a_;
	const Evas_Object *b = (const Evas_Object *)b_;
	evas_object_geometry_get(a, &ax, &ay, &aw, &ah);
	tax = (intptr_t)evas_object_data_get(a, __VC_ELM_SAVED_X);
	tay = (intptr_t)evas_object_data_get(a, __VC_ELM_SAVED_Y);
	tbx = (intptr_t)evas_object_data_get(b, __VC_ELM_SAVED_X);
	tby = (intptr_t)evas_object_data_get(b, __VC_ELM_SAVED_Y);
	if (tax > 0 && tay > 0) {
		ax = (int)tax;
		ay = (int)tay;
	}
	evas_object_geometry_get(b, &bx, &by, &bw, &bh);

	if (tbx > 0 && tby > 0) {
		bx = (int)tbx;
		by = (int)tby;
	}
	dx = ax - bx;
	dy = ay - by;

	return dy == 0 ? dx : dy;
}

void _vc_elm_add_tooltip(Evas_Object *obj, const char *tip)
{
	int x = 0;
	int y = 0;
	Evas_Object *tipobj;
	int aw;
	int ah;
	int mw;
	int mh;
	int mx;
	int my; /* geometry of tooltips overlay */
	Evas_Object *rec;
	Evas_Object *rec_bg;
	const char *image_path;
	const char * edj_path;

	{
		int x_;
		int y_;
		int w_;
		int h_;
		intptr_t px_;
		intptr_t py_;

		evas_object_geometry_get(obj, &x_, &y_, &w_, &h_);
		x = x_ + w_ / 2;
		y = y_ + h_ / 2;

		px_ = (intptr_t)evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_FIXED_X));
		py_ = (intptr_t)evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_FIXED_Y));

		if (x_ > 0 && y_ > 0) {
			x = (int)px_;
			y = (int)py_;
		}

		px_ = (intptr_t)evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_ONCE_FIXED_X));
		if (px_ > 0)
			evas_object_data_del(obj, _vc_elm_get_data_key(VC_ELM_ONCE_FIXED_X));
		py_ = (intptr_t)evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_ONCE_FIXED_Y));
		if (py_ > 0)
			evas_object_data_del(obj, _vc_elm_get_data_key(VC_ELM_ONCE_FIXED_Y));

		if (px_ > 0 && py_ > 0) {
			x = (int)px_;
			y = (int)py_;
		}

		px_ = (intptr_t)x;
		py_ = (intptr_t)y;
		
		evas_object_data_set(obj, __VC_ELM_SAVED_X, (void *)px_);
		evas_object_data_set(obj, __VC_ELM_SAVED_Y, (void *)py_);
	}

	if (!g_grid)
		__turn_on_tooltips();

	rec = elm_label_add(g_grid);
	elm_object_text_set(rec, tip);
	evas_object_color_set(rec, 0, 0, 0, 255);
	evas_object_show(rec);

	evas_object_geometry_get(g_grid, &mx, &my, &mw, &mh);
	evas_object_size_hint_min_get(rec, &aw, &ah);

	rec_bg = elm_image_add(g_grid);

	image_path = evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_HINT_IMAGE_PATH));
	if (image_path == NULL)
		image_path = _vc_elm_core_get_tooltip_image_path();

	elm_image_file_set(rec_bg, image_path, NULL);
	elm_image_resizable_set(rec_bg, EINA_TRUE, EINA_TRUE);
	elm_image_aspect_fixed_set(rec_bg, EINA_FALSE);
	evas_object_size_hint_align_set(rec_bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(rec_bg, (aw + 60), ah);

	evas_object_show(rec_bg);

	tipobj = elm_layout_add(g_grid);
	edj_path = tzplatform_mkpath(TZ_SYS_RO_SHARE, "edje/voice-control-elm.edj");
	elm_layout_file_set(tipobj, edj_path, "tip_group");
//	elm_layout_file_set(tipobj, "/usr/share/edje/voice-control-elm.edj", "tip_group");
	elm_object_part_content_set(tipobj, "content", rec);
	elm_object_part_content_set(tipobj, "bg", rec_bg);
	evas_object_show(tipobj);

	{
		int _x = ((x - mx) * SCALE) / mw;
		int _y = ((y - my) * SCALE) / mh;

		int _w = ((aw + 32) * SCALE) / mw;
		int _h = ((ah + 10) * SCALE) / mh;
		elm_grid_pack(g_grid, tipobj, _x + 32, _y - _h / 2, _w, _h);
	}

	evas_object_data_set(obj, "MyTooltipHandle", tipobj);
	obj_list = eina_list_sorted_insert(obj_list, &__evas_object_position_comparator_func, obj);
}

void _vc_elm_turn_off_tooltips()
{
	Eina_List *chlds = elm_grid_children_get(g_grid);
	Eina_List *l;
	Evas_Object *obj;

	if (!g_grid)
		return;

	EINA_LIST_FOREACH(chlds, l, obj) {
		elm_grid_unpack(g_grid, obj);
		evas_object_del(obj);
	}

	evas_object_del(g_grid);
	g_grid = NULL;

	obj_list = eina_list_free(obj_list);
}

void _vc_elm_relayout_and_show_tooltips()
{
	Eina_List *l;
	Evas_Object *obj;
	int inserted = 0;
	int mw;
	int mh;
	int mx;
	int my;
	size_t length = eina_list_count(obj_list);
	R *recs = (R *)malloc(sizeof(R) * length);
	int nx;
	int ny;
	int nw;
	int nh;

	evas_object_geometry_get(g_grid, &mx, &my, &mw, &mh);

	EINA_LIST_FOREACH(obj_list, l, obj) {
		int p;
		int direction;
		R o;
		R t;
		R i;
		Evas_Object *tip = evas_object_data_get(obj, "MyTooltipHandle");
		_R_set_from_efl(&o, obj);

		{
			int x;
			int y;
			int w;
			int h;
			elm_grid_pack_get(tip, &x, &y, &w, &h);
			t.x = ((x * mw) / SCALE) - mx;
			t.y = ((y * mh) / SCALE) - my;
			t.w = ((w * mw) / SCALE);
			t.h = ((h * mh) / SCALE);
		}

		t.x = o.x - t.w + o.w / 2;
		t.x = t.x < 0 ? 0 : t.x;
		t.y = o.y - t.h + o.h / 2;
		t.y = t.y < 0 ? 0 : t.y;

		for (p = 0; p < inserted; ++p) {
			if (1 == _R_intersection(&t, &recs[p], &i)) {
				t.y += i.h + 1;
				if (1 == _R_intersection(&t, &recs[p], &i)) {
					t.y -= i.h + 1;
					t.x += i.w + 1;
					if (1 == _R_intersection(&t, &recs[p], &i))
						t.y += i.h + 1;
				}

			}
		}

		if (evas_object_data_get(obj, VC_ELM_DIRECTION) != NULL) {
			intptr_t pdirection = (intptr_t)evas_object_data_get(obj, VC_ELM_DIRECTION);
			direction = (int)pdirection;
			direction--;
		} else if (evas_object_data_get(obj, VC_ELM_SUB_ITEM_DIRECTION) != NULL) {
			intptr_t pdirection = (intptr_t)evas_object_data_get(obj, VC_ELM_SUB_ITEM_DIRECTION);
			direction = (int)pdirection;
			direction--;
		} else {
			direction = _vc_elm_core_get_tooltip_default_direction();
		}
		if (direction == VC_ELM_DIRECTION_CENTER) {
			t.x = o.x + o.w / 2 - t.w / 2;
			t.y = o.y + o.h / 2 - t.h / 2;
		} else if (direction == VC_ELM_DIRECTION_LEFT) {
			t.x = o.x + o.w / 5 - t.w;
			t.y = o.y + o.h / 2 - t.h / 2;
		} else if (direction == VC_ELM_DIRECTION_RIGHT) {
			t.x = o.x + o.w - o.w / 5;
			t.y = o.y + o.h / 2 - t.h / 2;
		} else if (direction == VC_ELM_DIRECTION_TOP) {
			t.x = o.x + o.w / 2 - t.w / 2;
			t.y = o.y + o.h / 5 - t.h;
		} else if (direction == VC_ELM_DIRECTION_BOTTOM) {
			t.x = o.x + o.w / 2 - t.w / 2;
			t.y = o.y + o.h - o.h / 5;
		} else if (direction == VC_ELM_DIRECTION_LEFT_TOP) {
			t.x = o.x + o.w / 5 - t.w;
			t.y = o.y + o.h / 5 - t.h;
		} else if (direction == VC_ELM_DIRECTION_LEFT_BOTTOM) {
			t.x = o.x + o.w / 5 - t.w;
			t.y = o.y + o.h - o.h / 5;
		} else if (direction == VC_ELM_DIRECTION_RIGHT_TOP) {
			t.x = o.x + o.w - o.w / 5;
			t.y = o.y + o.h / 5 - t.h;
		} else if (direction == VC_ELM_DIRECTION_RIGHT_BOTTOM) {
			t.x = o.x + o.w - o.w / 5;
			t.y = o.y + o.h - o.h / 5;
		}

		if (recs != NULL) {
			recs[inserted].x = t.x;
			recs[inserted].y = t.y;
			recs[inserted].w = t.w;
			recs[inserted].h = t.h;
			++inserted;
		}

		if (evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_X)) != NULL && evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_Y)) != NULL) {
			intptr_t pposition_x = (intptr_t)(evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_X)));
			intptr_t pposition_y = (intptr_t)(evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_Y)));
			int position_x = (int)pposition_x;
			int position_y = (int)pposition_y;

			if (position_x > 0)
				--position_x;
			else
				++position_x;

			if (position_y > 0)
				--position_y;
			else
				++position_y;

			t.x = position_x + o.x;
			t.y = position_y + o.y;
		}

		nx = ((t.x - mx) * SCALE) / mw;
		ny = ((t.y - my) * SCALE) / mh;
		nw = (t.w * SCALE) / mw;
		nh = (t.h * SCALE) / mh;

		elm_grid_pack_set(tip, nx, ny, nw, nh);
	}
	free(recs);
}

int _vc_elm_relayout_changed()
{
	Eina_List *l;
	Evas_Object *obj;
	int inserted = 0;
	int mw;
	int mh;
	int mx;
	int my;
	size_t length = eina_list_count(obj_list);
	R *recs = (R *)malloc(sizeof(R) * length);
	int nx;
	int ny;
	int nw;
	int nh;

	evas_object_geometry_get(g_grid, &mx, &my, &mw, &mh);

	EINA_LIST_FOREACH(obj_list, l, obj) {
		int p;
		int direction;
		R o;
		R t;
		R i;
		Evas_Object *tip = evas_object_data_get(obj, "MyTooltipHandle");
		int x;
		int y;
		int w;
		int h;
		_R_set_from_efl(&o, obj);

		elm_grid_pack_get(tip, &x, &y, &w, &h);
		t.x = ((x * mw) / SCALE) - mx;
		t.y = ((y * mh) / SCALE) - my;
		t.w = ((w * mw) / SCALE);
		t.h = ((h * mh) / SCALE);

		t.x = o.x - t.w + o.w / 2;
		t.x = t.x < 0 ? 0 : t.x;
		t.y = o.y - t.h + o.h / 2;
		t.y = t.y < 0 ? 0 : t.y;

		for (p = 0; p < inserted; ++p) {
			if (1 == _R_intersection(&t, &recs[p], &i)) {
				t.y += i.h + 1;
				if (1 == _R_intersection(&t, &recs[p], &i)) {
					t.y -= i.h + 1;
					t.x += i.w + 1;
					if (1 == _R_intersection(&t, &recs[p], &i))
						t.y += i.h + 1;
				}

			}
		}

		if (evas_object_data_get(obj, VC_ELM_DIRECTION) != NULL) {
			intptr_t pdirection = (intptr_t)evas_object_data_get(obj, VC_ELM_DIRECTION);
			direction = (int)pdirection;
			direction--;
		} else if (evas_object_data_get(obj, VC_ELM_SUB_ITEM_DIRECTION) != NULL) {
			intptr_t pdirection = (intptr_t)evas_object_data_get(obj, VC_ELM_SUB_ITEM_DIRECTION);
			direction = (int)pdirection;
			direction--;
		} else {
			direction = _vc_elm_core_get_tooltip_default_direction();
		}
		if (direction == VC_ELM_DIRECTION_CENTER) {
			t.x = o.x + o.w / 2 - t.w / 2;
			t.y = o.y + o.h / 2 - t.h / 2;
		} else if (direction == VC_ELM_DIRECTION_LEFT) {
			t.x = o.x + o.w / 5 - t.w;
			t.y = o.y + o.h / 2 - t.h / 2;
		} else if (direction == VC_ELM_DIRECTION_RIGHT) {
			t.x = o.x + o.w - o.w / 5;
			t.y = o.y + o.h / 2 - t.h / 2;
		} else if (direction == VC_ELM_DIRECTION_TOP) {
			t.x = o.x + o.w / 2 - t.w / 2;
			t.y = o.y + o.h / 5 - t.h;
		} else if (direction == VC_ELM_DIRECTION_BOTTOM) {
			t.x = o.x + o.w / 2 - t.w / 2;
			t.y = o.y + o.h - o.h / 5;
		} else if (direction == VC_ELM_DIRECTION_LEFT_TOP) {
			t.x = o.x + o.w / 5 - t.w;
			t.y = o.y + o.h / 5 - t.h;
		} else if (direction == VC_ELM_DIRECTION_LEFT_BOTTOM) {
			t.x = o.x + o.w / 5 - t.w;
			t.y = o.y + o.h - o.h / 5;
		} else if (direction == VC_ELM_DIRECTION_RIGHT_TOP) {
			t.x = o.x + o.w - o.w / 5;
			t.y = o.y + o.h / 5 - t.h;
		} else if (direction == VC_ELM_DIRECTION_RIGHT_BOTTOM) {
			t.x = o.x + o.w - o.w / 5;
			t.y = o.y + o.h - o.h / 5;
		}

		if (recs != NULL) {
			recs[inserted].x = t.x;
			recs[inserted].y = t.y;
			recs[inserted].w = t.w;
			recs[inserted].h = t.h;
			++inserted;
		}

		if (evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_X)) != NULL && evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_Y)) != NULL) {
			intptr_t pposition_x = (intptr_t)(evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_X)));
			intptr_t pposition_y = (intptr_t)(evas_object_data_get(obj, _vc_elm_get_data_key(VC_ELM_POSITION_Y)));
			int position_x = (int)pposition_x;
			int position_y = (int)pposition_y;

			t.x = position_x + o.x;
			t.y = position_y + o.y;
		}

		nx = ((t.x - mx) * SCALE) / mw;
		ny = ((t.y - my) * SCALE) / mh;
		nw = (t.w * SCALE) / mw;
		nh = (t.h * SCALE) / mh;

		/*elm_grid_pack_set(tip, nx, ny, nw, nh);*/
		VC_ELM_LOG_DBG("View changes detected: %d %d %d %d", abs(x - nx), abs(y - ny), abs(h - nh), abs(w - nw));
		if ((abs(x - nx) > 5) || (abs(y - ny) > 5) || (abs(h - nh) > 5) || (abs(w - nw) > 5)) {
			free(recs);
			return 1;
		}
	}
	free(recs);
	return 0;
}

void _vc_elm_tooltips_show_tooltip(Evas_Object *obj, const char *text)
{
	if (domain_initialized == 0) {
		_vc_elm_get_text_domain(&g_app_domain);
		if (NULL != g_app_domain)
			domain_initialized = 1;
	}

	if (text) {
		const char font_pre[] = "<font_size=24><font color=\"black\">";
		const char font_post[] = "</font></font_size>";

		size_t tooltip_content_len = strlen(font_pre) + strlen(font_post) + strlen(_APP(text));
		char *tooltip_content = (char *)calloc(tooltip_content_len + 1, sizeof(char));
		if (tooltip_content != NULL)
			tooltip_content[0] = '\0';
		else {
			VC_ELM_LOG_ERR("Fail to allocate memory");
			return;
		}

		eina_strlcat(tooltip_content, font_pre, tooltip_content_len + 1);
		eina_strlcat(tooltip_content, _APP(text), tooltip_content_len + 1);
		eina_strlcat(tooltip_content, font_post, tooltip_content_len + 1);

		_vc_elm_add_tooltip(obj, tooltip_content);
		free(tooltip_content);
	}
}
