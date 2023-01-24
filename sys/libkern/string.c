#include <string.h>

void 
memcpy(void* dst, const void* src, size_t n)
{
  for (size_t i = 0; i < n; ++i)
  {
    ((char*)dst)[i] = ((char*)src)[i];
  }
}


size_t 
strlen(const char* str)
{
  size_t slen = 0;
  while (str[slen++]);
  return slen - 1;
}
