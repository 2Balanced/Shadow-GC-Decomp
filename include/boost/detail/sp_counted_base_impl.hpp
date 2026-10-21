#ifndef BOOST_DETAIL_SP_COUNTED_BASE_IMPL_HPP_INCLUDED
#define BOOST_DETAIL_SP_COUNTED_BASE_IMPL_HPP_INCLUDED

//
//  detail/sp_counted_base_impl.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  Split out of boost 1.32's detail/shared_count.hpp for this project: bad_weak_ptr and the
//  reference-count control blocks. Single-threaded (no mutex), as the game uses them; the hook
//  run when the last weak reference goes is named destroy() (the game's vtable order is
//  destructor, dispose, destroy, get_deleter).
//

#include <boost/config.hpp>
#include <boost/checked_delete.hpp>
#include <boost/throw_exception.hpp>

#include <exception>
#include <typeinfo>

namespace boost
{

class bad_weak_ptr: public std::exception
{
public:

    virtual char const * what() const
    {
        return "boost::bad_weak_ptr";
    }
};

namespace detail
{

class sp_counted_base
{
public:

    sp_counted_base(): use_count_(1), weak_count_(1)
    {
    }

#ifdef BOOST_SP_EXTERN_BASE_DESTRUCTOR
    // This project: a unit that defines BOOST_SP_EXTERN_BASE_DESTRUCTOR references the game's
    // shared copy of the destructor (0x80009500) and so emits no vtable of its own
    // (Game/ElecSearchLight.cpp).
    virtual ~sp_counted_base(); // nothrow
#else
    virtual ~sp_counted_base() // nothrow
    {
    }
#endif

    // dispose() is called when use_count_ drops to zero, to release
    // the resources managed by *this.

    virtual void dispose() = 0; // nothrow

    // destroy() is called when weak_count_ drops to zero.

    virtual void destroy() // nothrow
    {
        delete this;
    }

    virtual void * get_deleter(std::type_info const & ti) = 0;

    void add_ref_copy()
    {
        ++use_count_;
    }

    void add_ref_lock()
    {
        if(use_count_ == 0) boost::throw_exception(boost::bad_weak_ptr());
        ++use_count_;
    }

    void release() // nothrow
    {
        {
            long new_use_count = --use_count_;

            if(new_use_count != 0) return;
        }

        dispose();
        weak_release();
    }

    void weak_add_ref() // nothrow
    {
        ++weak_count_;
    }

    void weak_release() // nothrow
    {
        if(--weak_count_ == 0)
        {
            destroy();
        }
    }

    long use_count() const // nothrow
    {
        return use_count_;
    }

private:

    sp_counted_base(sp_counted_base const &);
    sp_counted_base & operator= (sp_counted_base const &);

    long use_count_;        // #shared
    long weak_count_;       // #weak + (#shared != 0)
};

template<class P, class D> class sp_counted_base_impl: public sp_counted_base
{
private:

    P ptr; // copy constructor must not throw
    D del; // copy constructor must not throw

    sp_counted_base_impl(sp_counted_base_impl const &);
    sp_counted_base_impl & operator= (sp_counted_base_impl const &);

    typedef sp_counted_base_impl<P, D> this_type;

public:

    // pre: initial_use_count <= initial_weak_count, d(p) must not throw

    sp_counted_base_impl(P p, D d): ptr(p), del(d)
    {
    }

    virtual void dispose() // nothrow
    {
        del(ptr);
    }

    virtual void * get_deleter(std::type_info const & ti)
    {
        return ti == typeid(D)? &del: 0;
    }
};

} // namespace detail

} // namespace boost

#endif  // #ifndef BOOST_DETAIL_SP_COUNTED_BASE_IMPL_HPP_INCLUDED
