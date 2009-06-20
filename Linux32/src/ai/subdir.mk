################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ai/baby.cpp \
../src/ai/balljack.cpp \
../src/ai/bear.cpp \
../src/ai/butler.cpp \
../src/ai/door.cpp \
../src/ai/earth.cpp \
../src/ai/fireball.cpp \
../src/ai/foob.cpp \
../src/ai/garg.cpp \
../src/ai/icebit.cpp \
../src/ai/icechunk.cpp \
../src/ai/meep.cpp \
../src/ai/mother.cpp \
../src/ai/nessie.cpp \
../src/ai/ninja.cpp \
../src/ai/platform.cpp \
../src/ai/platvert.cpp \
../src/ai/ray.cpp \
../src/ai/rope.cpp \
../src/ai/se.cpp \
../src/ai/sndwave.cpp \
../src/ai/tank.cpp \
../src/ai/tankep2.cpp \
../src/ai/teleport.cpp \
../src/ai/vort.cpp \
../src/ai/walker.cpp \
../src/ai/yorp.cpp 

OBJS += \
./src/ai/baby.o \
./src/ai/balljack.o \
./src/ai/bear.o \
./src/ai/butler.o \
./src/ai/door.o \
./src/ai/earth.o \
./src/ai/fireball.o \
./src/ai/foob.o \
./src/ai/garg.o \
./src/ai/icebit.o \
./src/ai/icechunk.o \
./src/ai/meep.o \
./src/ai/mother.o \
./src/ai/nessie.o \
./src/ai/ninja.o \
./src/ai/platform.o \
./src/ai/platvert.o \
./src/ai/ray.o \
./src/ai/rope.o \
./src/ai/se.o \
./src/ai/sndwave.o \
./src/ai/tank.o \
./src/ai/tankep2.o \
./src/ai/teleport.o \
./src/ai/vort.o \
./src/ai/walker.o \
./src/ai/yorp.o 

CPP_DEPS += \
./src/ai/baby.d \
./src/ai/balljack.d \
./src/ai/bear.d \
./src/ai/butler.d \
./src/ai/door.d \
./src/ai/earth.d \
./src/ai/fireball.d \
./src/ai/foob.d \
./src/ai/garg.d \
./src/ai/icebit.d \
./src/ai/icechunk.d \
./src/ai/meep.d \
./src/ai/mother.d \
./src/ai/nessie.d \
./src/ai/ninja.d \
./src/ai/platform.d \
./src/ai/platvert.d \
./src/ai/ray.d \
./src/ai/rope.d \
./src/ai/se.d \
./src/ai/sndwave.d \
./src/ai/tank.d \
./src/ai/tankep2.d \
./src/ai/teleport.d \
./src/ai/vort.d \
./src/ai/walker.d \
./src/ai/yorp.d 


# Each subdirectory must supply rules for building sources it contributes
src/ai/%.o: ../src/ai/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DUSE_OPENGL -D__i386__ -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O3 -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


