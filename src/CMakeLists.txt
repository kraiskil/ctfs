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

add_library(main_croak_only OBJECT
	main_croak_only.cc)

add_library(main_treefrog OBJECT
	treefrog.cc)
