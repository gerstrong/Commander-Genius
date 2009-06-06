################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sdlold/control.c \
../src/sdlold/joydrv.c \
../src/sdlold/polling.c \
../src/sdlold/snddrv.c \
../src/sdlold/timedrv.c \
../src/sdlold/viddrv.c 

OBJS += \
./src/sdlold/control.o \
./src/sdlold/joydrv.o \
./src/sdlold/polling.o \
./src/sdlold/snddrv.o \
./src/sdlold/timedrv.o \
./src/sdlold/viddrv.o 

C_DEPS += \
./src/sdlold/control.d \
./src/sdlold/joydrv.d \
./src/sdlold/polling.d \
./src/sdlold/snddrv.d \
./src/sdlold/timedrv.d \
./src/sdlold/viddrv.d 


# Each subdirectory must supply rules for building sources it contributes
src/sdlold/%.o: ../src/sdlold/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DBUILD_SDL -DBUILD_WITH_OGG -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


