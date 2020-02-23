cmake_minimum_required(VERSION 3.0)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

project(treefrog 
	VERSION 0.0.1
	LANGUAGES C CXX
	) 
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_C_STANDARD 99)

add_subdirectory(src)
add_subdirectory(target/linux)

