cmake_minimum_required(VERSION 3.8)
project(playground)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wattributes -std=c++17")

enable_testing()

# finds boost, triggers an error otherwise
find_package(Boost  REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

#set(SOURCE_FILES templateFromConstructorTest.cpp UtCallbackListN.hpp test_UtCallbackListN.cpp)
#add_executable(templateFromConstruct ${SOURCE_FILES})

set(SOURCE_FILES test_UtCallbackList.cpp CallbackList.h UtCallbackList.hpp UtCallbackListN.hpp )
add_executable(test_CallbackList ${SOURCE_FILES})
target_link_libraries(test_CallbackList ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

set(SOURCE_FILES test_UtCallbackListN.cpp CallbackList.h UtCallbackList.hpp UtCallbackListN.hpp )
add_executable(test_CallbackListN ${SOURCE_FILES})
target_link_libraries(test_CallbackListN ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(bitboard bitboardplay.cpp)
# indicates the include paths

# indicates the link paths




