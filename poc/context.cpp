#include "processprivate.hpp"

#include <iostream>

static void Worker( ) {
    for( int idx = 0; idx < 5; ++idx ) {
        std::cout << ThisProcess->name << ", idx " << idx << std::endl;
        yield( );
    }
}

static void Master( ) {
    createProcess( Worker, "Worker 1" );
    createProcess( Worker, "Worker 2" );
    createProcess( Worker, "Worker 3" );
}

int main( ) {
    // Inicializamos la lista circular de procesos.
    {
        ProcessPrivate *newProcess = new ProcessPrivate( "Master" );
        newProcess->initCircularList( );
        newProcess->entry = Master;
        ThisProcess = newProcess;
    }

    // Lanzamos el planificador.
    Scheduler( );

    std::cout << std::endl;

    return 0;
}
