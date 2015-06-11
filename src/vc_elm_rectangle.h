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


#ifndef __VC_ELM_RECTANGLE_H__
#define __VC_ELM_RECTANGLE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "vc_elm_main.h"

/**
 * @brief Structure that contains Evas_Object dimensions
 */
 struct _R {
	int x;
	int y;
	int w;
	int h;
};

typedef struct _R R;

/**
 * @brief Function that set structure R dimensions from Evas_Object settings
 * @param[in] pointer to structure R that will be filled with dimensions info
 * @param[in] pointer to Evas_Object
 */
void _R_set_from_efl(R *p, Evas_Object *obj);

/**
 * @brief Function that check intersection of two R structures.
 * @param[in] pointer to first structure R1
 * @param[in] pointer to second structure R2
 * @param[out] pointer to structure R that is intersection of provided R1 and R2
 */
int _R_intersection(R *r1, R *r2, R *out);


#ifdef __cplusplus
}
#endif

#endif	/* __VC_ELM_RECTANGLE_H__ */
