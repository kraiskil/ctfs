add_executable(unittests
	fft.cc)
target_link_libraries(unittests libtreefrog gtest gtest_main fftw3)
include(GoogleTest)
gtest_discover_tests(unittests)
