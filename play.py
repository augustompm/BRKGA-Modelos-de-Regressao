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


### =========== 8



pi = 3.1415926535

theta = 0.59

import math

t = 0.47478722
t_out = math.exp(((pi - pi) - (theta*theta))/2) / (2*pi)
t_out2 = math.exp(-(theta*theta)/2) / math.sqrt(2*pi)

print( t)
print("calculado:",t_out)
print("calculado2:",t_out2)

# eq 8 ...

from scipy.stats import norm

def custom_pdf(theta):
    return norm.pdf(theta)

# Example usage
theta_value = 0.82
result = custom_pdf(theta_value)

print("correct:", result)
