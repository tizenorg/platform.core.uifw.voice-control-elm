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
#include "vc_elm_widget_wrapper.h"
#include "vc_elm_tools.h"
#include "vc_elm_core.h"

#include <voice_control_widget.h>

struct __voice_control_wrapper_s {
	bool initialized;
	vc_cmd_list_h group;
	char *lang;
	int (*result_cb)(const char *cmd, const char *param1, const char *param2, void *data);
	void (*tooltips_cb)(bool);
	vc_current_language_changed_cb language_changed_cb;
	void *lang_user_data;
	void *data;
};

static struct __voice_control_wrapper_s vcw = {0, 0, 0, 0, 0, 0, 0, 0};

static void __vc_state_changed_cb(vc_state_e previous, vc_state_e current, void *user_data);
static void __vc_service_state_changed_cb(vc_service_state_e previous, vc_service_state_e current, void *user_data);
static void __vc_error_cb(vc_error_e reason, void *user_data);
static void __vc_result_cb(vc_result_event_e event, vc_cmd_list_h vc_cmd_list, const char *result, void *user_data);
static void __vc_show_tooltip_callback(bool show, void *data);
static void __vc_language_changed_cb(const char *previous, const char *current, void *data);

#define VC_CMD_ERROR_CHECK_CASE(VALUE)				\
	case VALUE:						\
	VC_ELM_LOG_ERR("vc error in (%s) -> %s", msg, #VALUE);	\
	break;

#define VC_ERROR_CHECK(args) do {							\
		int err = args;								\
		const char *msg = # args;						\
		switch (err) {							\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_OUT_OF_MEMORY);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_IO_ERROR);		\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_INVALID_PARAMETER);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_TIMED_OUT);		\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_RECORDER_BUSY);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_PERMISSION_DENIED);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_NOT_SUPPORTED);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_INVALID_STATE);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_INVALID_LANGUAGE);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_ENGINE_NOT_FOUND);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_OPERATION_FAILED);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_OPERATION_REJECTED);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_ITERATION_END);	\
			VC_CMD_ERROR_CHECK_CASE(VC_ERROR_EMPTY);		\
		case VC_ERROR_NONE:					\
			VC_ELM_LOG_DBG("NO error in (%s)", msg);	\
			break;						\
		default:						\
			VC_ELM_LOG_ERR("Unkown error in (%s)", msg);	\
		}								\
	} while (0)

static void __vc_widget_send_current_command_group_cb(vc_cmd_list_h *vc_group, void *user_data)
{
	(void)user_data;
	*vc_group = vcw.group;

	VC_ELM_LOG_DBG("SEND CURRENT COMMAND GROUP cb CALLED");
}

int _vc_elm_widget_wrapper_initialize()
{
	char *lang = NULL;
	if (vcw.initialized) {
		VC_ELM_LOG_ERR("already initialized");
		return 0;
	}
	vcw.initialized = 1;

	VC_ERROR_CHECK(vc_widget_initialize());
	VC_ERROR_CHECK(vc_widget_set_error_cb(&__vc_error_cb, NULL));
	VC_ERROR_CHECK(vc_widget_set_show_tooltip_cb(&__vc_show_tooltip_callback, NULL));
	VC_ERROR_CHECK(vc_widget_set_result_cb(&__vc_result_cb, NULL));
	VC_ERROR_CHECK(vc_widget_set_state_changed_cb(&__vc_state_changed_cb, NULL));
	VC_ERROR_CHECK(vc_widget_set_service_state_changed_cb(&__vc_service_state_changed_cb, NULL));
	VC_ERROR_CHECK(vc_widget_set_current_language_changed_cb(&__vc_language_changed_cb, NULL));
	VC_ERROR_CHECK(vc_widget_set_send_current_command_list_cb(__vc_widget_send_current_command_group_cb, NULL));
	VC_ERROR_CHECK(vc_widget_prepare());
	VC_ELM_LOG_DBG("VC default language used %s", lang);
	vcw.lang = lang;

	return 1;
}

int _vc_elm_widget_wrapper_deinitialize()
{
	if (vcw.group) {
		VC_ERROR_CHECK(vc_cmd_list_destroy(vcw.group, true));
		vcw.group = NULL;
	}

	if (vcw.lang) {
		free(vcw.lang);
		vcw.lang = NULL;
	}

	VC_ERROR_CHECK(vc_widget_unprepare());
	VC_ERROR_CHECK(vc_widget_deinitialize());
	vcw.initialized = 0;

	return 0;
}

