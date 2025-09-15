cmake_minimum_required(VERSION 4.0)
project(ProjetoConclusaoCurso)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ProjetoConclusaoCurso main.cpp
        geometry/Cell.h
        geometry/Edge.h
        geometry/Point.h
        geometry/UniformGrid.h
        geometry/Polygon.cpp
        geometry/Polygon.h
        graph/DynamicGraph.cpp
        graph/DynamicGraph.h
        screen/Screen.cpp
        screen/Screen.h
        helper/PointHelper.h
        helper/GridHelper.h)

target_link_libraries(ProjetoConclusaoCurso PRIVATE sfml-graphics sfml-window sfml-system)

configure_file(${CMAKE_SOURCE_DIR}/input1.txt ${CMAKE_BINARY_DIR}/input1.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/input2.txt ${CMAKE_BINARY_DIR}/input2.txt COPYONLY)
