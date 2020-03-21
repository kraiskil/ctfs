set(OPENCM3_DIR $ENV{OPENCM3_DIR})
if( NOT OPENCM3_DIR )
	message( FATAL_ERROR "Environment variable OPENCM3_DIR must be set\
 and point to the build/install directory of libopencm3")
endif()

if( ${CMAKE_OPENCM3_TARGET} STREQUAL stm32f4)
	set(OPENCM3_TARGET "STM32F4")
	set(OPENCM3_LIB "opencm3_stm32f4")
	set(LINKERFILE stm32f4-discovery.ld)
elseif( ${CMAKE_OPENCM3_TARGET} STREQUAL stm32f0)
	set(OPENCM3_TARGET "STM32F0")
	set(OPENCM3_LIB "opencm3_stm32f0")
	set(LINKERFILE stm32f0-discovery.ld)
else()
	message( FATAL_ERROR "set CMAKE_OPENCM3_TARGET in your toolchain")
endif()


add_executable(treefrog 
	io.cc
	main.cc
	)
target_link_libraries(treefrog libtreefrog ${OPENCM3_LIB})
target_compile_options(treefrog
	PRIVATE
		-D${OPENCM3_TARGET}
		-I/home/kraiskil/stuff/libopencm3/include
	)
#Custom target with insane memories. Just to make target link:
# GCC produces much nicer statistics after linking on  e.g. why
# it would not have linked with smaller memories
if( "${CMAKE_BUILD_TYPE}" STREQUAL "LinkerDebug")
	set(LINKERFILE debugtarget.ld)
endif()

target_link_options(treefrog
	PRIVATE 
		-T${CMAKE_CURRENT_SOURCE_DIR}/${LINKERFILE}
		-L${CMAKE_CURRENT_SOURCE_DIR}
		-L${OPENCM3_DIR}/lib
		-nostartfiles
	)
# Print some memory usage statistics.
add_custom_command(
	TARGET treefrog
	POST_BUILD
	COMMAND arm-none-eabi-readelf -l $<TARGET_FILE:treefrog>
	)
