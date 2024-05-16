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

#include "engine.h"

#include <android/choreographer.h>

#include <cstdlib>

#include "logging.h"

namespace ndksamples::graphics {

static void HandleCmd(android_app * _Nonnull app, int32_t cmd) {
  CHECK_NE(app, nullptr);
  auto engine = reinterpret_cast<Engine*>(app->userData);
  CHECK_NE(engine, nullptr);
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
      LOG(WARNING) << "Unhandled APP_CMD_INIT_WINDOW";
      break;
    case APP_CMD_TERM_WINDOW:
      LOG(WARNING) << "Unhandled APP_CMD_TERM_WINDOW";
      break;
    case APP_CMD_WINDOW_RESIZED:
      LOG(WARNING) << "Unhandled APP_CMD_WINDOW_RESIZED";
      break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
      LOG(WARNING) << "Unhandled APP_CMD_WINDOW_REDRAW_NEEDED";
      break;
    case APP_CMD_CONTENT_RECT_CHANGED:
      LOG(WARNING) << "Unhandled APP_CMD_CONTENT_RECT_CHANGED";
      break;
    case APP_CMD_SOFTWARE_KB_VIS_CHANGED:
      LOG(WARNING) << "Unhandled APP_CMD_SOFTWARE_KB_VIS_CHANGED";
      break;
    case APP_CMD_GAINED_FOCUS:
      engine->Resume();
      break;
    case APP_CMD_LOST_FOCUS:
      engine->Pause();
      break;
    case APP_CMD_CONFIG_CHANGED:
      LOG(WARNING) << "Unhandled APP_CMD_CONFIG_CHANGED";
      break;
    case APP_CMD_LOW_MEMORY:
      LOG(WARNING) << "Unhandled APP_CMD_LOW_MEMORY";
      break;
    case APP_CMD_START:
      LOG(WARNING) << "Unhandled APP_CMD_START";
      break;
    case APP_CMD_RESUME:
      LOG(WARNING) << "Unhandled APP_CMD_RESUME";
      break;
    case APP_CMD_SAVE_STATE:
      LOG(WARNING) << "Unhandled APP_CMD_SAVE_STATE";
      break;
    case APP_CMD_PAUSE:
      LOG(WARNING) << "Unhandled APP_CMD_PAUSE";
      break;
    case APP_CMD_STOP:
      LOG(WARNING) << "Unhandled APP_CMD_STOP";
      break;
    case APP_CMD_DESTROY:
      LOG(WARNING) << "Unhandled APP_CMD_DESTROY";
      break;
    case APP_CMD_WINDOW_INSETS_CHANGED:
      LOG(WARNING) << "Unhandled APP_CMD_WINDOW_INSETS_CHANGED";
      break;
    case APP_CMD_EDITOR_ACTION:
      LOG(WARNING) << "Unhandled APP_CMD_EDITOR_ACTION";
      break;
    case APP_CMD_KEY_EVENT:
      LOG(WARNING) << "Unhandled APP_CMD_KEY_EVENT";
      break;
    case APP_CMD_TOUCH_EVENT:
      LOG(WARNING) << "Unhandled APP_CMD_TOUCH_EVENT";
      break;
    default:
      LOG(ERROR) << "Unknown app command: " << cmd;
      break;
  }
}

static bool KeyEventFilter(const GameActivityKeyEvent *) { return false; }

static bool MotionEventFilter(const GameActivityMotionEvent *) { return false; }

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

Engine::Engine(android_app *_Nonnull app) : app_(app) {
  app_->userData = this;
  app_->onAppCmd = HandleCmd;

  android_app_set_key_event_filter(app_, KeyEventFilter);
  android_app_set_motion_event_filter(app_, MotionEventFilter);
}

Engine::~Engine() {}

void Engine::Run() {
  while (!app_->destroyRequested) {
    android_poll_source *poll_source = nullptr;
    auto result = ALooper_pollOnce(-1, nullptr, nullptr,
                                   reinterpret_cast<void **>(&poll_source));
    CHECK_NE(result, ALOOPER_POLL_ERROR);

    if (poll_source != nullptr) {
      poll_source->process(app_, poll_source);
    }

    // TODO: Use AInputQueue.
    // With the old native_app_glue, this was onInputEvent. I'm not sure why it
    // isn't any more.
    HandleInputEvents(app_);
  }
}

void Engine::Pause() {
  running_ = false;
}

void Engine::Resume() {
  // Checked to make sure we don't double schedule Choreographer.
  if (!running_) {
    running_ = true;
    ScheduleNextTick();
  }
}

void Engine::ScheduleNextTick() {
  AChoreographer_postFrameCallback(AChoreographer_getInstance(), TickCallback,
                                   this);
}

void Engine::Tick() {
  ScheduleNextTick();
}

void Engine::TickCallback(long, void * _Nonnull data) {
  CHECK_NE(data, nullptr);
  auto engine = reinterpret_cast<Engine *>(data);
  engine->Tick();
}

}  // namespace ndksamples::graphics
