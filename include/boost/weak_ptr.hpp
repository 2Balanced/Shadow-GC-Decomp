#ifndef BOOST_WEAK_PTR_HPP_INCLUDED
#define BOOST_WEAK_PTR_HPP_INCLUDED

//
//  weak_ptr.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/weak_ptr.htm for documentation.
//
//  Trimmed from boost 1.32 to the single-threaded configuration the game uses; reset() is
//  declared out of line, as in the game.
//

#include <boost/shared_ptr.hpp>

namespace boost
{

template<class T> class weak_ptr
{
private:

    typedef weak_ptr<T> this_type;

public:

    typedef T element_type;

    weak_ptr(): px(0), pn() // never throws in 1.30+
    {
    }

//  generated copy constructor, assignment, destructor are fine

    template<class Y>
    weak_ptr(weak_ptr<Y> const & r): pn(r.pn) // never throws
    {
        px = r.lock().get();
    }

    template<class Y>
    weak_ptr(shared_ptr<Y> const & r): px(r.px), pn(r.pn) // never throws
    {
    }

#ifdef BOOST_WEAK_PTR_CONVERTING_ASSIGNMENT
    // boost 1.32's converting assignment from weak_ptr<Y>, left out of the trimmed header. Only the
    // units that use it define the macro: another member template here renumbers the compiler's
    // @N local symbols in every unit that includes this header.
    template<class Y>
    weak_ptr & operator=(weak_ptr<Y> const & r) // never throws
    {
        px = r.lock().get();
        pn = r.pn;
        return *this;
    }
#endif

    template<class Y>
    weak_ptr & operator=(shared_ptr<Y> const & r) // never throws
    {
        px = r.px;
        pn = r.pn;
        return *this;
    }

    shared_ptr<T> lock() const // never throws
    {
        // single threaded: no try/catch needed
        return expired()? shared_ptr<element_type>(): shared_ptr<element_type>(*this);
    }

    long use_count() const // never throws
    {
        return pn.use_count();
    }

    bool expired() const // never throws
    {
        return pn.use_count() == 0;
    }

    void reset(); // never throws; out of line in the game

    void swap(this_type & other) // never throws
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    void _internal_assign(T * px2, detail::shared_count const & pn2)
    {
        px = px2;
        pn = pn2;
    }

    T * px;                     // contained pointer
    detail::weak_count pn;      // reference counter

};  // weak_ptr

} // namespace boost

#endif  // #ifndef BOOST_WEAK_PTR_HPP_INCLUDED
