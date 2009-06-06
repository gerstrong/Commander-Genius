################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/SDL/Input.cpp \
../src/SDL/Timer.cpp \
../src/SDL/Video.cpp 

OBJS += \
./src/SDL/Input.o \
./src/SDL/Timer.o \
./src/SDL/Video.o 

CPP_DEPS += \
./src/SDL/Input.d \
./src/SDL/Timer.d \
./src/SDL/Video.d 


# Each subdirectory must supply rules for building sources it contributes
src/SDL/%.o: ../src/SDL/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DDEBUG -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


