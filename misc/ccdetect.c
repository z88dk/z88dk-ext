/*
**  Tests for popular PC compilers and versions
*/

#include <stdio.h>

void main(void)         /* Actually, void main() is non-ANSI/ISO  */
{
      int version;

#if defined(__ZTC__)
 #ifdef __SC__
      printf("Symantec C++ ver. %x.%x\n", __SC__ >> 8, __SC__ & 0xff);
 #else
      printf("Zortech C++ ver. %x.%xr%x\n",
            __ZTC__ >> 8, (__ZTC__ >> 4) & 0xf, __ZTC__ & 0xf);
 #endif
#elif defined(__WATCOMC__)
      printf("Watcom C/C++ ver. %d.%d\n",
            __WATCOMC__ / 100, __WATCOMC__ % 100);
#elif defined(__TURBOC__)
      version = __TURBOC__;
      if (0x295 > version)
      {
            printf("Borland Turbo C ver. %x.%02x\n",
                  version >> 8, version & 0xff);
      }
      else if (0x400 <= version)
      {
            printf("Borland C++ ver. %x.%x\n",
                  (version >> 8) - 1, (version & 0xff) >> 4);
      }
      else if (0x297 > version)
            printf("Borland Turbo C++ ver. 1.%02x\n", version - 0x295);
      else  printf("Borland C++ ver. 2.%02x\n", version - 0x297);
#elif defined(_QC)
      printf("Microsoft Quick C ver. %d.%d\n", _QC / 100, _QC % 100);
#elif defined(_MSC_VER)
      printf("Microsoft C(/C++) ver. %d.%d\n",
            _MSC_VER / 100, _MSC_VER % 100);
#elif defined(__POWERC)
      printf ("MIX Power C ver. %d\n", __POWERC);
#elif defined(Z88DK_USES_SDCC)
  printf ("z88dk, SDCC\n");
#elif defined(__SMALLCDECL)
  printf ("z88dk, SCCZ80\n");
#else
      puts("Unknown compiler!");
#endif
}
