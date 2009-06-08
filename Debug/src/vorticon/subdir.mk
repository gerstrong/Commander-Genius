################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/vorticon/CDialog.cpp \
../src/vorticon/CHighScores.cpp \
../src/vorticon/CObject.cpp \
../src/vorticon/CPlayer.cpp 

OBJS += \
./src/vorticon/CDialog.o \
./src/vorticon/CHighScores.o \
./src/vorticon/CObject.o \
./src/vorticon/CPlayer.o 

CPP_DEPS += \
./src/vorticon/CDialog.d \
./src/vorticon/CHighScores.d \
./src/vorticon/CObject.d \
./src/vorticon/CPlayer.d 


# Each subdirectory must supply rules for building sources it contributes
src/vorticon/%.o: ../src/vorticon/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DUSE_OPENGL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


