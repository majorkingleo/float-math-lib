
dnl @synopsis CXX_FLAGS_CHECK [compiler flags]                                        
dnl @summary check whether compiler supports given C++ flags or not                   
AC_DEFUN([CXX_FLAGS_CHECK],                                                            
[dnl                                                                                  
  AC_MSG_CHECKING([if $CXX supports $1])
  AC_LANG_PUSH([C++])
  ac_saved_cxxflags="$CXXFLAGS"                                                       
  CXXFLAGS="-Werror $1"                                                               
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],                                            
    [AC_MSG_RESULT([yes])],                                                           
    [AC_MSG_ERROR([no])]                                                              
  )                                                                                   
  CXXFLAGS="$ac_saved_cxxflags"                                                       
  AC_LANG_POP([C++])
])


AC_INIT(float, 0.2)
AC_CONFIG_SRCDIR([src/float.cpp])
AM_INIT_AUTOMAKE([subdir-objects -Wall])
AC_PROG_CC
AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_INSTALL
CXX_FLAGS_CHECK([-std=c++11])

AC_OUTPUT
