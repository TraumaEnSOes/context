#include "processprivate.hpp"

#include <cstdlib>
#include <iostream>

int ProcessPrivate::listCount = 0;

ProcessPrivate *ThisProcess;

void yield( ) {
    ThisProcess->continuation = ThisProcess->continuation.resume( );
}

static boost::context::continuation Wrapper( boost::context::continuation &&sink ) {
    ThisProcess->continuation = std::move( sink );

    try {
        ThisProcess->entry( );
        ThisProcess->state = ProcessState::Finished;
    } catch( ... ) {
        ThisProcess->exception = std::current_exception( );
        ThisProcess->state = ProcessState::Exception;
    }

    return std::move( ThisProcess->continuation );
}

void createProcess( void ( *ep )( ), const char *name ) {
    auto newproc = new ProcessPrivate( name );
    newproc->entry = ep;

    ThisProcess->insertBefore( newproc );
}

void Scheduler( ) {
    while( true ) {
        if( ThisProcess ) {
            if( ThisProcess->continuation.operator bool( ) ) {
                // Ya se llamó con anterioridad.
                ThisProcess->continuation = ThisProcess->continuation.resume( );
            } else {
                // Es la primera vez que se llama.
                ThisProcess->continuation = boost::context::callcc( Wrapper );
            }

            if( ThisProcess->state == ProcessState::Finished ) {
                // Terminó su ejecución.
                std::cout << ThisProcess->name << " terminó su ejecución" << std::endl;

                if( ThisProcess->unique( ) ) {
                    delete ThisProcess;
                    break; // Fin del loop.
                } else {
                    auto tmp = ThisProcess->next_;
                    delete ThisProcess;
                    ThisProcess = tmp;
                }
            } else if( ThisProcess->state == ProcessState::Exception ) {
                // Se produjo una excepción.
                std::cout << ThisProcess->name << " lanzó una excepción" << std::endl;

                if( ThisProcess->unique( ) ) {
                    delete ThisProcess;
                    break; // Fin del loop.
                } else {
                    auto tmp = ThisProcess->next_;
                    delete ThisProcess;
                    ThisProcess = tmp;
                }
            } else {
                ThisProcess = ThisProcess->next_;
            }
        }
    }

    std::cout << "Fin del bucle" << std::endl;
}
