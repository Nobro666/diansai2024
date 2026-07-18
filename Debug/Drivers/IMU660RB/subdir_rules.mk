################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Drivers/IMU660RB/%.o: ../Drivers/IMU660RB/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/21943/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Motor" -I"C:/Users/21943/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Driver" -I"C:/Users/21943/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Drivers/MSPM0" -I"C:/Users/21943/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang" -I"C:/Users/21943/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Debug" -I"D:/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"Drivers/IMU660RB/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


