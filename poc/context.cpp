#include "uvpp/coro.hpp"

#include <iostream>

static void *Worker( void * ) {
    for( int idx = 0; idx < 5; ++idx ) {
        std::cout << uvpp::thisCoro( ).name( ) << ", idx " << idx << std::endl;
        uvpp::yield( );
    }

    return nullptr;
}

static void *Master( void * ) {
    using namespace uvpp;

    createCoro( Worker ).setName( "Worker 1" );
    createCoro( Worker ).setName( "Worker 2" );
    createCoro( Worker ).setName( "Worker 3" );

    return nullptr;
}

int main( ) {
    uvpp::createCoro( Master ).setName( "Master" );

    uvpp::runLoop( );

    std::cout << std::endl;

    return 0;
}
