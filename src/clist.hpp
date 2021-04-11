#ifndef CLIST_HPP
#define CLIST_HPP

namespace clist {

struct CircularListNode;

inline void insertBefore( CircularListNode *target, CircularListNode *newNode );
inline bool unique( const CircularListNode *target );
inline void init( CircularListNode *target );
inline CircularListNode *next( const CircularListNode *target );

struct CircularListNode {
    friend void insertBefore( CircularListNode *target, CircularListNode *newNode );
    friend bool unique( const CircularListNode *target );
    friend void init( CircularListNode *target );
    friend inline CircularListNode *next( const CircularListNode *target );

    ~CircularListNode( ) {
        // Quitamos de la lista.
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }

private:
    CircularListNode *m_next;
    CircularListNode *m_prev;
};

void insertBefore( CircularListNode *target, CircularListNode *newNode ) {
    newNode->m_next = target;
    newNode->m_prev = target->m_prev;
    target->m_prev->m_next = newNode;
    target->m_prev = newNode;
}

bool unique( const CircularListNode *target ) { return target->m_next == target; }

void init( CircularListNode *target ) {
    target->m_prev = target;
    target->m_next = target;
}

CircularListNode *next( const CircularListNode *target ) { return target->m_next; }

} // namespace clist.

#endif
