set(OPENCM3_DIR $ENV{OPENCM3_DIR})
if( NOT OPENCM3_DIR )
	message( FATAL_ERROR "Environment variable OPENCM3_DIR must be set\
 and point to the build/install directory of libopencm3")
endif()

if(${CMAKE_BOARD} STREQUAL stm32f4_disco)
	set(OPENCM3_TARGET "STM32F4_DISCO")
	set(OPENCM3_LIB "opencm3_stm32f4")
	set(BOARD_SUPPORT_FILE "stm32f4_discovery.cc")
	set(LINKERFILE stm32f4-discovery.ld)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTM32F4_DISCO" PARENT_SCOPE)
	set(CMAKE_FLASH "openocd")
	set(OPENOCD_OPTIONS -f interface/stlink-v2.cfg -f target/stm32f4x.cfg)
elseif(${CMAKE_BOARD} STREQUAL stm32f4_nucleo)
	set(OPENCM3_TARGET "STM32F4_NUCLEO")
	set(OPENCM3_LIB "opencm3_stm32f4")
	set(BOARD_SUPPORT_FILE "stm32f4_nucleo.cc")
	set(LINKERFILE stm32f4-discovery.ld)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTM32F4_NUCLEO" PARENT_SCOPE)
	set(CMAKE_FLASH "openocd")
	set(OPENOCD_OPTIONS -f interface/stlink-v2-1.cfg -f target/stm32f4x.cfg)
elseif(${CMAKE_BOARD} STREQUAL stm32f4_bluepill)
	set(OPENCM3_TARGET "STM32F4_BLUEPILL")
	set(OPENCM3_LIB "opencm3_stm32f4")
	set(BOARD_SUPPORT_FILE "stm32f4_bluepill.cc")
	set(LINKERFILE stm32f4-bluepill.ld)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTM32F4_BLUEPILL" PARENT_SCOPE)
	set(CMAKE_FLASH "dfu")
else()
	message( FATAL_ERROR "set CMAKE_BOARD in your toolchain")
endif()

include_directories(".")

#Custom target with insane memories. Just to make target link:
# GCC produces much nicer statistics after linking on  e.g. why
# it would not have linked with smaller memories
if( "${CMAKE_BUILD_TYPE}" STREQUAL "LinkerDebug")
	set(LINKERFILE debugtarget.ld)
endif()


#Build target with main in target/opencm3 directory
function(build_target target_name)
	add_executable( ${target_name}
		${target_name}.cc
		opencm3.cc
		${BOARD_SUPPORT_FILE}
	)
	target_link_libraries(${target_name} libtreefrog ${OPENCM3_LIB})
	common_build_target(${target_name})
endfunction()

#Build target with main in common src/ directory
function(build_link_target target_name)
	add_executable( ${target_name}
		opencm3.cc
		${BOARD_SUPPORT_FILE}
	)
	target_link_libraries(${target_name} libtreefrog ${OPENCM3_LIB} main_${target_name})
	common_build_target(${target_name})
endfunction()

function (common_build_target target_name)
	target_compile_options(${target_name}
		PRIVATE
			-D${OPENCM3_TARGET}
			-I/home/kraiskil/stuff/libopencm3/include
			-Wno-unused-function
		)
	target_link_options(${target_name}
		PRIVATE 
			-T${CMAKE_CURRENT_SOURCE_DIR}/${LINKERFILE}
			-L${CMAKE_CURRENT_SOURCE_DIR}
			-L${OPENCM3_DIR}/lib
			-nostartfiles
			-Wl,-Map=${target_name}.map
			-Wl,--print-memory-usage
		)
	if("${CMAKE_FLASH}" STREQUAL "openocd")
		add_custom_target(
			flash_${target_name}
			DEPENDS ${target_name}
			COMMAND openocd ${OPENOCD_OPTIONS} -c \"program ${target_name} verify reset exit\"
			)
	elseif("${CMAKE_FLASH}" STREQUAL "dfu")
		add_custom_target(
			flash_${target_name}
			DEPENDS ${target_name}
			COMMAND arm-none-eabi-objcopy -O binary ${target_name} ${target_name}.bin
			# convert to a DFU file and upload it. Press the correct reset & boot buttons :)
			COMMAND dfu-suffix -a ${target_name}.bin
			COMMAND dfu-util -a 0 -s 0x08000000 -D ${target_name}.bin
			)
	else()
		message(FATAL_ERROR "Select flashing method by setting CMAKE_FLASH")
	endif()
endfunction()


build_link_target( treefrog )
build_link_target( mic_recorder )
build_link_target( fft_record )
build_target( peak_detect )
build_target( tone_detect )
build_link_target( croak_only )
build_link_target( blink_led )

## Print some memory usage statistics.
add_custom_command(
	TARGET treefrog
	POST_BUILD
	COMMAND arm-none-eabi-readelf -l $<TARGET_FILE:treefrog>
	)

