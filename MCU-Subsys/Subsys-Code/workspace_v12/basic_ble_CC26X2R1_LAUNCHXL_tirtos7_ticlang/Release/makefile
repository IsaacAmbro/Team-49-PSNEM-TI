################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS

GEN_OPTS__FLAG := @"syscfg/ti_build_config.opt" @"syscfg/ti_ble_app_config.opt" 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./syscfg/ti_ble_config.o" \
"./syscfg/ti_devices_config.o" \
"./syscfg/ti_radio_config.o" \
"./syscfg/ti_drivers_config.o" \
"./syscfg/ti_sysbios_config.o" \
"./app/app_broadcaster.o" \
"./app/app_central.o" \
"./app/app_data.o" \
"./app/app_main.o" \
"./app/app_observer.o" \
"./app/app_pairing.o" \
"./app/app_peripheral.o" \
"./common/BLEAppUtil/bleapputil_init.o" \
"./common/BLEAppUtil/bleapputil_process.o" \
"./common/BLEAppUtil/bleapputil_stack_callbacks.o" \
"./common/BLEAppUtil/bleapputil_task.o" \
"./common/Drivers/NV/crc.o" \
"./common/Drivers/NV/nvocmp.o" \
"./common/Drivers/UartLog.o" \
"./common/Startup/ble_user_config_stack.o" \
"./common/Startup/main.o" \
"./common/Startup/osal_icall_ble.o" \
"./common/Startup/rom_init.o" \
"./common/dpl/DebugP_tirtos.o" \
"./common/dpl/EventP_tirtos.o" \
"./common/dpl/QueueP_tirtos.o" \
"./common/dpl/TaskP_tirtos.o" \
"./common/iCall/icall.o" \
"./common/iCall/icall_cc2650.o" \
"./common/iCall/icall_user_config.o" \
"./common/iCallBLE/ble_stack_api.o" \
"./common/iCallBLE/ble_user_config.o" \
"./common/iCallBLE/icall_api_lite.o" \
"../cc13x2_cc26x2_app_tirtos7.cmd" \
$(GEN_CMDS__FLAG) \
-Wl,-lti_utils_build_linker.cmd.genlibs \
-Wl,-l"ti/devices/cc13x2_cc26x2/driverlib/bin/ticlang/driverlib.lib" \
-Wl,-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include app/subdir_vars.mk
-include common/BLEAppUtil/subdir_vars.mk
-include common/Drivers/NV/subdir_vars.mk
-include common/Drivers/subdir_vars.mk
-include common/Startup/subdir_vars.mk
-include common/dpl/subdir_vars.mk
-include common/iCall/subdir_vars.mk
-include common/iCallBLE/subdir_vars.mk
-include subdir_rules.mk
-include app/subdir_rules.mk
-include common/BLEAppUtil/subdir_rules.mk
-include common/Drivers/NV/subdir_rules.mk
-include common/Drivers/subdir_rules.mk
-include common/Startup/subdir_rules.mk
-include common/dpl/subdir_rules.mk
-include common/iCall/subdir_rules.mk
-include common/iCallBLE/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.out \

EXE_OUTPUTS__QUOTED += \
"basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.out" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.out"

# Tool invocations
basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang.exe" @"C:/Users/isaac/workspace_v12/basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Release/syscfg/ti_ble_app_config.opt" @"C:/Users/isaac/workspace_v12/basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Release/syscfg/ti_build_config.opt" @"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_41_00_17/source/ti/ble5stack/config/build_components.opt" @"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_41_00_17/source/ti/ble5stack/config/factory_config.opt"  -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -DICALL_NO_APP_EVENTS -DUARTLOG_ENABLE -Duartlog_FILE=0 -DDeviceFamily_CC26X2 -DFLASH_ROM_BUILD -DNVOCMP_NWSAMEITEM=1 -DHEAPMGR_CONFIG=0x80 -DHEAPMGR_SIZE=0x0 -gdwarf-3 -march=armv7e-m -Wl,-m"basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.map" -Wl,-i"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_41_00_17/source" -Wl,-i"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_41_00_17/kernel/tirtos7/packages" -Wl,-i"C:/Users/isaac/workspace_v12/basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Release/syscfg" -Wl,-i"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/lib" -Wl,--reread_libs -Wl,--define=FLASH_ROM_BUILD=2 -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang_linkInfo.xml" -Wl,--rom_model -o "basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "syscfg\ti_ble_config.o" "syscfg\ti_devices_config.o" "syscfg\ti_radio_config.o" "syscfg\ti_drivers_config.o" "syscfg\ti_sysbios_config.o" "app\app_broadcaster.o" "app\app_central.o" "app\app_data.o" "app\app_main.o" "app\app_observer.o" "app\app_pairing.o" "app\app_peripheral.o" "common\BLEAppUtil\bleapputil_init.o" "common\BLEAppUtil\bleapputil_process.o" "common\BLEAppUtil\bleapputil_stack_callbacks.o" "common\BLEAppUtil\bleapputil_task.o" "common\Drivers\NV\crc.o" "common\Drivers\NV\nvocmp.o" "common\Drivers\UartLog.o" "common\Startup\ble_user_config_stack.o" "common\Startup\main.o" "common\Startup\osal_icall_ble.o" "common\Startup\rom_init.o" "common\dpl\DebugP_tirtos.o" "common\dpl\EventP_tirtos.o" "common\dpl\QueueP_tirtos.o" "common\dpl\TaskP_tirtos.o" "common\iCall\icall.o" "common\iCall\icall_cc2650.o" "common\iCall\icall_user_config.o" "common\iCallBLE\ble_stack_api.o" "common\iCallBLE\ble_user_config.o" "common\iCallBLE\icall_api_lite.o" 
	-$(RM) "syscfg\ti_ble_config.d" "syscfg\ti_devices_config.d" "syscfg\ti_radio_config.d" "syscfg\ti_drivers_config.d" "syscfg\ti_sysbios_config.d" "app\app_broadcaster.d" "app\app_central.d" "app\app_data.d" "app\app_main.d" "app\app_observer.d" "app\app_pairing.d" "app\app_peripheral.d" "common\BLEAppUtil\bleapputil_init.d" "common\BLEAppUtil\bleapputil_process.d" "common\BLEAppUtil\bleapputil_stack_callbacks.d" "common\BLEAppUtil\bleapputil_task.d" "common\Drivers\NV\crc.d" "common\Drivers\NV\nvocmp.d" "common\Drivers\UartLog.d" "common\Startup\ble_user_config_stack.d" "common\Startup\main.d" "common\Startup\osal_icall_ble.d" "common\Startup\rom_init.d" "common\dpl\DebugP_tirtos.d" "common\dpl\EventP_tirtos.d" "common\dpl\QueueP_tirtos.d" "common\dpl\TaskP_tirtos.d" "common\iCall\icall.d" "common\iCall\icall_cc2650.d" "common\iCall\icall_user_config.d" "common\iCallBLE\ble_stack_api.d" "common\iCallBLE\ble_user_config.d" "common\iCallBLE\icall_api_lite.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmhex.exe -order MS --memwidth=8 --romwidth=8 --intel -o basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.hex basic_ble_CC26X2R1_LAUNCHXL_tirtos7_ticlang.out
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

