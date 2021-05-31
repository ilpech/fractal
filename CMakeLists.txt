 cmake_minimum_required(VERSION 3.11)

project(fractallib)

find_package(OpenCV REQUIRED HINTS "/usr/local/share/OpenCV")

add_library(
    ${PROJECT_NAME} 
    fract.h 
    fract.cpp 
    tools.h
    tools.cpp
)
target_link_libraries(
    ${PROJECT_NAME}  
    ${OpenCV_LIBS}
)

project(fractal)
add_executable(
    ${PROJECT_NAME} 
    fract_go.cpp
)

target_link_libraries(
    ${PROJECT_NAME}  
    fractallib
)

project(vecfield)

add_executable(
    ${PROJECT_NAME}
    vecfield.cpp
)

target_link_libraries(
    ${PROJECT_NAME}  
    fractallib
)