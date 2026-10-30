/* boost's error handler (boost/throw_exception.hpp). The game is built without exceptions
 * (BOOST_NO_EXCEPTIONS), so boost leaves throw_exception to the program, which defines it as doing
 * nothing; the definition is a unit of its own (0x801EC2B4, four bytes after
 * Game/TextureUVAnimManagerTask.cpp). File name inferred. */
#include <boost/throw_exception.hpp>

namespace boost {
void throw_exception(std::exception const &e) {}
} // namespace boost
