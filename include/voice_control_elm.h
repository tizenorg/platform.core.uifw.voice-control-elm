/*
* Copyright (c) 2011-2015 Samsung Electronics Co., Ltd All Rights Reserved
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


#ifndef VOICE_CONTROL_ELEMENTARY_H_
#define VOICE_CONTROL_ELEMENTARY_H_


#include <Elementary.h>
#include <Evas.h>
#include <tizen.h>

/**
* @addtogroup VOICE_CONTROL_ELEMENTARY_MODULE
* @{
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
* @file voice_control_elm.h
* @brief This file contains the voice control elementary API and related handle definitions and enums.
*/

/**
* @brief This enum describes status of voice control elementary after API call
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*/
typedef enum {
	VC_ELM_ERROR_NONE		= TIZEN_ERROR_NONE,			/**< Successful */
	VC_ELM_ERROR_OUT_OF_MEMORY	= TIZEN_ERROR_OUT_OF_MEMORY,		/**< Out of Memory */
	VC_ELM_ERROR_IO_ERROR		= TIZEN_ERROR_IO_ERROR,			/**< I/O error */
	VC_ELM_ERROR_INVALID_PARAMETER	= TIZEN_ERROR_INVALID_PARAMETER,	/**< Invalid parameter */
	VC_ELM_ERROR_PERMISSION_DENIED	= TIZEN_ERROR_PERMISSION_DENIED,	/**< Permission denied */
	VC_ELM_ERROR_NOT_SUPPORTED	= TIZEN_ERROR_NOT_SUPPORTED,		/**< voice control elementary NOT supported */
	VC_ELM_ERROR_INVALID_STATE	= TIZEN_ERROR_VOICE_CONTROL | 0x021,	/**< Invalid state */
	VC_ELM_ERROR_INVALID_LANGUAGE	= TIZEN_ERROR_VOICE_CONTROL | 0x022,	/**< Invalid language */
	VC_ELM_ERROR_OPERATION_FAILED	= TIZEN_ERROR_VOICE_CONTROL | 0x023,	/**< Operation failed */
	VC_ELM_ERROR_OPERATION_REJECTED = TIZEN_ERROR_VOICE_CONTROL | 0x024	/**< Operation rejected */
} vc_elm_error_e;

/**
* @brief This enum describes directions of the widget hints.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*
* @see vc_elm_set_command_hint_direction()
*
* @image html hints_orientation.png Example: various hints orientations.
*/
typedef enum {
	VC_ELM_DIRECTION_CENTER		= 0,	/**< Direction displayed to the center of the widget */
	VC_ELM_DIRECTION_LEFT		= 1,	/**< Direction displayed to the left of the widget */
	VC_ELM_DIRECTION_RIGHT		= 2,	/**< Direction displayed to the right of the widget */
	VC_ELM_DIRECTION_TOP		= 3,	/**< Direction displayed to the top of the widget */
	VC_ELM_DIRECTION_BOTTOM		= 4,	/**< Direction displayed to the bottom of the widget */
	VC_ELM_DIRECTION_LEFT_TOP	= 5,	/**< Direction displayed to the top left corner of the widget */
	VC_ELM_DIRECTION_LEFT_BOTTOM	= 6,	/**< Direction displayed to the bottom left corner of the widget */
	VC_ELM_DIRECTION_RIGHT_TOP	= 7,	/**< Direction displayed to the top right corner of the widget */
	VC_ELM_DIRECTION_RIGHT_BOTTOM	= 8	/**< Direction displayed to the bottom right corner of the widget */
} vc_elm_direction_e;

/**
* @brief A handle to the voice control elementary for object or item object
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*/
typedef struct vc_elm_s* vc_elm_h;

/**
* @brief Called to retrieve supported language.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*
* @param[in] language A language is specified as an ISO 3166 alpha-2 two letter country-code \n
*		followed by ISO 639-1 for the two-letter language code \n
*		For example, "ko_KR" for Korean, "en_US" for American English
* @param[in] user_data The user data passed from the foreach function
*
* @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
* @pre The function will invoke this callback.
*
* @see vc_elm_foreach_supported_languages()
*/
typedef bool (*vc_elm_supported_language_cb)(const char* language, void* user_data);

