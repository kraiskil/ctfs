add_executable(unittests
	fft.cc)
target_link_libraries(unittests libtreefrog gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(unittests)
