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

#pragma once

#include "game-activity/native_app_glue/android_native_app_glue.h"

namespace ndksamples::graphics {

class Engine {
 public:
  Engine(android_app* _Nonnull app);
  Engine(const Engine&) = delete;
  virtual ~Engine();

  Engine& operator=(const Engine&) = delete;

  void Run();

  /**
   * Pauses ticking the application.
   *
   * When paused, sensor and input events will still be processed, but the
   * update and render parts of the loop will not run.
   */
  void Pause();

  /**
   * Resumes ticking the application.
   */
  void Resume();

  /**
   * Entry point for Choreographer.
   *
   * The first argument (the frame time) is not used as it is not needed for
   * this sample. If you copy from this sample and make use of that argument,
   * note that there's an API bug: that time is a signed 32-bit nanosecond
   * counter on 32-bit systems, so it will roll over every ~2 seconds. If your
   * minSdkVersion is 29 or higher, use AChoreographer_postFrameCallback64
   * instead, which is 64-bits for all architectures. Otherwise, bitwise-and
   * the value with the upper bits from CLOCK_MONOTONIC.
   *
   * @param data The Engine being ticked.
   */
  static void TickCallback(long, void* _Nonnull data);

 private:
  android_app* _Nonnull app_;
  
  /// False if the engine is currently paused.
  bool running_ = false;

  void ScheduleNextTick();
  void Tick();
};

}  // namespace ndksamples::graphics
