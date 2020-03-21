add_library(libtreefrog STATIC
	croak.cc
	fft.cc
	harmonics.cc
	treefrog.cc
	sin_table.cc
	)
target_include_directories(libtreefrog
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)
