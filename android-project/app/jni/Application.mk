
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
APP_STL := gnustl_static

APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

# Min runtime API level
APP_PLATFORM=android-14

# Enable c++14 extentions in source code
APP_CPPFLAGS += -std=c++14
APP_CPPFLAGS += -frtti
APP_CPPFLAGS += -fexceptions