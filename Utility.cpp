#include "Utility.h"

int nearestRoundPow2(float edgeLength)
{
  return int(round(abs(log2(edgeLength))));
}

int nearestCeilPow2(float edgeLength)
{
  return int(ceil(abs(log2(edgeLength))));
}

int maxInt2(int a, int b)
{
  if (a >= b) return a;
  else return b;
}

int maxInt3(int a, int b, int c)
{
  if (a >= b)
  {
    if (a >= c) return a;
    else return c;
  }
  else
  {
    if (b >= c) return b;
    else return c;
  }
}

int maxIntIndex(int arr[])
{
  int maxIdx = 0;
  int max = arr[0];
  size_t length = sizeof(arr) / sizeof(arr[0]);

  for (int i = 0; i < length; i++)
  {
    if (arr[i] > max)
    {
      max = arr[i];
      maxIdx = i;
    }
  }

  return maxIdx;
}
