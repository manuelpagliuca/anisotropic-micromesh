#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <iostream>
#include <GLM/gtx/string_cast.hpp>
#include "Mesh.h"

#define PRINT_VECTOR(x) std::cout << #x << ": " << glm::to_string(x) << std::endl;

#define INF std::numeric_limits<float>::infinity()

int nearestRoundPow2(float edgeLength);
int nearestCeilPow2(float edgeLength);

int maxInt2(int a, int b);
int maxInt3(int a, int b, int c);
int maxIntIndex(int arr[]);
int minInt2(int a, int b);
int minInt3(int a, int b, int c);

float maxFloat2(float a, float b);
float maxFloat3(float a, float b, float c);
float minFloat2(float a, float b);
float minFloat3(float a, float b, float c);

double maxDouble2(double a, double b);
double maxDouble3(double a, double b, double c);
double minDouble2(double a, double b);
double minDouble3(double a, double b, double c);

#endif // UTILITY_H
