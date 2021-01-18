#include <boost/context/continuation.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>

enum ProcessState {
    Ready,
    Finished,
    Exception
};

struct ProcessPrivate {
    ~ProcessPrivate( ) {
        // Quitamos de la lista.
        prev_->next_ = next_;
        next_->prev_ = prev_;

        --listCount;
    }
    ProcessPrivate( const char *n = "Unknown process" ) : name( n ) {
    }
    ProcessPrivate( const ProcessPrivate & ) = delete;
    ProcessPrivate &operator=( const ProcessPrivate & ) = delete;

    const char *name;
    int state = ProcessState::Ready;
    void ( *entry )( ) = nullptr;
    boost::context::continuation continuation;
    std::exception_ptr exception;

    ProcessPrivate *prev_;
    ProcessPrivate *next_;

    void insertBefore( ProcessPrivate *node ) {
        node->next_ = this;
        node->prev_ = prev_;
        prev_->next_ = node;
        prev_ = node;

        ++listCount;
    }

    bool unique( ) const { return next_ == this; }

    void initCircularList( ) {
        prev_ = this;
        next_ = this;

        listCount = 1;
    }

    static int listCount;
};

int ProcessPrivate::listCount = 0;

static ProcessPrivate *ThisProcess;

static void yield( ) {
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

static void createProcess( void ( *ep )( ), const char *name = nullptr ) {
    auto newproc = new ProcessPrivate( name );
    newproc->entry = ep;

    ThisProcess->insertBefore( newproc );
}

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
