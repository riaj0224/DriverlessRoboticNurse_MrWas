################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/main.c" \
"../Sources/sa_mtb.c" \
"../Sources/system_IRQ.c" \
"../Sources/system_functions.c" \
"../Sources/system_init.c" \

C_SRCS += \
../Sources/main.c \
../Sources/sa_mtb.c \
../Sources/system_IRQ.c \
../Sources/system_functions.c \
../Sources/system_init.c \

OBJS += \
./Sources/main.o \
./Sources/sa_mtb.o \
./Sources/system_IRQ.o \
./Sources/system_functions.o \
./Sources/system_init.o \

C_DEPS += \
./Sources/main.d \
./Sources/sa_mtb.d \
./Sources/system_IRQ.d \
./Sources/system_functions.d \
./Sources/system_init.d \

OBJS_QUOTED += \
"./Sources/main.o" \
"./Sources/sa_mtb.o" \
"./Sources/system_IRQ.o" \
"./Sources/system_functions.o" \
"./Sources/system_init.o" \

C_DEPS_QUOTED += \
"./Sources/main.d" \
"./Sources/sa_mtb.d" \
"./Sources/system_IRQ.d" \
"./Sources/system_functions.d" \
"./Sources/system_init.d" \

OBJS_OS_FORMAT += \
./Sources/main.o \
./Sources/sa_mtb.o \
./Sources/system_IRQ.o \
./Sources/system_functions.o \
./Sources/system_init.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/main.o: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/main.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/sa_mtb.o: ../Sources/sa_mtb.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/sa_mtb.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/sa_mtb.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/system_IRQ.o: ../Sources/system_IRQ.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/system_IRQ.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/system_IRQ.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/system_functions.o: ../Sources/system_functions.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/system_functions.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/system_functions.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/system_init.o: ../Sources/system_init.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/system_init.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/system_init.o"
	@echo 'Finished building: $<'
	@echo ' '


