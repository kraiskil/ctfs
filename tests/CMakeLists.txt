add_executable(unittests
	frog_tones_freq.cc
	frog_tones_peak.cc
	fft.cc
	)
target_link_libraries(unittests libtreefrog gtest gtest_main fftw3)
include(GoogleTest)
gtest_discover_tests(unittests)
