cmake_minimum_required(VERSION 3.16)
project(my_sync)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_CXX_STANDARD 14)

add_executable(my_sync main.cpp include/common.h core/Thread.cpp core/Thread.h core/Monitor.cpp core/Monitor.h core/ObjectMonitor.cpp core/ObjectMonitor.h core/ParkEvent.cpp core/ParkEvent.h core/Atomic.cpp core/Atomic.h core/ObjectWaiter.cpp core/ObjectWaiter.h core/ObjectSynchronizer.cpp core/ObjectSynchronizer.h oops/InstanceOopDesc.cpp oops/InstanceOopDesc.h oops/MarkOopDesc.cpp oops/MarkOopDesc.h oops/Klass.cpp oops/Klass.h core/GlobalDefinitions.h core/BasicLock.cpp core/BasicLock.h core/BiasedLocking.cpp core/BiasedLocking.h)