static bool __cb(vc_cmd_h vc_command, void *user_data)
{
	char *cmd = NULL;
	vc_cmd_format_e type;
	(void)user_data;

	vc_cmd_get_format(vc_command, &type);
	vc_cmd_get_command(vc_command, &cmd);
	VC_ELM_LOG_DBG("cmd: %s %d", cmd, type);
	return 1;
}

int _vc_elm_widget_wrapper_commit_commands(vc_elm_widget_wrapper_result_cb callback, void *data)
{
	if (!vcw.group) {
		VC_ELM_LOG_WARN("commit 2");
		return -1;
	}

	VC_ELM_LOG_WARN("commit %p", (void*)vcw.group);
	vc_cmd_list_foreach_commands(vcw.group, &__cb, NULL);

	vcw.result_cb = callback;
	vcw.data = data;
	VC_ELM_LOG_WARN("commit ok");
	return 0;
}

int _vc_elm_widget_wrapper_add_command(const char *cmd, const char *param1)
{
	vc_cmd_h cmdh;
	size_t len = 1;
	char* command;
	if (!vcw.group) {
		VC_ERROR_CHECK(vc_cmd_list_create(&vcw.group));
		VC_ELM_LOG_WARN("new_command_group %p", (void*)vcw.group);
	}
	if (NULL == cmd)
		return VC_ELM_ERROR_INVALID_PARAMETER;
	len += strlen(cmd);
	if (NULL != param1) {
		len = len + strlen(param1) + 1;
		command = (char *)calloc(len, sizeof(char));
		if (NULL == command) {
			VC_ELM_LOG_ERR("Fail to allocate memory");
			return -1;
		}
		snprintf(command, len, "%s %s", cmd, param1);
	} else {
		command = (char *)calloc(len, sizeof(char));
		if (NULL == command) {
			VC_ELM_LOG_ERR("Fail to allocate memory");
			return -1;
		}
		snprintf(command, len, "%s", cmd);
	}
	VC_ERROR_CHECK(vc_cmd_create(&cmdh));
	VC_ERROR_CHECK(vc_cmd_set_command(cmdh, command));
	VC_ERROR_CHECK(vc_cmd_set_format(cmdh, VC_CMD_FORMAT_FIXED));
	VC_ERROR_CHECK(vc_cmd_set_type(cmdh, VC_COMMAND_TYPE_WIDGET));
	VC_ERROR_CHECK(vc_cmd_list_add(vcw.group, cmdh));

	free(command);
	return 0;
}

static void __vc_state_changed_cb(vc_state_e previous, vc_state_e current, void *user_data)
{
	const char *prev = NULL;
	const char *curr = NULL;
	char *tmp_lang = NULL;
	char *lang = NULL;
	(void)user_data;

	switch (previous) {
	case VC_STATE_INITIALIZED:
		prev = "VC_STATE_INITIALIZED";
		break;
	case VC_STATE_READY:
		prev = "VC_STATE_READY";
		break;
	case VC_STATE_NONE:
		curr = "VC_STATE_READY";
		break;
	default:
		VC_ELM_LOG_ERR("unkown value");
		exit(-1);
	}

	switch (current) {
	case VC_STATE_INITIALIZED:
		curr = "VC_STATE_INITIALIZED";
		break;
	case VC_STATE_READY:
		curr = "VC_STATE_READY";
		vc_widget_set_foreground(EINA_TRUE);
		vc_widget_get_current_language(&tmp_lang);
		if (NULL != tmp_lang) {
			int ret = asprintf(&lang, "%s.UTF-8", tmp_lang);
			if (-1 == ret) {
				VC_ELM_LOG_ERR("error changing locale");
				exit(-1);
			}
			VC_ELM_LOG_DBG("locale changing - %s", setlocale(LC_ALL, lang));
		}
		break;
	case VC_STATE_NONE:
		curr = "VC_STATE_READY";
		break;
	default:
		VC_ELM_LOG_ERR("unkown value");
		exit(-1);
	}

	free(lang);
	free(tmp_lang);

	VC_ELM_LOG_DBG("VC state changed from %s to %s", prev, curr);
}

