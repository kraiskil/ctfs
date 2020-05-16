add_library(libtreefrog STATIC
	croak.cc
	debug.cc
	fft.cc
	fft_float.cc
	frog_tones.cc
	harmonics.cc
	treefrog.cc
	sin_table.cc
	sin_table_float.cc
	)
target_include_directories(libtreefrog
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)
