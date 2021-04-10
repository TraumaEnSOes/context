cmake_minimum_required( VERSION 3.16.3 )
project( UVPP CXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )
add_compile_options( -Wall -Wextra -pedantic )

find_package( Boost 1.71 REQUIRED COMPONENTS context )

set( CMAKE_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/include" )

add_subdirectory( src )
add_subdirectory( poc )