/**
* @brief Called when default language is changed.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*
* @param[in] previous Previous language
* @param[in] current Current language
* @param[in] user_data The user data passed from the callback registration function
*
* @pre An application registers this callback to detect changing language.
*
* @see vc_elm_set_current_language_changed_cb()
*/
typedef void (*vc_elm_current_language_changed_cb)(const char* previous, const char* current, void* user_data);

/**
* @brief Called to retrieve supported widget.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*
* @param[in] widget Widget name
* @param[in] user_data The user data passed from the callback registration function
*
* @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
* @pre The function will invoke this callback.
*
* @see vc_elm_foreach_supported_widgets()
*/
typedef bool (*vc_elm_widget_cb)(const char* widget, void* user_data);

/**
* @brief Called to retrieve supported action.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
*
* @param[in] action Action name
* @param[in] user_data The user data passed from the callback registration function
*
* @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
* @pre The function will invoke this callback.
*
* @see vc_elm_foreach_supported_actions()
*/
typedef bool (*vc_elm_action_cb)(const char* action, void* user_data);


/**
* @brief Initializes voice control elementary module.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @remarks If the function succeeds, the voice control elementary must be released with vc_elm_deinitialize().
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_OUT_OF_MEMORY Out of memory
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @see vc_elm_deinitialize()
*/
int vc_elm_initialize(void);

/**
* @brief Deinitializes voice control elementary module.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_OUT_OF_MEMORY Out of memory
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*
* @see vc_elm_initialize()
*/
int vc_elm_deinitialize(void);

/**
* @brief Retrieves all supported languages using callback function.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] callback Callback function to invoke
* @param[in] user_data The user data to be passed to the callback function
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
* @post	This function invokes vc_supported_language_cb() repeatedly for getting languages.
*
* @see vc_elm_supported_language_cb()
* @see vc_elm_get_current_language()
*/
int vc_elm_foreach_supported_languages(vc_elm_supported_language_cb callback, void* user_data);

/**
* @brief Gets current language.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @remark If the function succeeds, @a language must be released with free() by you when you no longer need it.
*
* @param[out] language A language is specified as an ISO 3166 alpha-2 two letter country-code \n
*			followed by ISO 639-1 for the two-letter language code \n
*			For example, "ko_KR" for Korean, "en_US" for American English
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OUT_OF_MEMORY Out of memory
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*/
int vc_elm_get_current_language(char** language);

/**
* @brief Retrieves all supported widget using callback function.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] callback Callback function to invoke
* @param[in] user_data The user data to be passed to the callback function
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
* @post	This function invokes vc_elm_widget_cb() repeatedly for getting widgets.
*
* @see vc_elm_widget_cb()
* @see vc_elm_foreach_supported_actions()
*/
int vc_elm_foreach_supported_widgets(vc_elm_widget_cb callback, void* user_data);

/**
* @brief Retrieves all supported actions of widget using callback function.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] widget Widget name
* @param[in] callback Callback function to invoke
* @param[in] user_data The user data to be passed to the callback function
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful and widget name can get from vc_elm_foreach_supported_widgets().
* @post	This function invokes vc_elm_action_cb() repeatedly for getting actions.
*
* @see vc_elm_action_cb()
* @see vc_elm_foreach_supported_widgets()
*/
int vc_elm_foreach_supported_actions(const char* widget, vc_elm_action_cb callback, void* user_data);

/**
* @brief Gets action command of action.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @remark If the function succeeds, @a command must be released with free() by you when you no longer need it.  \n
*	If command is NULL, the action is default action. The default action works by only command of widget.
*
* @param[in] action Action name
* @param[out] command Command for action
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OUT_OF_MEMORY Out of memory
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*/
int vc_elm_get_action_command(const char* action, char** command);

/**
* @brief Creates vc elm handle for evas object.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @remarks If the function succeeds, The vc elm handle must be released with vc_elm_destroy().
*
* @param[in] object Evas object included in handle
* @param[out] vc_elm The voice control elementary handle
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_OUT_OF_MEMORY Out of memory
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_OPERATION_REJECTED Operation rejected when widget is not supported
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*
* @see vc_elm_destroy()
*/
int vc_elm_create_object(Evas_Object* object, vc_elm_h* vc_elm);

