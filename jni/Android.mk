# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_C_INCLUDES  $(ANDROID_SOURCE_TINY4412)/
LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) external/skia/include/images/ \
					external/skia/include/core/

#C_INCLUDE_PATH += $(LOCAL_C_INCLUDES_TEST)
#CPLUS_INCLUDE_PATH += $(LOCAL_C_INCLUDES_TEST)

$(warning $(LOCAL_C_INCLUDES))
$(warning $(JNI_H_INCLUDE))
$(warning $(path-to-system-libs))

LOCAL_MODULE    := ImageProc
LOCAL_SRC_FILES := ImageProc.cpp
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/lib/ -L $(path-to-system-libs)
#LOCAL_LDLIBS  += -lcutils
#LOCAL_LDLIBS    := -llog -ljnigraphics -lskiagl  libskiagl
LOCAL_SHARED_LIBRARIES += libcutils liblog libjnigraphics libskia

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)