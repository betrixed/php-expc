make cdnl config.m4 for extension phiz


PHP_ARG_ENABLE([phiz],
  [whether to enable phiz support],
  [AS_HELP_STRING([--enable-phiz],
    [Enable phiz support])],
  [no])

if test "$PHP_PHIZ" != "no"; then
  AC_DEFINE(HAVE_PHIZ, 1, [ Have phiz support ])
  PHP_ADD_INCLUDE(phiz)
  PHP_NEW_EXTENSION(phiz, php_phiz.c phiz_exceptions.c  src/ucode8.c , $ext_shared)
  PHP_SUBST(PHIZ_SHARED_LIBADD)
  CFLAGS="$CFLAGS -std=c17"
fi
