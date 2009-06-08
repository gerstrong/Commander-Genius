################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sdl/CInput.cpp \
../src/sdl/COpenGL.cpp \
../src/sdl/CTimer.cpp \
../src/sdl/CVideoDriver.cpp \
../src/sdl/joydrv.cpp 

OBJS += \
./src/sdl/CInput.o \
./src/sdl/COpenGL.o \
./src/sdl/CTimer.o \
./src/sdl/CVideoDriver.o \
./src/sdl/joydrv.o 

CPP_DEPS += \
./src/sdl/CInput.d \
./src/sdl/COpenGL.d \
./src/sdl/CTimer.d \
./src/sdl/CVideoDriver.d \
./src/sdl/joydrv.d 


# Each subdirectory must supply rules for building sources it contributes
src/sdl/%.o: ../src/sdl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DUSE_OPENGL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


