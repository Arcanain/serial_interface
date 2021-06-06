cmake_minimum_required(VERSION 3.0.2)
project(serial_interface)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  message_generation
  roscpp
  rospy
  std_msgs
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
   INCLUDE_DIRS include
#  LIBRARIES serial_interface
   CATKIN_DEPENDS message_generation roscpp rospy std_msgs
#  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(
  serial_interface 
  src/serial_interface_node.cpp 
  src/SerialDriver.cpp 
  src/SerialInterface.cpp
)

target_link_libraries(serial_interface ${catkin_LIBRARIES})