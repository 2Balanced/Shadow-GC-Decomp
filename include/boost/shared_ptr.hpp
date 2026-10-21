#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#define BOOST_SHARED_PTR_HPP_INCLUDED

//
//  shared_ptr.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002, 2003 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/shared_ptr.htm for documentation.
//
//  Trimmed from boost 1.32 (shared_ptr.hpp with detail/shared_count.hpp) to the configuration
//  the game was built with: no threads and no exceptions (allocation failure and expired locks go
//  through the user-defined boost::throw_exception), a plain operator bool, reset() declared out
//  of line, dynamic_pointer_cast as the only cast, and no auto_ptr or stream support. Members
//  are public because CodeWarrior lacks member template friends
//  (BOOST_NO_MEMBER_TEMPLATE_FRIENDS).
//
//  static_pointer_cast and its constructor are compiled only where BOOST_SP_STATIC_POINTER_CAST is
//  defined before this header (Game/WeaponVacuumPod.cpp): every template declaration advances
//  CodeWarrior's @N counter, so declaring them everywhere would renumber the other units' locals.
//  For the same reason the constructor taking a deleter is compiled only where
//  BOOST_SP_DELETER_CONSTRUCTOR is defined (Game/EffectPJS.cpp), and the converting assignment
//  only where BOOST_SP_CONVERTING_ASSIGNMENT is defined (Game/DevilDoom.cpp), and reset(Y *)
//  only where BOOST_SP_RESET_POINTER is defined (Game/WeaponVacuumBase.cpp).
//

#include <boost/config.hpp>
#include <boost/checked_delete.hpp>
#include <boost/throw_exception.hpp>
#include <boost/detail/sp_counted_base_impl.hpp>

#include <algorithm>            // for std::swap
#include <new>                  // for std::bad_alloc
#include <typeinfo>

namespace boost
{

template<class T> class weak_ptr;

namespace detail
{

struct static_cast_tag {};
struct dynamic_cast_tag {};

class weak_count;

class shared_count
{
private:

    sp_counted_base * pi_;

    friend class weak_count;

public:

    shared_count(): pi_(0) // nothrow
    {
    }

    template<class P, class D> shared_count(P p, D d): pi_(0)
    {
        pi_ = new sp_counted_base_impl<P, D>(p, d);

        if(pi_ == 0)
        {
            d(p); // delete p
            boost::throw_exception(std::bad_alloc());
        }
    }

    ~shared_count() // nothrow
    {
        if(pi_ != 0) pi_->release();
    }

    shared_count(shared_count const & r): pi_(r.pi_) // nothrow
    {
        if(pi_ != 0) pi_->add_ref_copy();
    }

    explicit shared_count(weak_count const & r); // throws bad_weak_ptr when r.use_count() == 0

