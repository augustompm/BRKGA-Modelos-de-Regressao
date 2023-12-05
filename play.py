#!/bin/bash


q1 = 8.20384E+18
r =	6
Ef = 2.04817E+27

pi = 3.1415926535
epsilon = 0.08854 #\epsilon _{0}=8,854187817\cdot 10^{{-12}}{\mathrm {C^{2}N^{{-1}}m^{{-2}}}},

res = q1 / (4*pi*epsilon*pow(10,-10)*r*r)

print(res)
print(Ef)
