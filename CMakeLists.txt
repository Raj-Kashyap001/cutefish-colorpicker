cmake_minimum_required(VERSION 3.16)

project(cutefish-colorpicker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

find_package(CutefishToolkit REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/loupeview.cpp
    src/colorpickerview.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE COLORPICKER_VERSION="${PROJECT_VERSION}")
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Cutefish::Toolkit)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
