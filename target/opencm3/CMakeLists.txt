set(OPENCM3_DIR $ENV{OPENCM3_DIR})
if( NOT OPENCM3_DIR )
	message( FATAL_ERROR "Environment variable OPENCM3_DIR must be set\
 and point to the build/install directory of libopencm3")
endif()

if(${CMAKE_BOARD} STREQUAL stm32f4_disco)
	set(OPENCM3_TARGET "STM32F4")
	set(OPENCM3_LIB "opencm3_stm32f4")
	set(LINKERFILE stm32f4-discovery.ld)
	set(OPENOCD_OPTIONS -f interface/stlink-v2.cfg -f target/stm32f4x.cfg)
elseif(${CMAKE_BOARD} STREQUAL stm32f4_bluepill)
	set(OPENCM3_TARGET "STM32F4")
	set(OPENCM3_LIB "opencm3_stm32f4")
	set(LINKERFILE stm32f4-bluepill.ld)
else()
	message( FATAL_ERROR "set CMAKE_BOARD in your toolchain")
endif()


#Custom target with insane memories. Just to make target link:
# GCC produces much nicer statistics after linking on  e.g. why
# it would not have linked with smaller memories
if( "${CMAKE_BUILD_TYPE}" STREQUAL "LinkerDebug")
	set(LINKERFILE debugtarget.ld)
endif()


# Main building rules - common for all the targets.
function(build_target target_name)
	add_executable( ${target_name}
		${target_name}.cc
		io.cc
	)
	target_link_libraries(${target_name} libtreefrog ${OPENCM3_LIB})
	target_compile_options(${target_name}
		PRIVATE
			-D${OPENCM3_TARGET}
			-I/home/kraiskil/stuff/libopencm3/include
		)
	target_link_options(${target_name}
		PRIVATE 
			-T${CMAKE_CURRENT_SOURCE_DIR}/${LINKERFILE}
			-L${CMAKE_CURRENT_SOURCE_DIR}
			-L${OPENCM3_DIR}/lib
			-nostartfiles
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
			COMMAND dfu-tool convert raw ${target_name}.bin ${target_name}.dfu
			COMMAND dfu-util -a 0 -s 0x08000000 -D ${target_name}.dfu
			)
	else()
		message(FATAL_ERROR "Select flashing method by setting CMAKE_FLASH")
	endif()

endfunction()

build_target( main )
build_target( mic_recorder )
build_target( fft_record )
build_target( peak_detect )
build_target( tone_detect )

## Print some memory usage statistics.
add_custom_command(
	TARGET main
	POST_BUILD
	COMMAND arm-none-eabi-readelf -l $<TARGET_FILE:main>
	)

