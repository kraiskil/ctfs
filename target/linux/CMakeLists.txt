include_directories(".")

function(build_target target_name)
	add_executable(${target_name}
		io_pulse.cc
		posix.cc
	)
	target_link_libraries(${target_name} pulse-simple pulse fftw3 libtreefrog main_${target_name})
endfunction()

add_executable(treefrog_pulse
	io_pulse.cc
	posix.cc
	)
target_link_libraries(treefrog_pulse pulse-simple pulse fftw3 libtreefrog main_treefrog)

add_executable(treefrog_file
	io_file.cc
	posix.cc
	)
target_link_libraries(treefrog_file sndfile fftw3 libtreefrog main_treefrog)

add_executable(deaf_frog
	io_pulse.cc
	posix.cc
	)
target_link_libraries(deaf_frog pulse-simple pulse libtreefrog main_croak_only)

add_executable(blink_led
	posix.cc
	io_none.cc
	)
target_link_libraries(blink_led libtreefrog main_blink_led)

build_target( mic_recorder )
build_target( fft_record )
