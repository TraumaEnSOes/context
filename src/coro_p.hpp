#ifndef CORO_P_HPP
#define CORO_P_HPP

#include <boost/context/continuation.hpp>

#include <exception>

namespace uvpp {

enum ProcessState {
    Ready,
    Finished,
    Exception
};

struct CoroPrivate {
    ~CoroPrivate( ) {
        // Quitamos de la lista.
        prev_->next_ = next_;
        next_->prev_ = prev_;

        --listCount;
    }
    CoroPrivate( const char *n = "Unknown process" ) : name( n ) { }
    CoroPrivate( const CoroPrivate & ) = delete;
    CoroPrivate &operator=( const CoroPrivate & ) = delete;

    std::string name;
    int state = ProcessState::Ready;
    void *( *entry )( void * ) = nullptr;
    boost::context::continuation continuation;
    std::exception_ptr exception;
    void *exitValue;
    void *argument;

    CoroPrivate *prev_;
    CoroPrivate *next_;

    void insertBefore( CoroPrivate *node ) {
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

extern CoroPrivate *ThisCoro;

void Scheduler( );

} // namespace uvpp.

#endif
