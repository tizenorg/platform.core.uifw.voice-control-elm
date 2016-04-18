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


#ifndef __VC_ELM_TOOLS_H__
#define __VC_ELM_TOOLS_H__

#ifdef __cplusplus
extern "C" {
#endif


#pragma GCC system_header
#include "vc_elm_main.h"

#define LOG_TAG "VC_ELM"

#define VC_ELM_LOG_DBG(fmt, ...) do { EINA_LOG_ERR("\033[0;32m" fmt "\033[m", ## __VA_ARGS__); SLOGI("\033[0;32m" fmt "\033[m", ## __VA_ARGS__); } while (0)
#define VC_ELM_LOG_INFO(fmt, ...) do { EINA_LOG_ERR("\033[0;35m" fmt "\033[m", ## __VA_ARGS__); SLOGI("\033[0;32m" fmt "\033[m", ## __VA_ARGS__); } while (0)
#define VC_ELM_LOG_ERR(fmt, ...) do { EINA_LOG_ERR("\033[0;31m" fmt "\033[m", ## __VA_ARGS__); SLOGE("\033[0;32m" fmt "\033[m", ## __VA_ARGS__); } while (0)
#define VC_ELM_LOG_WARN(fmt, ...) do { EINA_LOG_ERR("\033[0;36m" fmt "\033[m", ## __VA_ARGS__); SLOGW("\033[0;32m" fmt "\033[m", ## __VA_ARGS__); } while (0)


#ifdef __cplusplus
}
#endif

#endif/*__VC_ELM_TOOLS_H__*/
