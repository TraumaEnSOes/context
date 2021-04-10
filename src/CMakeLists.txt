add_library( uvpp-coro SHARED
    context.cpp
)

target_include_directories( uvpp-coro PRIVATE "${CMAKE_INCLUDE_PATH}" )
target_link_libraries( uvpp-coro PUBLIC Boost::context )