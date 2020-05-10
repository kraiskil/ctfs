add_executable(unittests
	frog_tones_freq.cc
	frog_tones_harmonics.cc
	frog_tones_peak.cc
	harmonics.cc
	fft.cc
	main.cc
	)
target_link_libraries(unittests libtreefrog gtest gtest_main gmock)
include(GoogleTest)
gtest_discover_tests(unittests)
