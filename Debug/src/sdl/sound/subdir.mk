################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sdl/sound/CSound.cpp \
../src/sdl/sound/CSoundChannel.cpp \
../src/sdl/sound/CSoundSlot.cpp 

OBJS += \
./src/sdl/sound/CSound.o \
./src/sdl/sound/CSoundChannel.o \
./src/sdl/sound/CSoundSlot.o 

CPP_DEPS += \
./src/sdl/sound/CSound.d \
./src/sdl/sound/CSoundChannel.d \
./src/sdl/sound/CSoundSlot.d 


# Each subdirectory must supply rules for building sources it contributes
src/sdl/sound/%.o: ../src/sdl/sound/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DUSE_OPENGL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


