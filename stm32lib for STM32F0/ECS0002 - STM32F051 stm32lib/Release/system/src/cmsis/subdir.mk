################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32f0xx.c \
../system/src/cmsis/vectors_stm32f0xx.c 

OBJS += \
./system/src/cmsis/system_stm32f0xx.o \
./system/src/cmsis/vectors_stm32f0xx.o 

C_DEPS += \
./system/src/cmsis/system_stm32f0xx.d \
./system/src/cmsis/vectors_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F051 -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"/Users/Robbe/Documents/PROJECTS/ECLIPSE STM32 RVA_STM32Lib/ECR0002 - RVA_STM32Lib REV B/RVA_STM32Lib_Final/RVA_STM32Lib_include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f0-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

