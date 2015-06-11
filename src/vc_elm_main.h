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


#ifndef __VC_ELM_MAIN_H__
#define __VC_ELM_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif


#pragma GCC system_header
#define _GNU_SOURCE

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Elementary.h>
#include <dlog.h>

#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <elm_widget.h>
#undef ELM_INTERNAL_API_ARGESFSDFEFC

static inline Evas_Object *__safe_efl_cast(const Evas_Object *cobj)
{
	union {
		Evas_Object *obj;
		const Evas_Object *cobj;
	} helper;

	helper.cobj = cobj;
	return helper.obj;
}

void *eo_data_scope_get(const Eo *obj, const Eo_Class *klass);


#ifdef __cplusplus
}
#endif

#endif/*__VC_ELM_MAIN_H__*/
