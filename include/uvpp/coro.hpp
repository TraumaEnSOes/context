#ifndef UVPP_CORO_HPP
#define UVPP_CORO_HPP

#include <string>

namespace uvpp {

struct CoroPrivate;

struct Coro {
    ~Coro( ) = default;
    Coro( CoroPrivate *priv = nullptr ) : m_details( priv ) { }
    Coro( const Coro & ) = default;
    Coro( Coro &&other ) : m_details( other.m_details ) {
        if( m_details ) {
            other.m_details = nullptr;
        }
    }
    Coro &operator=( const Coro & ) = default;
    Coro &operator=( Coro &&other ) {
        m_details = other.m_details;
        if( m_details ) {
            other.m_details = nullptr;
        }
        return *this;
    }

    bool operator==( const Coro &other ) const {
        return m_details == other.m_details;
    }
    bool operator!=( const Coro &other ) const {
        return m_details != other.m_details;
    }
    bool operator!( ) const {
        return !m_details;
    }

    const std::string &name( ) const;
    void setName( std::string name );

private:
    CoroPrivate *m_details;
};

Coro createCoro( void *( *ep )( void * ), void *args = nullptr );
void yield( );
Coro thisCoro( );

void runLoop( );

} // namespace uvpp.

#endif
