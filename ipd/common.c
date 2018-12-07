#include "common.h"

unsigned short computeChecksum(unsigned short *buffer, unsigned short size)
{
  // the size must be in 16bit words
  unsigned long sum = 0;

  for(int i = 0; i < size; i++)
  {
    sum += *buffer++;
    if (sum & 0xFFFF0000)
    {
      sum &= 0xFFFF;
      sum++;
    }
  }
  return ~(sum & 0xFFFF);
}
