cmake_minimum_required(VERSION 3.0)

# C++ std 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-no-pie")

project(anpr)

#OpenCV
set(OpenCV_DIR libopencv_world.so)
link_directories( ${CMAKE_SOURCE_DIR}/libopencv/lib)
include_directories(${CMAKE_SOURCE_DIR}/libopencv/include/opencv4)

#Libevent
#set(LIBEVENT_LIBS "-levent")
#find_library(EVENT_LIBRARY REQUIRED PATHS /usr/lib /usr/local/lib)
set(EVENT_DIR libevent.so)
link_directories(${CMAKE_SOURCE_DIR}/libevent/lib)
include_directories(${CMAKE_SOURCE_DIR}/libevent/include)

#libcurl
set(CURL_DIR libcurl.so)
link_directories(${CMAKE_SOURCE_DIR}/libcurl/lib)
include_directories(${CMAKE_SOURCE_DIR}/libcurl/include)

#Setup
file(GLOB SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_DIR}
    ${EVENT_DIR}
	${CURL_DIR}
    -lpthread
    -lGL
    -ldl 
)

