cmake_minimum_required(VERSION 3.19)
# 3.13 because of target_link_options
# 3.19 because of presets
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

project(treefrog 
	VERSION 0.0.1
	LANGUAGES C CXX
	) 
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_C_STANDARD 99)


if("${CMAKE_SYSTEM_NAME}" STREQUAL Linux)
	add_subdirectory(target/linux)
	include_directories(target/linux)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL Generic)
	add_subdirectory(target/opencm3)
	include_directories(target/opencm3)
endif()

add_subdirectory(src)

#TODO: a Real Professional (TM) would have the tests running on host even when cross compiling :)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
	enable_testing()
	add_subdirectory(googletest)
	add_subdirectory(tests)
endif()

