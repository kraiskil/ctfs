#Use this file as CMake toolchain file to compile for
#baremetal ARM. e.g.
# cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake .
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION OpenCM3)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_C_FLAGS "--specs=nosys.specs -Wall -Werror -mcpu=cortex-m0 -mthumb -DNDEBUG")
set(CMAKE_CXX_FLAGS "--specs=nosys.specs -Wall -Werror -mcpu=cortex-m0 -mthumb -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
