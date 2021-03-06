# Process this file with autoconf to produce a configure script.
AC_INIT([libatomic_ops],[7.3alpha1])

AC_PREREQ(2.64)
AC_CANONICAL_TARGET([])
AC_CONFIG_SRCDIR(src/atomic_ops.c)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign dist-bzip2 nostdinc])
AM_MAINTAINER_MODE

AM_CONFIG_HEADER(src/config.h)

# Checks for programs.
AM_PROG_CC_C_O
AM_PROG_AS
LT_INIT([disable-shared])

# Checks for functions.
AC_FUNC_MMAP

# Determine PIC flag, adjust default CFLAGS
need_asm=false
PICFLAG=
AC_MSG_CHECKING(Determining PIC compiler flag)
if test "$GCC" = yes; then
  case "$host" in
    *-*-solaris*)
      AC_MSG_RESULT(-fPIC)
      PICFLAG=-fPIC
      # Workaround: at least GCC 3.4.6 does not define this macro.
      CFLAGS="$CFLAGS -D__PIC__=1"
      ;;
    *-*-cygwin* | *-*-mingw*)
      # Cygwin and Mingw[-w32/64] do not need -fPIC.
      ;;
    *)
      AC_MSG_RESULT(-fPIC)
      PICFLAG=-fPIC
      ;;
  esac
  # Output all warnings.
  CFLAGS="$CFLAGS -Wall -Wextra"
else
  case "$host" in
    *-*-hpux*)
      AC_MSG_RESULT("+Z")
      PICFLAG="+Z"
      CFLAGS="$CFLAGS +O2 -mt"
      ;;
    *-*-solaris*)
      AC_MSG_RESULT(-Kpic)
      PICFLAG=-Kpic
      CFLAGS="$CFLAGS -O"
      need_asm=true
      ;;
    *-*-linux*)
      AC_MSG_RESULT(-fPIC)
      PICFLAG=-fPIC
      # Any Linux compiler had better be gcc compatible.
      ;;
    *)
      AC_MSG_RESULT("<none>")
      ;;
  esac
fi
CFLAGS="$CFLAGS -DNDEBUG"
AC_SUBST(PICFLAG)
AC_SUBST(DEFS)

AH_TEMPLATE([_PTHREADS], [Indicates the use of pthreads (NetBSD).])

THREADDLLIBS=
have_pthreads=true
## Libraries needed to support threads.
AC_CHECK_LIB(pthread, pthread_self, THREADDLLIBS="-lpthread",,)
AC_DEFINE(_REENTRANT, 1, [Required define if using POSIX threads.])
case "$host" in
    *-*-netbsd*)
      AC_DEFINE(_PTHREADS)
      ;;
    *-*-openbsd* | *-*-kfreebsd*-gnu | *-*-dgux*)
      THREADDLLIBS=-pthread
      ;;
    *-*-cygwin* | *-*-darwin*)
      THREADDLLIBS=
      ;;
    *-*-mingw*)
      THREADDLLIBS=
      have_pthreads=false
      ;;
esac
AC_SUBST(THREADDLLIBS)

AM_CONDITIONAL(HAVE_PTHREAD_H, test x$have_pthreads = xtrue)
AM_CONDITIONAL(NEED_ASM, test x$need_asm = xtrue)

AC_CONFIG_FILES([
        Makefile
        doc/Makefile
        src/Makefile
        tests/Makefile
        pkgconfig/atomic_ops.pc
        pkgconfig/atomic_ops-uninstalled.pc ])

AC_CONFIG_COMMANDS([default],[[]],[[
PICFLAG="${PICFLAG}"
CC="${CC}"
DEFS="${DEFS}"
]])
AC_OUTPUT
