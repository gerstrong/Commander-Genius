################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/fileio/CParser.cpp \
../src/fileio/lzexe.cpp \
../src/fileio/rle.cpp \
../src/fileio/story.cpp 

OBJS += \
./src/fileio/CParser.o \
./src/fileio/lzexe.o \
./src/fileio/rle.o \
./src/fileio/story.o 

CPP_DEPS += \
./src/fileio/CParser.d \
./src/fileio/lzexe.d \
./src/fileio/rle.d \
./src/fileio/story.d 


# Each subdirectory must supply rules for building sources it contributes
src/fileio/%.o: ../src/fileio/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_SDL -DUSE_OPENGL -DBUILD_WITH_OGG -DTARGET_LNX -I/usr/include/SDL -I/usr/include/vorbis -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


