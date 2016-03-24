cmake_minimum_required (VERSION 2.8)
project (MicroGridRTU)
set(MICROGRIDRTU_MAJOR_VERSION 1)
set(MICROGRIDRTU_MINOR_VERSION 0)
set(MICROGRIDRTU_MICRO_VERSION 0)
set(PIFACERTU_VERSION ${MICROGRIDRTU_MAJOR_VERSION}.${MICROGRIDRTU_MINOR_VERSION}.${MICROGRIDRTU_MICRO_VERSION})

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -L/usr/local/lib -lwiringPi")
set(CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_FLAGS})

# different release and debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# ----- microgridrtu executable -----
add_executable (MicroGridRTU MicroGridIOHandler.cpp MicroGridRTU.cpp)
target_link_libraries (MicroGridRTU LINK_PUBLIC asiodnp3 opendnp3 asiopal openpal pthread wiringPi)
