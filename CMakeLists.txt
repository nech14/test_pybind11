cmake_minimum_required(VERSION 3.15)
project(example_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)

FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.13.1  # Вы можете указать нужную версию или 'master'
)

FetchContent_MakeAvailable(pybind11)


if(MSVC)
    add_compile_options(/utf-8)
endif()

pybind11_add_module(example main.cpp)