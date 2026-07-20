################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/21943/workspace_ccstheia/diansai2024/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Driver" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MPU6050" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MSPM0" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/OLED_Hardware_I2C" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/PID" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/trace" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User" -I"C:/Users/21943/workspace_ccstheia/diansai2024" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Debug" -I"D:/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_10_00_04/source" -DMOTION_DRIVER_TARGET_MSPM0 -DMPU6050 -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-438123170: ../main.syscfg
	@echo 'SysConfig - building file: "$<"'
	"D:/ti/ccs2051/ccs/utils/sysconfig_1.27.1/sysconfig_cli.bat" -s "D:/ti/mspm0_sdk_2_10_00_04/.metadata/product.json" --script "C:/Users/21943/workspace_ccstheia/diansai2024/main.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-438123170 ../main.syscfg
device.opt: build-438123170
device.cmd.genlibs: build-438123170
ti_msp_dl_config.c: build-438123170
ti_msp_dl_config.h: build-438123170
Event.dot: build-438123170

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/21943/workspace_ccstheia/diansai2024/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Driver" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MPU6050" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MSPM0" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/OLED_Hardware_I2C" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/PID" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/trace" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User" -I"C:/Users/21943/workspace_ccstheia/diansai2024" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Debug" -I"D:/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_10_00_04/source" -DMOTION_DRIVER_TARGET_MSPM0 -DMPU6050 -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: D:/ti/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/21943/workspace_ccstheia/diansai2024/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Driver" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MPU6050" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/Motor" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/MSPM0" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Drivers/OLED_Hardware_I2C" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/PID" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User/trace" -I"C:/Users/21943/workspace_ccstheia/diansai2024/User" -I"C:/Users/21943/workspace_ccstheia/diansai2024" -I"C:/Users/21943/workspace_ccstheia/diansai2024/Debug" -I"D:/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_10_00_04/source" -DMOTION_DRIVER_TARGET_MSPM0 -DMPU6050 -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


