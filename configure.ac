# Process this file with autoconf to produce a configure script.
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(iris, 0.9)
AC_CONFIG_SRCDIR([include/SDLEvent.h])
AC_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS([gethostbyname inet_ntoa memset select socket sqrt strcasecmp])

dnl Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

dnl With fink, SDL uses SDL_CFLAGS=-I/sw/include/SDL wich causes problems with UO Iris
dnl So we just get rid of the last SDL path component when using fink-sdl
case $SDL_CFLAGS in
*sw*)
	SDL_CFLAGS="-I/sw/include -D_THREAD_SAFE"
;;
esac

CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"
dnl echo "SDL Flags : $SDL_CFLAGS"
dnl echo "SDL Libs : $SDL_LIBS"

case $target in
*darwin*)
	is_mac="yes"
    dnl CFLAGS="$CFLAGS -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -g"
	SYS_GL_LIBS="-framework OpenGL -lz"
	GL_DIR="OpenGL"
;;
*)
	GL_DIR="GL"
;;
esac

AC_CHECK_LIB(SDL_image, IMG_LoadPNG_RW, , AC_MSG_ERROR([
*** Unable to find SDL_image libary with PNG support
(http://www.libsdl.org/projects/SDL_image/)
]))

AC_CHECK_LIB(SDL_mixer, Mix_CloseAudio, , AC_MSG_ERROR([
*** Unable to find SDL_mixer libary
(http://www.libsdl.org/projects/SDL_mixer/)
]))

AC_CHECK_LIB(SDL_net, SDLNet_Init, , AC_MSG_ERROR([
*** Unable to find SDL_net libary
(http://www.libsdl.org/projects/SDL_net/)
]))

AC_CHECK_LIB(SDL_ttf, TTF_Init, , AC_MSG_ERROR([
*** Unable to find SDL_ttf libary
(http://www.libsdl.org/projects/SDL_ttf/)
]))

MATHLIB="-lm"
AC_PATH_X
AC_PATH_XTRA
if ! (test $is_mac = yes); then
	if test x$have_x = xyes; then
	    CFLAGS="$CFLAGS $X_CFLAGS"
	    SYS_GL_LIBS="$X_LIBS -lGL"
	else
	    SYS_GL_LIBS="-lGL"
	fi
fi

AC_MSG_CHECKING(for OpenGL support)
HAVE_OPENGL=no
AC_TRY_COMPILE([
 #include <$GL_DIR/gl.h>
],[
],[
HAVE_OPENGL=yes
])
AC_MSG_RESULT($HAVE_OPENGL)
if test x$HAVE_OPENGL = xyes; then
    CFLAGS="$CFLAGS -DHAVE_OPENGL"
    GL_LIBS="$SYS_GL_LIBS"
else
    AC_MSG_ERROR([could not find GL/gl.h])
    GL_LIBS=""
fi

AC_MSG_CHECKING(for GLU support)
HAVE_GLU=no
AC_TRY_COMPILE([
 #include <$GL_DIR/glu.h>
],[
],[
HAVE_GLU=yes
])
AC_MSG_RESULT($HAVE_GLU)

if test x$HAVE_GLU = xyes; then
	if !( test $is_mac = "yes"); then
    	GL_LIBS="$SYS_GL_LIBS -lGLU"
	fi
else
    AC_MSG_ERROR([could not find GL/glu.h])
    GL_LIBS=""
fi

CXXFLAGS="$CFLAGS"
AC_SUBST(GL_LIBS)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
