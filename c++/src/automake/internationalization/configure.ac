#                     -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# autoconf版本
AC_PREREQ([2.69])
# 初始化包信息，将会自动生成PACKAGE_NAME、PACKAGE_VERSION、PACKAGE_BUGREPORT宏
AC_INIT([app], [1.0], [fwdssg@gmail.com])
# 通过检测目录中必定存在的文件来判断目录是否存在
AC_CONFIG_SRCDIR([src/main.cpp])
# 生成config.h文件保存configure.ac定义的宏，此文件可被源文件包含
AC_CONFIG_HEADERS([config.h])
# 用来存储本地宏文件，.m4的文件都将被保存进此目录,acloacl命令会自动创建此目录
AC_CONFIG_MACRO_DIRS([m4])
# 用来存储一些辅助脚本文件
AC_CONFIG_AUX_DIR([build-aux])
# 初始化automake
AM_INIT_AUTOMAKE([subdir-objects -Wno-portability])
# 初始化国际化字符提取工具
IT_PROG_INTLTOOL([0.40.0])
# 初始化mo文件名，也是bindtextdomain搜索时候的domainname
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

AC_MSG_CHECKING([for native Win32])
case "$host_os" in
     *mingw*|*cygwin*)
        os_win32=yes
        gio_os=gio-windows-2.0
        red_target=Windows
        ;;
     *)
        os_win32=no
        gio_os=gio-unix-2.0
        red_target=Unix
        ;;
esac
AC_MSG_RESULT([$os_win32])
AM_CONDITIONAL([OS_WIN32],[test "$os_win32" = "yes"])

# Checks for programs.
AC_PROG_CXX
# 初始化要连接的obj目录
AC_CONFIG_LIBOBJ_DIR([src])

# Checks for libraries.
GLIB2_REQUIRED="2.36"
GLIB2_ENCODED_VERSION="GLIB_VERSION_2_36"
PKG_CHECK_MODULES(GLIB2, glib-2.0 >= $GLIB2_REQUIRED)
GLIB2_CFLAGS="$GLIB2_CFLAGS -DGLIB_VERSION_MIN_REQUIRED=$GLIB2_ENCODED_VERSION \
  -DGLIB_VERSION_MAX_ALLOWED=$GLIB2_ENCODED_VERSION"
AC_SUBST(GLIB2_CFLAGS)
AC_SUBST(GLIB2_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([setlocale])

# 设置运行configure后需要生成的文件，需要编写对应的.in文件，即需要生成Makefile，则必须存在Makefile.in文件，它们将被config.status使用用来生成Makefile
AC_CONFIG_FILES([ Makefile
    src/Makefile
    po/Makefile.in
])
AC_OUTPUT
