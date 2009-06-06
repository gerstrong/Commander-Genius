################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scale2x/getopt.cpp \
../src/scale2x/pixel.cpp \
../src/scale2x/scale2x.cpp \
../src/scale2x/scale3x.cpp \
../src/scale2x/scalebit.cpp 

OBJS += \
./src/scale2x/getopt.o \
./src/scale2x/pixel.o \
./src/scale2x/scale2x.o \
./src/scale2x/scale3x.o \
./src/scale2x/scalebit.o 

CPP_DEPS += \
./src/scale2x/getopt.d \
./src/scale2x/pixel.d \
./src/scale2x/scale2x.d \
./src/scale2x/scale3x.d \
./src/scale2x/scalebit.d 


# Each subdirectory must supply rules for building sources it contributes
src/scale2x/%.o: ../src/scale2x/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


