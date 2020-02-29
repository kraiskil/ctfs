set(OPENCM3_DIR $ENV{OPENCM3_DIR})
if( NOT OPENCM3_DIR )
	message( FATAL_ERROR "Environment variable OPENCM3_DIR must be set\
 and point to the build/install directory of libopencm3")
endif()

add_executable(treefrog 
	io.cc
	main.cc
	)
target_link_libraries(treefrog libtreefrog opencm3_stm32f0)
target_compile_options(treefrog
	PRIVATE
		#OpenCM3 target selection
		-DSTM32F0
		-I/home/kraiskil/stuff/libopencm3/include
	)
target_link_options(treefrog
	PRIVATE 
		-T${CMAKE_CURRENT_SOURCE_DIR}/stm32f0-discovery.ld
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
