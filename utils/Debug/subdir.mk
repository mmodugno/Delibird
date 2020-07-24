################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../datos_broker.c \
../datos_gamecard.c \
../datos_team.c \
../utils_en_comun.c 

OBJS += \
./datos_broker.o \
./datos_gamecard.o \
./datos_team.o \
./utils_en_comun.o 

C_DEPS += \
./datos_broker.d \
./datos_gamecard.d \
./datos_team.d \
./utils_en_comun.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


