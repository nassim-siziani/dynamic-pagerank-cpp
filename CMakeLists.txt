cmake_minimum_required(VERSION 3.16)
project(Ranking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

add_library(matrice_core
    Matrice.cc
    Matrice.hh
)
target_link_libraries(matrice_core PUBLIC Qt6::Widgets Qt6::Charts)

add_executable(Ranking main.cpp)
target_link_libraries(Ranking PRIVATE matrice_core)

add_executable(tests_m tests_m.cpp)
target_link_libraries(tests_m PRIVATE matrice_core)

add_executable(tests_p tests_p.cpp)
target_link_libraries(tests_p PRIVATE matrice_core)

add_executable(tests_alpha tests_alpha.cpp)
target_link_libraries(tests_alpha PRIVATE matrice_core)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/wb-cs-stanford.mtx"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)
