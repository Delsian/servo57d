
cmake_minimum_required(VERSION 3.21)

# Enable export of compilation database for further analysis
# This creates a compile_comands.json file in the build directory that
# can be used by visual studio and cppcheck for code completion and
# build analysis.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the path to custom modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# This is the toolchain file, we use the arm-none-eabi toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/arm-gcc-toolchain.cmake)

find_package(Git)
add_custom_target(version
  ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/fw_version.h.in
                   -D DST=${CMAKE_BINARY_DIR}/fw_version.h
                   -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                   -P ${CMAKE_MODULE_PATH}/GenerateVersionHeader.cmake
  )

#---------------------------------------------------------------------------------------
# Set debug/release build configuration Options
#---------------------------------------------------------------------------------------

# Options for DEBUG build
# -Og   Enables optimizations that do not interfere with debugging.
# -g    Produce debugging information in the operating system’s native format.
#set(CMAKE_C_FLAGS_DEBUG "-Og -g" CACHE INTERNAL "C Compiler options for debug build type")
#set(CMAKE_CXX_FLAGS_DEBUG "-Og -g" CACHE INTERNAL "C++ Compiler options for debug build type")
#set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "ASM Compiler options for debug build type")
#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Linker options for debug build type")

# Options for RELEASE build
# -Os   Optimize for size. -Os enables all -O2 optimizations.
# -flto Runs the standard link-time optimizer.
#set(CMAKE_C_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "C Compiler options for release build type")
#set(CMAKE_CXX_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "C++ Compiler options for release build type")
#set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM Compiler options for release build type")
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto" CACHE INTERNAL "Linker options for release build type")

#---------------------------------------------------------------------------------------
# Set compiler/linker flags
#---------------------------------------------------------------------------------------

# Object build options
# -O0                   No optimizations, reduce compilation time and make debugging produce the expected results.
# -mthumb               Generat thumb instructions.
# -fno-builtin          Do not use built-in functions provided by GCC.
# -Wall                 Print only standard warnings, for all use Wextra
# -ffunction-sections   Place each function item into its own section in the output file.
# -fdata-sections       Place each data item into its own section in the output file.
# -fomit-frame-pointer  Omit the frame pointer in functions that don’t need one.
# -mabi=aapcs           Defines enums to be a variable sized type.
set(OBJECT_GEN_FLAGS "-Og -g -mcpu=cortex-m4 -mthumb -fno-builtin -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs")

set(CMAKE_C_FLAGS   "${OBJECT_GEN_FLAGS} -std=gnu99 " CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} -std=c++11 " CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp " CACHE INTERNAL "ASM Compiler options")


project(servo57 LANGUAGES C ASM VERSION 1.0.0)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -DVERSION=\"${CMAKE_PROJECT_VERSION}\"")
set(SDK_DIR                        ${PROJECT_SOURCE_DIR}/SDK CACHE PATH "N32 SDK directory")
set(KLIPPER_DIR                    ${PROJECT_SOURCE_DIR}/../klipper/src CACHE PATH "Klipper directory")

  # -Wl,--gc-sections     Perform the dead code elimination.
# --specs=nano.specs    Link with newlib-nano.
# --specs=nosys.specs   No syscalls, provide empty implementations for the POSIX system calls.
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -lgcc -lc_nano -mthumb -mabi=aapcs" CACHE INTERNAL "Linker options")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-gc-sections,--print-memory-usage,-Map=${CMAKE_PROJECT_NAME}.map")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${SDK_DIR}/CMSIS/device/n32l40x_flash.ld")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/RTT)
include_directories(${SDK_DIR}/CMSIS/core)
include_directories(${SDK_DIR}/CMSIS/device)
include_directories(${SDK_DIR}/n32l40x_std_periph_driver/inc)
include_directories(${KLIPPER_DIR})
include_directories(${KLIPPER_DIR}/../lib/fast-hash)
include_directories(${CMAKE_BINARY_DIR})

set(SDK_SOURCES
    ${SDK_DIR}/CMSIS/device/system_n32l40x.c
    ${SDK_DIR}/CMSIS/device/startup/startup_n32l40x_gcc.s
    ${SDK_DIR}/n32l40x_std_periph_driver/src/misc.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_rcc.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_pwr.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_exti.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_gpio.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_i2c.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_spi.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_adc.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_tim.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_dma.c
    ${SDK_DIR}/n32l40x_std_periph_driver/src/n32l40x_can.c
  )

set(RTT_SOURCES
  ${PROJECT_SOURCE_DIR}/RTT/SEGGER_RTT_ASM_ARMv7M.s
  ${PROJECT_SOURCE_DIR}/RTT/SEGGER_RTT_printf.c
  ${PROJECT_SOURCE_DIR}/RTT/SEGGER_RTT.c
)

set(KLIPPER_SOURCES
  ${KLIPPER_DIR}/sched.c
  ${KLIPPER_DIR}/generic/armcm_timer.c
  ${KLIPPER_DIR}/generic/armcm_irq.c
  ${KLIPPER_DIR}/generic/canserial.c
  ${KLIPPER_DIR}/../lib/fast-hash/fasthash.c
)

set(SOURCE_FILES
    ${SDK_SOURCES}
    ${RTT_SOURCES}
    ${KLIPPER_SOURCES}
    src/main.c
    src/compile_time_request.c
    src/dummy.c
    src/drivers/board.c
    src/drivers/adc.c
    src/drivers/button.c
    src/drivers/external.c
    src/drivers/mt6816.c
    src/drivers/oled.c
    src/drivers/led.c
    src/drivers/can.c
    src/drivers/stepper.c
	)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME}.elf version)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD  
COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
COMMENT "Building ${HEX_FILE}")
