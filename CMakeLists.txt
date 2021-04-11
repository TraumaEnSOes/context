cmake_minimum_required( VERSION 3.16.3 )
project( UVPP CXX )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )
add_compile_options( -Wall -Wextra -pedantic )

find_package( PkgConfig REQUIRED )

pkg_check_modules( Uv REQUIRED IMPORTED_TARGET libuv )

if( BOOST_PATH )
    find_package( Boost 1.71.0 REQUIRED COMPONENTS context PATHS "${BOOST_PATH}" )
else( )
    find_package( Boost 1.71.0 REQUIRED COMPONENTS context )
endif( )

set( CMAKE_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/include" )

add_subdirectory( src )
add_subdirectory( poc )
