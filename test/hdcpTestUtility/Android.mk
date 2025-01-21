#
# Copyright (c) 2009-2018, Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# For ALOGV and enter/exit log, set ENABLE_DEBUG=1 during compilation e.g. mm ENABLE_DEBUG=1 -j32
LOCAL_CPPFLAGS += \
    -DANDROID \
    -DANDROID_VERSION=800 \
    -DLOG_TAG=\"HDCPTESTUTILITY\"

# LOG_CONSOLE will print ALOGI, ALOGE, ALOGW in Android. Enable on debug build
ifeq ($(TARGET_BUILD_VARIANT),userdebug)
LOCAL_CPPFLAGS += -DLOG_CONSOLE
endif

# For ALOGV and function enter/exit log, set ENABLE_DEBUG=1 during compilation e.g. mm ENABLE_DEBUG=1 -j32
ifeq ($(ENABLE_DEBUG),1)
LOCAL_CPPFLAGS += \
    -DHDCP_USE_VERBOSE_LOGGING \
    -DHDCP_USE_FUNCTION_LOGGING \
    -DHDCP_USE_LINK_FUNCTION_LOGGING
endif

ifeq ($(ENABLE_DEBUG),1)
    LOCAL_CPPFLAG += \
        -DLOG_CONSOLE \
        -DHDCP_USE_VERBOSE_LOGGING \
        -DHDCP_USE_FUNCTION_LOGGING \
        -DHDCP_USE_LINK_FUNCTION_LOGGING
endif

#WA
LOCAL_CPPFLAGS += \
    -Wno-unused-parameter \
    -Wno-error

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../sdk \
    $(LOCAL_PATH)/../../test/hdcpTestUtility \

LOCAL_SRC_FILES := \
    hdcpTestUtility.cpp \

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libhdcpsdk \

LOCAL_EXPORT_C_INCLUDE_DIRS = \
    $(LOCAL_PATH)/

LOCAL_MODULE := hdcpTestUtility
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
