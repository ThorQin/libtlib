################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/lex/dfa.cpp \
../src/lex/exp.cpp \
../src/lex/lexbc.cpp \
../src/lex/lexical.cpp \
../src/lex/range.cpp \
../src/lex/scan.cpp 

OBJS += \
./src/lex/dfa.o \
./src/lex/exp.o \
./src/lex/lexbc.o \
./src/lex/lexical.o \
./src/lex/range.o \
./src/lex/scan.o 

CPP_DEPS += \
./src/lex/dfa.d \
./src/lex/exp.d \
./src/lex/lexbc.d \
./src/lex/lexical.d \
./src/lex/range.d \
./src/lex/scan.d 


# Each subdirectory must supply rules for building sources it contributes
src/lex/%.o: ../src/lex/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 `pkg-config --cflags glib-2.0` -pthread -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


