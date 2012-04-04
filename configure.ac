dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(gramd, 1.0, [http://www.jovislab.com/gramd])

AC_CANONICAL_SYSTEM
AC_PROG_CXX
AC_PROG_LIBTOOL 
AC_PROG_INSTALL
AM_INIT_AUTOMAKE()

AC_ARG_WITH([libgram],
  [AS_HELP_STRING([--with-libgram],
    [location of the libgram library files])],
  [LIBGRAM_CFLAGS="-I$withval/include"],
  [LIBGRAM_CFLAGS=''])
AC_SUBST([LIBGRAM_CFLAGS])

AC_LANG_PUSH(C++)
AC_CHECK_HEADER(tr1/unordered_map,[AC_DEFINE([LIBGRAM_HAVE_TR1],[],["Have tr1"])],[])
AC_CHECK_HEADER(unordered_map,[AC_DEFINE([LIBGRAM_HAVE_CXX0X],[],["Have C++0x"])],[])
AC_CHECK_HEADER(boost/unordered_map.hpp,[AC_DEFINE([LIBGRAM_HAVE_BOOST],[],["Have boost::unordered_map"])],[])
AC_LANG_POP

BOOST_REQUIRE([1.34])
BOOST_THREADS([mt])
BOOST_SYSTEM([mt])
BOOST_SERIALIZATION([mt])
BOOST_WSERIALIZATION([mt])
BOOST_PROGRAM_OPTIONS([mt])

CPPFLAGS="$CPPFLAGS $LIBGRAM_CFLAGS"

AC_LANG_PUSH([C++])
#	boost/serialization/collections_load_imp.hpp \
#	boost/serialization/collections_save_imp.hpp \
#	boost/archive/binary_oarchive.hpp \

#AC_CHECK_HEADERS([boost/program_options.hpp \
#	boost/unordered_map.hpp \
#	boost/progress.hpp \
#	boost/serialization/serialization.hpp \
#	boost/archive/binary_iarchive.hpp \
#	boost/asio.hpp \
#	boost/thread.hpp \
#	boost/shared_ptr.hpp \
#	boost/bind.hpp], [], AC_MSG_ERROR([Missing required BOOST header files.]))
AC_CHECK_HEADERS([libgram/solver.h], [], AC_MSG_ERROR([Missing required libgram header files. Download from http://www.jovislab.com/libgram and install.]))
AC_LANG_POP

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

