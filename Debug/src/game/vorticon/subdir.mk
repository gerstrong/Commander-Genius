################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/game/vorticon/CDemo.cpp \
../src/game/vorticon/CIntro.cpp \
../src/game/vorticon/CTitle.cpp 

OBJS += \
./src/game/vorticon/CDemo.o \
./src/game/vorticon/CIntro.o \
./src/game/vorticon/CTitle.o 

CPP_DEPS += \
./src/game/vorticon/CDemo.d \
./src/game/vorticon/CIntro.d \
./src/game/vorticon/CTitle.d 


# Each subdirectory must supply rules for building sources it contributes
src/game/vorticon/%.o: ../src/game/vorticon/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DDEBUG -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


