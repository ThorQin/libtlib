################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/binary.cpp \
../src/charset.cpp \
../src/encode.cpp \
../src/md5.cpp \
../src/os.cpp \
../src/strfunc.cpp \
../src/timefunc.cpp 

OBJS += \
./src/binary.o \
./src/charset.o \
./src/encode.o \
./src/md5.o \
./src/os.o \
./src/strfunc.o \
./src/timefunc.o 

CPP_DEPS += \
./src/binary.d \
./src/charset.d \
./src/encode.d \
./src/md5.d \
./src/os.d \
./src/strfunc.d \
./src/timefunc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 `pkg-config --cflags glib-2.0` -pthread -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


