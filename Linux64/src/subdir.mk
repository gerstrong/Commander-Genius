################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CCGenius.cpp \
../src/CGame.cpp \
../src/CGraphics.cpp \
../src/CLatch.cpp \
../src/CLogFile.cpp \
../src/eseq_ep1.cpp \
../src/eseq_ep2.cpp \
../src/eseq_ep3.cpp \
../src/fileio.cpp \
../src/finale.cpp \
../src/game.cpp \
../src/gamedo.cpp \
../src/gamepdo.cpp \
../src/gm_pdowm.cpp \
../src/lz.cpp \
../src/main.cpp \
../src/map.cpp \
../src/menu.cpp \
../src/misc.cpp \
../src/sgrle.cpp 

OBJS += \
./src/CCGenius.o \
./src/CGame.o \
./src/CGraphics.o \
./src/CLatch.o \
./src/CLogFile.o \
./src/eseq_ep1.o \
./src/eseq_ep2.o \
./src/eseq_ep3.o \
./src/fileio.o \
./src/finale.o \
./src/game.o \
./src/gamedo.o \
./src/gamepdo.o \
./src/gm_pdowm.o \
./src/lz.o \
./src/main.o \
./src/map.o \
./src/menu.o \
./src/misc.o \
./src/sgrle.o 

CPP_DEPS += \
./src/CCGenius.d \
./src/CGame.d \
./src/CGraphics.d \
./src/CLatch.d \
./src/CLogFile.d \
./src/eseq_ep1.d \
./src/eseq_ep2.d \
./src/eseq_ep3.d \
./src/fileio.d \
./src/finale.d \
./src/game.d \
./src/gamedo.d \
./src/gamepdo.d \
./src/gm_pdowm.d \
./src/lz.d \
./src/main.d \
./src/map.d \
./src/menu.d \
./src/misc.d \
./src/sgrle.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


