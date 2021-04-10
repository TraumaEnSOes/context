#ifndef PROCESSPRIVATE_HPP
#define PROCESSPRIVATE_HPP

#include <boost/context/continuation.hpp>

#include <exception>

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

extern ProcessPrivate *ThisProcess;

void Scheduler( );
void createProcess( void ( *ep )( ), const char *name = nullptr );
void yield( );

#endif
