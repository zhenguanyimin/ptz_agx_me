# cmake file for agx

cmake_minimum_required(VERSION 3.8.1)
set(CMAKE_BUILD_TYPE "Debug")#Debug

project(agx-app VERSION 0.0.1)

message("CMAKE_SOURCE_DIR:${CMAKE_SOURCE_DIR}")
set(ROOT_DIR ${CMAKE_SOURCE_DIR})

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wl,--copy-dt-needed-entries") #11
set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -g -ggdb -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -Wall")
# set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")


# import jetson-inference and jetson-utils packages.
# note that if you didn't do "sudo make install"
# while building jetson-inference, this will error.
find_package(jetson-utils)
# find_package(jetson-inference)


# CUDA is required
find_package(CUDA)

# add directory for libnvbuf-utils to program
link_directories(/usr/lib/aarch64-linux-gnu/tegra)

include_directories(/usr/include/gstreamer-1.0/)
include_directories(/usr/include/glib-2.0/)
include_directories(/usr/lib/aarch64-linux-gnu/glib-2.0/include/)
link_directories(/usr/lib/aarch64-linux-gnu/)

# cuda
include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)
# tensorrt
include_directories(/usr/include/x86_64-linux-gnu/)
link_directories(/usr/lib/x86_64-linux-gnu/)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

#engin3
include_directories(/usr/local/include/eigen3)
include_directories(/usr/include/eigen3)
include_directories(/usr/local/include/eigen3/Eigen)
include_directories(/usr/include/eigen3/Eigen)

#add source file directories
add_subdirectory(src)

# link_directories(lib)
# link_directories(libs/unbuntn-64_g++_6.2.0/)

include_directories(${ROOT_DIR}/include/)
link_directories(${ROOT_DIR}/thirdpart/zlog/lib/)
#set compile options
# add_definitions(-fstack-protector-strong)
# add_definitions(-O2)
# add_definitions(-D_FORTIFY_SOURCE=2)
# add_definitions(-Wformat -Wformat-security -Werror=format-security -Wdate-time)
# add_definitions(-D__XEN_LINUX__)
add_definitions(-g)

#general the target file
add_executable(${PROJECT_NAME}.elf src/main.c)

target_link_libraries(${PROJECT_NAME}.elf 
	-Wl,--start-group
	jetson-utils
	${CUDA_LIBRARIES}
    ${TensorRT_LIBRARIES}
	${SRC_LIBS}
	pthread
	rt
	m
	gstreamer-1.0
    gobject-2.0
	zlog
#	tmControlClient
	-Wl,--end-group
	)

target_link_libraries(${PROJECT_NAME}.elf nvinfer)
target_link_libraries(${PROJECT_NAME}.elf nvinfer_plugin)
target_link_libraries(${PROJECT_NAME}.elf cudart)

target_link_libraries(${PROJECT_NAME}.elf ${OpenCV_LIBS})

