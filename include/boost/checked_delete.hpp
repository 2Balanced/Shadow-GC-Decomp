#ifndef BOOST_CHECKED_DELETE_HPP_INCLUDED
#define BOOST_CHECKED_DELETE_HPP_INCLUDED

/* boost/checked_delete.hpp (boost 1.32): deletes x, refusing incomplete types. Copyright Peter
 * Dimov and Daniel Frey, Boost Software License 1.0 (see LICENSE_1_0.txt); trimmed to the parts the
 * game uses. */
namespace boost {

template <class T> inline void checked_delete(T *x) {
    typedef char type_must_be_complete[sizeof(T)];
    delete x;
}

template <class T> struct checked_deleter {
    typedef void result_type;
    typedef T *argument_type;

    void operator()(T *x) const {
        boost::checked_delete(x);
    }
};

} // namespace boost

#endif
