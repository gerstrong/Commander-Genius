LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2
SDL_IMAGE_PATH := ../SDL2_image
SDL_MIXER_PATH := ../SDL2_mixer
BOOST_PATH := ../boost
CURL_PATH := ../curl

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_IMAGE_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SDL_MIXER_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BOOST_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(CURL_PATH)/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/cgSrc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/GsKit


# Application sources
# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/GsKit, *.cpp)
LOCAL_SRC_FILES += $(call rwildcard, $(LOCAL_PATH)/cgSrc, *.cpp)
LOCAL_SRC_FILES += $(call rwildcard, $(LOCAL_PATH)/GsKit, *.c)
LOCAL_SRC_FILES += $(call rwildcard, $(LOCAL_PATH)/cgSrc, *.c)

# When debugging variables is required!
#$(info $$LOCAL_C_INCLUDES is [${LOCAL_C_INCLUDES}])


LOCAL_SHARED_LIBRARIES := SDL2_image
LOCAL_SHARED_LIBRARIES += SDL2_mixer
LOCAL_SHARED_LIBRARIES += SDL2
LOCAL_SHARED_LIBRARIES += curl

LOCAL_CFLAGS += -DREFKEEN_VER_KDREAMS -DREFKEEN_VER_KDREAMS_ANYEGA_ALL -DDOWNLOADER -DUSE_FILE32API

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -lz

include $(BUILD_SHARED_LIBRARY)
