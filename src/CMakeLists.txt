add_library(libtreefrog STATIC
	treefrog.cc)
target_include_directories(libtreefrog
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)
