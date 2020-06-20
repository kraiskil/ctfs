add_executable(treefrog_pulse
	io_pulse.cc
	main.cc
	posix.cc
	)
target_link_libraries(treefrog_pulse pulse-simple pulse fftw3 libtreefrog)

add_executable(treefrog_file
	io_file.cc
	main.cc
	posix.cc
	)
target_link_libraries(treefrog_file sndfile fftw3 libtreefrog)

add_executable(deaf_frog
	io_pulse.cc
	posix.cc
	)
target_link_libraries(deaf_frog pulse-simple pulse libtreefrog main_croak_only)

