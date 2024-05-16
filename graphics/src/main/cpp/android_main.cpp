/*
* Copyright (C) 2024 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <cstdlib>

#include <android/looper.h>

#include "game-activity/native_app_glue/android_native_app_glue.h"

#define LOGI(...)

static void HandleCmd(android_app *, int32_t) {
  // Nothing for now.
}


static bool KeyEventFilter(const GameActivityKeyEvent *) {
  return false;
}

static bool MotionEventFilter(const GameActivityMotionEvent *) {
  return false;
}

static void HandleInputEvents(android_app *app) {
  auto inputBuf = android_app_swap_input_buffers(app);
  if (inputBuf == nullptr) {
    return;
  }

  // For the minimum, apps need to process the exit event (for example,
  // listening to AKEYCODE_BACK). This sample has done that in the Kotlin side
  // and not processing other input events, we just reset the event counter
  // inside the android_input_buffer to keep app glue code in a working state.
  android_app_clear_motion_events(inputBuf);
  android_app_clear_motion_events(inputBuf);
}


void android_main(android_app* app) {
  app->onAppCmd = HandleCmd;

  android_app_set_key_event_filter(app, KeyEventFilter);
  android_app_set_motion_event_filter(app, MotionEventFilter);

  while (!app->destroyRequested) {
    android_poll_source* poll_source = nullptr;
    auto result = ALooper_pollOnce(-1, nullptr, nullptr, reinterpret_cast<void**>(&poll_source));
    if (result == ALOOPER_POLL_ERROR) {
      std::abort();
    }

    if (poll_source != nullptr) {
      poll_source->process(app, poll_source);
    }

    HandleInputEvents(app);
  }
}