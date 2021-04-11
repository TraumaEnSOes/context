#include "coro_p.hpp"

#include "uvpp/coro.hpp"

#include <cstdlib>
#include <iostream>

namespace uvpp {

int CoroPrivate::listCount = 0;

CoroPrivate *ThisCoro;

void yield( ) {
    ThisCoro->continuation = ThisCoro->continuation.resume( );
}

static boost::context::continuation Wrapper( boost::context::continuation &&sink ) {
    ThisCoro->continuation = std::move( sink );

    try {
        ThisCoro->exitValue = ThisCoro->entry( ThisCoro->argument );
        ThisCoro->state = ProcessState::Finished;
    } catch( ... ) {
        ThisCoro->exception = std::current_exception( );
        ThisCoro->state = ProcessState::Exception;
    }

    return std::move( ThisCoro->continuation );
}

Coro createCoro( void *( *ep )( void * ), void *arg ) {
    auto newproc = new CoroPrivate( );
    newproc->entry = ep;
    newproc->exitValue = nullptr;
    newproc->argument = arg;

    if( ThisCoro == nullptr ) {
        // Es la primera tarea. Tenemos que inicializar.
        newproc->initCircularList( );
        ThisCoro = newproc;
    } else {
        // Hay tareas ya creadas.
        ThisCoro->insertBefore( newproc );
    }

    return Coro( newproc );
}

void runLoop( ) {
    while( true ) {
        if( ThisCoro ) {
            if( ThisCoro->continuation.operator bool( ) ) {
                // Ya se llamó con anterioridad.
                ThisCoro->continuation = ThisCoro->continuation.resume( );
            } else {
                // Es la primera vez que se llama.
                ThisCoro->continuation = boost::context::callcc( Wrapper );
            }

            if( ThisCoro->state == ProcessState::Finished ) {
                // Terminó su ejecución.
                std::cout << ThisCoro->name << " terminó su ejecución" << std::endl;

                if( ThisCoro->unique( ) ) {
                    delete ThisCoro;
                    break; // Fin del loop.
                } else {
                    auto tmp = ThisCoro->next_;
                    delete ThisCoro;
                    ThisCoro = tmp;
                }
            } else if( ThisCoro->state == ProcessState::Exception ) {
                // Se produjo una excepción.
                std::cout << ThisCoro->name << " lanzó una excepción" << std::endl;

                if( ThisCoro->unique( ) ) {
                    delete ThisCoro;
                    break; // Fin del loop.
                } else {
                    auto tmp = ThisCoro->next_;
                    delete ThisCoro;
                    ThisCoro = tmp;
                }
            } else {
                ThisCoro = ThisCoro->next_;
            }
        }
    }

    std::cout << "Fin del bucle" << std::endl;
}

Coro thisCoro( ) {
    return Coro( ThisCoro );
}

} // namespace uvpp.

