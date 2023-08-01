#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>

#define INF std::numeric_limits<float>::infinity()

int nearestRoundPow2(float edgeLength);
int nearestCeilPow2(float edgeLength);
int maxInt2(int a, int b);
int maxInt3(int a, int b, int c);
int maxIntIndex(int arr[]);

#endif // UTILITY_H
