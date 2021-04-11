#ifndef CORO_P_HPP
#define CORO_P_HPP

#include "clist.hpp"

#include <boost/context/continuation.hpp>

#include <exception>

namespace uvpp {

enum ProcessState {
    Ready,
    Finished,
    Exception
};

struct CoroPrivate : public clist::CircularListNode {
    CoroPrivate( void *( *entryPoint )( void * ), void *args = nullptr ) :
        m_entryPoint( entryPoint ),
        m_args( args ) {
    }
    CoroPrivate( ) = default;
    CoroPrivate( const CoroPrivate & ) = delete;
    CoroPrivate &operator=( const CoroPrivate & ) = delete;

    boost::context::continuation &continuation( ) { return m_continuation; }

    const std::string &name( ) const { return m_name; }
    void setName( std::string name ) {
        m_name = std::move( name );
    }

    ProcessState state( ) const { return m_state; }
    void setState( ProcessState state ) {
        m_state = state;
    }

    void *exitValue( ) const { return m_exitValue; }
    std::exception_ptr exception( ) const { return m_exception; }

    void run( ) {
        try {
            m_exitValue = m_entryPoint( m_args );
            m_state = ProcessState::Finished;
        } catch( ... ) {
            m_exception = std::current_exception( );
            m_state = ProcessState::Exception;
        }
    }

private:
    void *( *m_entryPoint )( void * );
    void *m_args;
    void *m_exitValue = nullptr;
    ProcessState m_state = ProcessState::Ready;
    std::string m_name;
    boost::context::continuation m_continuation;
    std::exception_ptr m_exception;
};

extern CoroPrivate *ThisCoro;

void Scheduler( );

} // namespace uvpp.

#endif
