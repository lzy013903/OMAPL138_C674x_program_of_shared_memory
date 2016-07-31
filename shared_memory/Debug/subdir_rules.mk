################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
fft_lzy.obj: ../fft_lzy.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=eabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="F:/ccs/shared_memory" --include_path="D:/ti/OMAPL138_StarterWare_1_10_04_01" --define=omapl138 --preproc_with_compile --preproc_dependency="fft_lzy.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=eabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="F:/ccs/shared_memory" --include_path="D:/ti/OMAPL138_StarterWare_1_10_04_01" --define=omapl138 --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

similar.obj: ../similar.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=eabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="F:/ccs/shared_memory" --include_path="D:/ti/OMAPL138_StarterWare_1_10_04_01" --define=omapl138 --preproc_with_compile --preproc_dependency="similar.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


