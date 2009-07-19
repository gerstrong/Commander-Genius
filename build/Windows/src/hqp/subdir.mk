################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/hqp/CHQBitmap.cpp \
../src/hqp/CMusic.cpp \
../src/hqp/hq_sound.cpp 

OBJS += \
./src/hqp/CHQBitmap.o \
./src/hqp/CMusic.o \
./src/hqp/hq_sound.o 

CPP_DEPS += \
./src/hqp/CHQBitmap.d \
./src/hqp/CMusic.d \
./src/hqp/hq_sound.d 


# Each subdirectory must supply rules for building sources it contributes
src/hqp/%.o: ../src/hqp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	i586-mingw32msvc-g++ -DBUILD_SDL -DUSE_OPENGL -DBUILD_WITH_OGG -DTARGET_WIN32 -I/usr/i586-mingw32msvc/include/SDL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


