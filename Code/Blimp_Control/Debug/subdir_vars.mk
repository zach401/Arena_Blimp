################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../lnk_msp430g2553.cmd 

CFG_SRCS += \
../main.cfg 

C_SRCS += \
../cc11xL_spi.c \
../hal_board.c \
../hal_digio2.c \
../hal_int.c \
../hal_int_rf_msp_exp430g2.c \
../hal_msp_exp430g2_spi.c \
../main.c 

OBJS += \
./cc11xL_spi.obj \
./hal_board.obj \
./hal_digio2.obj \
./hal_int.obj \
./hal_int_rf_msp_exp430g2.obj \
./hal_msp_exp430g2_spi.obj \
./main.obj 

GEN_SRCS += \
./configPkg/compiler.opt \
./configPkg/linker.cmd 

C_DEPS += \
./cc11xL_spi.pp \
./hal_board.pp \
./hal_digio2.pp \
./hal_int.pp \
./hal_int_rf_msp_exp430g2.pp \
./hal_msp_exp430g2_spi.pp \
./main.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_SRCS__QUOTED += \
"configPkg\compiler.opt" \
"configPkg\linker.cmd" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"cc11xL_spi.pp" \
"hal_board.pp" \
"hal_digio2.pp" \
"hal_int.pp" \
"hal_int_rf_msp_exp430g2.pp" \
"hal_msp_exp430g2_spi.pp" \
"main.pp" 

OBJS__QUOTED += \
"cc11xL_spi.obj" \
"hal_board.obj" \
"hal_digio2.obj" \
"hal_int.obj" \
"hal_int_rf_msp_exp430g2.obj" \
"hal_msp_exp430g2_spi.obj" \
"main.obj" 

C_SRCS__QUOTED += \
"../cc11xL_spi.c" \
"../hal_board.c" \
"../hal_digio2.c" \
"../hal_int.c" \
"../hal_int_rf_msp_exp430g2.c" \
"../hal_msp_exp430g2_spi.c" \
"../main.c" 

CFG_SRCS__QUOTED += \
"../main.cfg" 

GEN_OPTS__FLAG += \
--cmd_file="./configPkg/compiler.opt" 

GEN_CMDS__FLAG += \
-l"./configPkg/linker.cmd" 


