cmake_minimum_required(VERSION 3.13)
# 3.13 because of target_link_options
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

project(treefrog 
	VERSION 0.0.1
	LANGUAGES C CXX
	) 
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_C_STANDARD 99)

add_subdirectory(src)
if("${CMAKE_SYSTEM_NAME}" STREQUAL Linux)
	add_subdirectory(target/linux)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL Generic)
	add_subdirectory(target/opencm3)
endif()

