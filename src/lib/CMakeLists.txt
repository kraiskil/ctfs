add_library(libtreefrog STATIC
	croak.cc
	debug.cc
	fft.cc
	fft_float.cc
	harmonics.cc
	peak_detect.cc
	treefrog_lib.cc
	tones.cc
	sin_table.cc
	sin_table_float.cc
	)
target_include_directories(libtreefrog
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)

