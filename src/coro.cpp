#include <uvpp/coro.hpp>

#include "coro_p.hpp"

namespace uvpp {

const std::string &Coro::name( ) const {
    return m_details->name( );
}

void Coro::setName( std::string name ) {
    m_details->setName( std::move( name ) );
}

}