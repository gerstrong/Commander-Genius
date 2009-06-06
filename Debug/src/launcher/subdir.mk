################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/launcher/GameSlot.cpp \
../src/launcher/GamesMenu.cpp 

OBJS += \
./src/launcher/GameSlot.o \
./src/launcher/GamesMenu.o 

CPP_DEPS += \
./src/launcher/GameSlot.d \
./src/launcher/GamesMenu.d 


# Each subdirectory must supply rules for building sources it contributes
src/launcher/%.o: ../src/launcher/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DDEBUG -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


