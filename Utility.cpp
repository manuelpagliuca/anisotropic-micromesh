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

int minInt2(int a, int b)
{
  return (a < b) ? a : b;
}

int minInt3(int a, int b, int c)
{
  return minInt2(minInt2(a, b), c);
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

float maxFloat2(float a, float b)
{
  return (a >= b) ? a : b;
}

float maxFloat3(float a, float b, float c)
{
  return maxFloat2(maxFloat2(a, b), c);
}

float minFloat2(float a, float b)
{
  return (a < b) ? a : b;
}

float minFloat3(float a, float b, float c)
{
  return minFloat2(minFloat2(a, b), c);
}

double maxDouble2(double a, double b)
{
  return (a >= b) ? a : b;
}

double maxDouble3(double a, double b, double c)
{
  return maxDouble2(maxDouble2(a, b), c);
}

double minDouble2(double a, double b)
{
  return (a < b) ? a : b;
}

double minDouble3(double a, double b, double c)
{
  return minDouble2(minDouble2(a, b), c);
}