static void __vc_service_state_changed_cb(vc_service_state_e previous, vc_service_state_e current, void *user_data)
{
	(void)user_data;

	if (previous == VC_SERVICE_STATE_RECORDING)
		_vc_elm_core_unregister_view_change_detection();
	else if (current == VC_SERVICE_STATE_RECORDING)
		_vc_elm_core_register_view_change_detection();


	if ((VC_SERVICE_STATE_PROCESSING == previous && VC_SERVICE_STATE_READY == current) || (VC_SERVICE_STATE_RECORDING == previous && VC_SERVICE_STATE_READY == current)) {
		VC_ELM_LOG_DBG("VC Service processing ends, clear commands");
		if (NULL != vcw.group) {
			vc_cmd_list_destroy(vcw.group, true);
			vcw.group = NULL;
		}
	}
}

static void __vc_error_cb(vc_error_e reason, void *user_data)
{
	(void)user_data;
	VC_ELM_LOG_ERR("VC Error occured : %d", reason);
}

static void __vc_result_cb(vc_result_event_e event, vc_cmd_list_h vc_cmd_list, const char *result, void *user_data)
{
	vc_cmd_h vc_command = NULL;
	char *cmd_name = NULL;
	int ret = 0;
	vc_cmd_format_e type;
	(void)user_data;
	(void)result;
	if (event != VC_RESULT_EVENT_RESULT_SUCCESS)
		return;

	vc_cmd_list_first(vc_cmd_list);
	while (VC_ERROR_ITERATION_END != ret) {
		if (0 != vc_cmd_list_get_current(vc_cmd_list, &vc_command)) {
			VC_ELM_LOG_ERR("cannot get command from list");
			break;
		}

		if (NULL == vc_command)
			break;

		VC_ERROR_CHECK(vc_cmd_get_format(vc_command, &type));
		VC_ERROR_CHECK(vc_cmd_get_command(vc_command, &cmd_name));

		if (vcw.result_cb) {
			vcw.result_cb(cmd_name, NULL, NULL, vcw.data);
			break;
		}

		ret = vc_cmd_list_next(vc_cmd_list);
	}

	free(cmd_name);
}

void __vc_show_tooltip_callback(bool show, void *data)
{
	(void)data;
	VC_ELM_LOG_DBG("show tooltips %d", show);

	if (vcw.tooltips_cb && _vc_elm_core_get_tooltip_show())
		vcw.tooltips_cb(show);
}

void _vc_elm_widget_wrapper_set_show_tooltips_callback(vc_elm_widget_wrapper_show_tooltips_callback callback)
{
	vcw.tooltips_cb = callback;
}

static void __vc_language_changed_cb(const char *previous, const char *current, void *data)
{
	char *lang;
	int ret;
	(void)previous;
	(void)current;
	(void)data;

	VC_ELM_LOG_INFO("Widget language changed from %s to %s", previous, current);

	ret = asprintf(&lang, "%s.UTF-8", current);
	if (-1 == ret) {
		VC_ELM_LOG_ERR("Error setting locale");
		return;
	}
	VC_ELM_LOG_INFO("Locale changing - %s", setlocale(LC_ALL, lang));
	free(lang);

	if (NULL != vcw.language_changed_cb) {
		vcw.language_changed_cb(previous, current, vcw.lang_user_data);
	} else {
		VC_ELM_LOG_DBG("Not registered language changed callback");
	}
	return;
}

int _vc_elm_widget_wrapper_set_current_language_changed_callback(vc_elm_widget_wrapper_language_changed_callback callback, void *data)
{
	if (NULL == callback) {
		VC_ELM_LOG_ERR("Null parameter");
		return -1;
	}

	vcw.language_changed_cb = callback;
	vcw.lang_user_data = data;
	return 0;

}

int _vc_elm_widget_wrapper_unset_current_language_changed_callback()
{
	if (NULL == vcw.language_changed_cb) {
		VC_ELM_LOG_ERR("No registered cb");
		return -1;
	}

	vcw.language_changed_cb = NULL;
	vcw.lang_user_data = NULL;
	return 0;
}

void _vc_elm_widget_wrapper_clear_commands()
{
	if (NULL != vcw.group) {
		vc_cmd_list_destroy(vcw.group, true);
		vcw.group = NULL;
	}
}

int _vc_elm_widget_wrapper_cancel()
{
	int ret = VC_ELM_ERROR_NONE;
	VC_ELM_LOG_DBG("===========");

	ret = vc_widget_cancel();
	if (VC_ELM_ERROR_NONE != ret) {
		VC_ELM_LOG_ERR("Error while widget cancel (%d)", ret);
	}
	return ret;
}

