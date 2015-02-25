#ifndef MAC_OS_X
  #include <error.h>
#endif
#ifdef MAC_OS_X
  #define userreal_c(a,b,c,d,e) userreal_c(a, (fint *) b, (fint *) c, d,e)
#endif
