#!/bin/bash


q1 = 8.20384E+18
r =	6
Ef = 2.04817E+27

pi = 3.1415926535
epsilon = 0.08854 #\epsilon _{0}=8,854187817\cdot 10^{{-12}}{\mathrm {C^{2}N^{{-1}}m^{{-2}}}},

res = q1 / (4*pi*epsilon*pow(10,-10)*r*r)

print(Ef)
print("calculado:", res)

# ================================
# Eq 6

pi = 3.1415926535

d = 3.84E+05
t = 7.43E+01 # saida 
G = 0.667408 #m^3 * kg^-1 * s^-2
m1 = 5.972E+4 # kg
m2 = 7.34E+2 # kg

c4 = 10000000000

import math

t_out =2 * pi * math.sqrt(d * d * d)/ math.sqrt(G*c4*(m1+m2))

print( t)
print("calculado:",t_out)

