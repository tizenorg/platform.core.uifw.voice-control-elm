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


/**
 * @defgroup VOICE_CONTROL_ELEMENTARY_MODULE Voice control elementary
 * @ingroup CAPI_UIX_FRAMEWORK
 * @brief The @ref VOICE_CONTROL_ELEMENTARY_MODULE API provides functions to control widget by voice commands.
 * 
 * @section VOICE_CONTROL_ELEMENTARY_MODULE_HEADER Required Header
 *   \#include <voice_control_elm.h>
 * 
 * @section VOICE_CONTROL_ELEMENTARY_MODULE_OVERVIEW Overview
 * The voice control elementary API is provided for controlling widgets by voice commands.
 * The voice UI control provided by voice control elementary is done the following way:
 *
 * - Each widget supported and currently visible in the application is given a text hint.<br>
 * - Widget hints are shown on the User's wish.<br> 
 * - User speaks the hint name of the widget he/she wants to trigger and the action he/she wants to perform on that widget.<br>
 * - The voice command is interpreted and the requested action on the widget is performed.<br>
 *
 * To use of voice control elementary, use the following steps: <br>
 * 1. Initialize voice control elementary <br>
 * 2. Register callback functions for notifications <br>
 * 3. Create a handle after creating an evas object or elementary object item <br>
 * 4. Set command and hint <br>
 * 5. Set hint direction if you want to change hint position <br>
 * 6. Run action mapped widget which is spoken by user <br>
 * 7. Destroy handle <br>
 * 8. Uninitialize voice control elementary <br>
 *
 * When window is updated by user's operation with touch, key or etc, it is needed to set command and hint creating evas object or elementary object item.<br>
 * And the callback function of action related to widget is called internally, when user speaks a particular widget's hint.
 *
 * @image html sample_app_hints.png Example: your application layout with voice control elementary enabled with default settings.
 * 
 * Here is an example code for supporting voice control elm module :
 * @code
 * #include <app.h>
 * #include <voice_control_elm.h>
 *
 * void app_control(app_control_h app_control, void *user_data)
 * {
 *		vc_elm_initialize();
 *		vc_elm_set_current_language_changed_cb(_vc_elm_language_changed_cb, NULL); 
 *		elm_main(0, (char **)NULL);
 * }
 * 
 * void app_terminate(void *user_data)
 * {
 *		vc_elm_deinitialize();
 * }
 * 
 * int main(int argc, char *argv[])
 * {
 *		struct appdata ad;
 *		ui_app_lifecycle_callback_s event_callback = {0,};
 *
 *		event_callback.create = app_create;
 *		event_callback.terminate = app_terminate;
 *		event_callback.pause = NULL;
 *		event_callback.resume = NULL;
 *		event_callback.app_control = NULL;
 *
 *		memset(&ad, 0x0, sizeof(struct appdata));
 *
 *		return ui_app_main(argc, argv, &event_callback, &ad);
 * }
 * @endcode
 * 
 * If you want to create some widgets with voice control, you can refer below function. 
 *
 * @code
 * int elm_main(int argc, char **argv)
 * {
 *		Evas_Object* win;
 *		Evas_Object* btn;
 *		vc_elm_h vc_elm_buttons_h;
 *
 *		win = elm_win_add(NULL, "example", ELM_WIN_BASIC);
 *		elm_win_title_set(win, "example");
 *		elm_win_autodel_set(win, EINA_TRUE);
 *		evas_object_resize(win, 720, 1280);
 *		evas_object_show(win);
 *
 *		btn = elm_button_add(win);
 *		elm_object_text_set(btn, "Button");
 *		evas_object_move(btn, 50, 50);
 *		evas_object_resize(btn, 200, 50);
 *
 *		vc_elm_create_object(btn, &vc_elm_buttons_h);
 *		vc_elm_set_command(vc_elm_buttons_h, "Button");
 *		vc_elm_set_command_hint(vc_elm_buttons_h, "Button");
 *
 *		evas_object_show(btn);
 *		return 0;
 * }
 * @endcode
 *
 * If you want to support several language for voice control elm module, you need to set language changed callback function. 
 *
 * @code
 * void _vc_elm_language_changed_cb(void)
 * {
 *		// You need to update codes in order to change text of command & hint according to changed language.
 * }
 * @endcode
 * 
 * @section VOICE_CONTROL_ELEMENTARY_MODULE_FEATURE Related Features
 * This API is related with the following features:<br>
 *  - http://tizen.org/feature/microphone<br>
 *  - http://tizen.org/feature/speech.control<br>
 *
 * It is recommended to design feature related codes in your application for reliability.<br>
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.<br>
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.<br>
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 * 
 */



