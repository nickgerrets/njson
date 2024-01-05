cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(njson VERSION 1.0 DESCRIPTION "easy json!")

add_library(${PROJECT_NAME}
	src/Json.cpp
	src/Parsing.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	include/
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
)
