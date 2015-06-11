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
#include "vc_elm_rectangle.h"

static const char VC_ELM_FIXED_X[] = "vc_elm_fixed_x";
static const char VC_ELM_FIXED_Y[] = "vc_elm_fixed_y";

/**
 * @brief Function that set structure R dimensions from Evas_Object settings
 * @param[in] pointer to structure R that will be filled with dimensions info
 * @param[in] pointer to Evas_Object
 */
void _R_set_from_efl(R *p, Evas_Object *obj)
{
	int x;
	int y;
	int w;
	int h;
	int *tx;
	int *ty;
	evas_object_geometry_get(obj, &x, &y, &w, &h);
	tx = (int *)evas_object_data_get(obj, VC_ELM_FIXED_X);
	ty = (int *)evas_object_data_get(obj, VC_ELM_FIXED_Y);
	if (tx != 0 && ty != 0) {
		x = (int)tx;
		y = (int)ty;
	}
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
}

#if !defined max
#define max(a, b) (a > b ? a : b)
#endif

#if!defined min
#define min(a, b) (a < b ? a : b)
#endif

/**
 * @brief Function that check intersection of two R structures.
 * @param[in] pointer to first structure R1
 * @param[in] pointer to second structure R2
 * @param[out] pointer to structure R that is intersection of provided R1 and R2
 */
int _R_intersection(R *r1, R *r2, R *out)
{
	int leftX = max(r1->x, r2->x);
	int rightX = min(r1->x + r1->w, r2->x + r2->w);
	int topY = max(r1->y, r2->y);
	int bottomY = min(r1->y + r1->h, r2->y + r2->h);

	int W = rightX - leftX;
	int H = bottomY - topY;
	out->x = leftX;
	out->y = topY;
	out->w = rightX - leftX;
	out->h = bottomY - topY;

	if (W < 0 || H < 0)
		return 0;

	return 1;
}
