#include "coro_p.hpp"

#include "uvpp/coro.hpp"

#include "uv.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <type_traits>

namespace uvpp {

CoroPrivate *ThisCoro;

void yield( ) {
    ThisCoro->continuation( ) = ThisCoro->continuation( ).resume( );
}

Coro createCoro( void *( *ep )( void * ), void *arg ) {
    auto newproc = new CoroPrivate( ep, arg );

    if( ThisCoro == nullptr ) {
        // Es la primera tarea. Tenemos que inicializar.
        clist::init( newproc );
        ThisCoro = newproc;
    } else {
        // Hay tareas ya creadas.
        clist::insertBefore( ThisCoro, newproc );
    }

    return Coro( newproc );
}

boost::context::continuation Wrapper( boost::context::continuation &&sink ) {
    ThisCoro->continuation( ) = std::move( sink );
    ThisCoro->run( );

    return std::move( ThisCoro->continuation( ) );
}

void runLoop( RunLoopOptions options ) {
    static uv_loop_t loop;

    uv_loop_init( &loop );

    if( options ) {
        // Se nos han pasado opciones. Las convertimos a opciones de libuv.
        if( options & RunLoopOptions::BlockSigProf ) {
            uv_loop_configure( &loop, UV_LOOP_BLOCK_SIGNAL, SIGPROF );
        }
        if( options & RunLoopOptions::MetricsIdleTime ) {
            uv_loop_configure( &loop, UV_METRICS_IDLE_TIME );
        }
    }
    
    while( true ) {
        if( ThisCoro ) {
            if( ThisCoro->continuation( ).operator bool( ) ) {
                // Ya se llamó con anterioridad.
                ThisCoro->continuation( ) = ThisCoro->continuation( ).resume( );
            } else {
                // Es la primera vez que se llama.
                ThisCoro->continuation( ) = boost::context::callcc( Wrapper );
            }

            if( ThisCoro->state( ) == ProcessState::Finished ) {
                // Terminó su ejecución.
                std::cout << ThisCoro->name( ) << " terminó su ejecución" << std::endl;

                if( clist::unique( ThisCoro ) ) {
                    delete ThisCoro;
                    break; // Fin del loop.
                } else {
                    auto tmp = clist::next( ThisCoro );
                    delete ThisCoro;
                    ThisCoro = static_cast< CoroPrivate * >( tmp );
                }
            } else if( ThisCoro->state( ) == ProcessState::Exception ) {
                // Se produjo una excepción.
                std::cout << ThisCoro->name( ) << " lanzó una excepción" << std::endl;

                if( clist::unique( ThisCoro ) ) {
                    delete ThisCoro;
                    break; // Fin del loop.
                } else {
                    auto tmp = clist::next( ThisCoro );
                    delete ThisCoro;
                    ThisCoro = static_cast< CoroPrivate * >( tmp );
                }
            } else {
                ThisCoro = static_cast< CoroPrivate * >( clist::next( ThisCoro ) );
            }

            int uvresult;
            // Ejecutamos uno de los eventos pendientes de libuv.
            if( ThisCoro ) {
                // Hay coros esperando. Si no hay eventos de libuv, no los esperamos y seguimos ejecutando.
                uvresult = uv_run( &loop, UV_RUN_NOWAIT );
            } else {
                // No hay coros esperando. Esperamos hasta que se produzca un evento libuv.
                uvresult = uv_run( &loop, UV_RUN_ONCE );
            }

            if( uvresult ) {
                // Hubo algún problema con libuv.
            }
        }
    }

    uv_loop_close( &loop );
}

Coro thisCoro( ) {
    return Coro( ThisCoro );
}

} // namespace uvpp.

