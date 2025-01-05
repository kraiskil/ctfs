include_directories(".")

function(build_target target_name)
	add_executable(${target_name}
		io_pulse.cc
		posix.cc
	)
	target_link_libraries(${target_name} pulse-simple pulse fftw3 libtreefrog main_${target_name} pthread)
endfunction()


build_target( mic_recorder )
build_target( fft_record )
build_target( blink_led )
build_target( croak_only )
build_target( treefrog )
