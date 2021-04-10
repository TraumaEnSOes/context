add_executable( context context.cpp )

add_dependencies( context uvpp-coro )

target_include_directories( context PRIVATE "${CMAKE_INCLUDE_PATH}" )

target_link_libraries( context
    PRIVATE uvpp-coro
)