/**
* @brief Creates vc elm handle for elm object item.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @remarks If the function succeeds, The vc elm handle must be released with vc_elm_destroy().
*
* @param[in] item The elm Object item included in handle
* @param[out] vc_elm Handle containing pointer to widget
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_OUT_OF_MEMORY Out of memory
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*
* @see vc_elm_destroy()
*/
int vc_elm_create_item(Elm_Object_Item* item, vc_elm_h* vc_elm);

/**
* @brief Destroys the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_create_object()
* @see vc_elm_create_item()
*/
int vc_elm_destroy(vc_elm_h vc_elm);

/**
* @brief Sets command to the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
* @param[in] command Command's text
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful.
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_unset_command()
*/
int vc_elm_set_command(vc_elm_h vc_elm, const char* command);

/**
* @brief Unsets command from the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle to include pointer to widget
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_set_command()
*/
int vc_elm_unset_command(vc_elm_h vc_elm);

/**
* @brief Set command hint for the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
* @param[in] hint Hint's text
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_unset_command_hint()
*/
int vc_elm_set_command_hint(vc_elm_h vc_elm, const char* hint);

/**
* @brief Unset command hint for the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_set_command_hint()
*/
int vc_elm_unset_command_hint(vc_elm_h vc_elm);

/**
* @brief Sets the direction of hint to the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @remarks If direction is not set, default direction is pre-configured will be used.
*
* @param[in] vc_elm Handle containing pointer to widget
* @param[in] direction Direction of hint defined by #vc_elm_direction_e
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_set_command_hint()
* @see vc_elm_get_command_hint_direction()
*/
int vc_elm_set_command_hint_direction(vc_elm_h vc_elm, vc_elm_direction_e direction);

/**
* @brief Unsets the direction of hint from the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
* @param[out] direction Direction of hint defined by #vc_elm_direction_e
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_set_command_hint_direction()
*/
int vc_elm_get_command_hint_direction(vc_elm_h vc_elm, vc_elm_direction_e* direction);

/**
* @brief Sets command hint's x,y position to the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
* @param[in] pos_x The x position of hint
* @param[in] pos_y The y position of hint
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_set_command_hint()
* @see vc_elm_get_command_hint_offset()
*/
int vc_elm_set_command_hint_offset(vc_elm_h vc_elm, int pos_x, int pos_y);

/**
* @brief Gets command hint's x,y position from the handle.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] vc_elm Handle containing pointer to widget
* @param[out] pos_x The x position of hint
* @param[out] pos_y The y position of hint
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_OPERATION_FAILED Operation failed
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre The vc_elm handle should be valid with vc_elm_create_object() or vc_elm_create_item().
*
* @see vc_elm_set_command_hint_offset()
*/
int vc_elm_get_command_hint_offset(vc_elm_h vc_elm, int* pos_x, int* pos_y);


/**
* @brief Registers a callback function to be called when current language is changed.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @param[in] callback Callback function to register
* @param[in] user_data The user data to be passed to the callback function
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*
* @see vc_elm_unset_current_language_changed_cb()
*/
int vc_elm_set_current_language_changed_cb(vc_elm_current_language_changed_cb callback, void* user_data);

/**
* @brief Unregisters the callback function.
* @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
* @privlevel public
* @privilege %http://tizen.org/privilege/recorder
*
* @return 0 on success, otherwise a negative error value
* @retval #VC_ELM_ERROR_NONE Successful
* @retval #VC_ELM_ERROR_INVALID_STATE Invalid state
* @retval #VC_ELM_ERROR_PERMISSION_DENIED Permission denied
* @retval #VC_ELM_ERROR_NOT_SUPPORTED Not supported
*
* @pre vc_elm_initialize() should be successful.
*
* @see vc_elm_set_current_language_changed_cb()
*/
int vc_elm_unset_current_language_changed_cb(void);


#ifdef __cplusplus
}
#endif

/**
 * @}@}
 */

#endif /* VOICE_CONTROL_ELEMENTARY_H_ */
