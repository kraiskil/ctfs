add_executable(treefrog_pulse
	io_pulse.cc
	main.cc
	play_croak.cc
	)
target_link_libraries(treefrog_pulse pulse-simple pulse libtreefrog)

add_executable(treefrog_file
	io_file.cc
	main.cc
	play_croak.cc
	)
target_link_libraries(treefrog_file sndfile libtreefrog)