    shared_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if(tmp != pi_)
        {
            if(tmp != 0) tmp->add_ref_copy();
            if(pi_ != 0) pi_->release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(shared_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool unique() const // nothrow
    {
        return use_count() == 1;
    }

    void * get_deleter(std::type_info const & ti) const
    {
        return pi_? pi_->get_deleter(ti): 0;
    }
};

class weak_count
{
private:

    sp_counted_base * pi_;

    friend class shared_count;

public:

    weak_count(): pi_(0) // nothrow
    {
    }

    weak_count(shared_count const & r): pi_(r.pi_) // nothrow
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    weak_count(weak_count const & r): pi_(r.pi_) // nothrow
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    ~weak_count() // nothrow
    {
        if(pi_ != 0) pi_->weak_release();
    }

    weak_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->weak_add_ref();
        if(pi_ != 0) pi_->weak_release();
        pi_ = tmp;

        return *this;
    }

    weak_count & operator= (weak_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->weak_add_ref();
        if(pi_ != 0) pi_->weak_release();
        pi_ = tmp;

        return *this;
    }

    void swap(weak_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }
};

inline shared_count::shared_count(weak_count const & r): pi_(r.pi_)
{
    if(pi_ != 0)
    {
        pi_->add_ref_lock();
    }
    else
    {
        boost::throw_exception(boost::bad_weak_ptr());
    }
}

// The overload for enable_shared_from_this (used by EggLastMech::EggLastMech) is declared in
// boost/enable_shared_from_this.hpp; every other pointer takes the plain overload.

inline void sp_enable_shared_from_this( shared_count const & /*pn*/, ... )
{
}

} // namespace detail


//
//  shared_ptr
//
//  An enhanced relative of scoped_ptr with reference counted copy semantics.
//  The object pointed to is deleted when the last shared_ptr pointing to it
//  is destroyed or reset.
//

template<class T> class shared_ptr
{
private:

    typedef shared_ptr<T> this_type;

public:

    typedef T element_type;
    typedef T value_type;
    typedef T * pointer;
    typedef T & reference;

    shared_ptr(): px(0), pn() // never throws in 1.30+
    {
    }

    template<class Y>
    explicit shared_ptr(Y * p): px(p), pn(p, checked_deleter<Y>()) // Y must be complete
    {
        detail::sp_enable_shared_from_this( pn, p, p );
    }

#ifdef BOOST_SP_DELETER_CONSTRUCTOR
    //
    // Requirements: D's copy constructor must not throw
    //
    // shared_ptr will release p by calling d(p)
    //

    template<class Y, class D> shared_ptr(Y * p, D d): px(p), pn(p, d)
    {
        detail::sp_enable_shared_from_this( pn, p, p );
    }
#endif

//  generated copy constructor, assignment, destructor are fine...

    template<class Y>
    explicit shared_ptr(weak_ptr<Y> const & r): pn(r.pn) // may throw
    {
        // it is now safe to copy r.px, as pn(r.pn) did not throw
        px = r.px;
    }

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r): px(r.px), pn(r.pn) // never throws
    {
    }

#ifdef BOOST_SP_STATIC_POINTER_CAST
    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::static_cast_tag): px(static_cast<element_type *>(r.px)), pn(r.pn)
    {
    }
#endif

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::dynamic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0) // need to allocate new counter -- the cast failed
        {
            pn = detail::shared_count();
        }
    }

#ifdef BOOST_SP_CONVERTING_ASSIGNMENT
    template<class Y>
    shared_ptr & operator=(shared_ptr<Y> const & r) // never throws
    {
        px = r.px;
        pn = r.pn; // shared_count::op= doesn't throw
        return *this;
    }
#endif

    void reset(); // never throws; out of line in the game

#ifdef BOOST_SP_RESET_POINTER
    template<class Y> void reset(Y * p) // Y must be complete
    {
        this_type(p).swap(*this);
    }
#endif

    reference operator* () const // never throws
    {
        return *px;
    }

    T * operator-> () const // never throws
    {
        return px;
    }

    T * get() const // never throws
    {
        return px;
    }

    // implicit conversion to "bool"

    operator bool () const // never throws
    {
        return px != 0;
    }

    bool operator! () const // never throws
    {
        return px == 0;
    }

    bool unique() const // never throws
    {
        return pn.unique();
    }

    long use_count() const // never throws
    {
        return pn.use_count();
    }

    void swap(shared_ptr<T> & other) // never throws
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    T * px;                     // contained pointer
    detail::shared_count pn;    // reference counter

};  // shared_ptr

template<class T, class U> inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() == b.get();
}

#ifdef BOOST_SP_NOT_EQUAL
// boost 1.32's operator!=, compiled only where BOOST_SP_NOT_EQUAL is defined before this header
// (Game/VehicleColliMove.cpp), for the same reason as the casts below.
template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() != b.get();
}
#endif

#ifdef BOOST_SP_STATIC_POINTER_CAST
template<class T, class U> shared_ptr<T> static_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::static_cast_tag());
}
#endif

template<class T, class U> shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::dynamic_cast_tag());
}

} // namespace boost

#endif  // #ifndef BOOST_SHARED_PTR_HPP_INCLUDED
