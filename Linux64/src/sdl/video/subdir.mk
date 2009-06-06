################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sdl/video/colourconvert.cpp 

OBJS += \
./src/sdl/video/colourconvert.o 

CPP_DEPS += \
./src/sdl/video/colourconvert.d 


# Each subdirectory must supply rules for building sources it contributes
src/sdl/video/%.o: ../src/sdl/video/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


