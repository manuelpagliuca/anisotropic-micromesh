#include "Utility.h"

int nearestRoundPow2(float edgeLength)
{
  return max(int(round(log2(edgeLength))), 0);
}

int nearestCeilPow2(float edgeLength)
{
  return int(ceil(log2(edgeLength)));
}

int maxInt2(int a, int b)
{
  return (a >= b) ? a : b;
}

int maxInt3(int a, int b, int c)
{
  return maxInt2(maxInt2(a, b), c);
}

int maxIntIndex(int arr[])
{
  int maxIdx = 0;
  int max = arr[0];
  size_t length = sizeof(arr) / sizeof(arr[0]);

  for (size_t i = 0; i < length; i++) {
    if (arr[i] > max) {
      max = arr[i];
      maxIdx = int(i);
    }
  }

  return maxIdx;
}
