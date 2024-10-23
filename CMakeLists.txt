cmake_minimum_required(VERSION 3.16)
project(my_sync)

set(CMAKE_CXX_STANDARD 14)

add_executable(my_sync main.cpp include/common.h core/Thread.cpp core/Thread.h core/Monitor.cpp core/Monitor.h core/ObjectMonitor.cpp core/ObjectMonitor.h core/ParkEvent.cpp core/ParkEvent.h core/Atomic.cpp core/Atomic.h core/ObjectWaiter.cpp core/ObjectWaiter.h)
