 cmake_minimum_required(VERSION 3.11)

project(fractal)

find_package(OpenCV REQUIRED HINTS "/usr/local/share/OpenCV")

add_executable(
    ${PROJECT_NAME} 
    fract.h 
    fract.cpp 
    fract_go.cpp
    tools.h
    tools.cpp
)
target_link_libraries(
    ${PROJECT_NAME}  
    ${OpenCV_LIBS}
)
