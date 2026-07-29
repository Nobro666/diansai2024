################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
User/balance/%.o: ../User/balance/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/21943/workspace_ccstheia/diansai2024/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Driver" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MPU6050" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MSPM0" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/OLED_Hardware_I2C" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/PID" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/trace" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/Task" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/Distance" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/BUZZER" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/balance" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User" -I"C:/Users/21943/workspace_ccstheia/diansai2024" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Debug" -I"D:/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_10_00_04/source" -DMOTION_DRIVER_TARGET_MSPM0 -DMPU6050 -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"User/balance/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


