# Plotting of logistic activation functions.
#
# @file activation_function.py
# @author Jurriaan van den Berg
# @author Maxim van den Berg
# @author Melvin Seitner
# @date 20-01-2020

import numpy as np
import matplotlib.pyplot as plt

σ = lambda x: 1 / (1 + np.exp(-x))

# Base logistic function.
t = np.linspace(-2*np.pi, 2*np.pi,2000)
plt.figure(figsize=(12,6))
plt.plot(t,σ(t), color='orange')
plt.ylabel("Probability")
plt.xlabel("System state (p.d.u.)")
plt.title("The base logistic function")
plt.gcf().subplots_adjust(bottom=0.25,left=0.25)
plt.grid()
plt.show()

# Erosion activation function.
ero_act   = 0.00
ero_lim   = 1.0
ero_slope = 1000.0
a = ero_slope * ero_act
ero = lambda x: (σ(ero_slope * x - a) - σ(-a)) / (1 - σ(-a)) * ero_lim

t = np.linspace(0, 0.02,2000)
plt.plot(t,ero(t))
plt.show()

# Sedimentation activation function.
sed_act   = 0.00
sed_lim   = 0.005
sed_slope = 100.0
a = sed_slope * sed_act
sed = lambda x: sed_lim - (σ(sed_slope * x - a) - σ(-a)) / (1 - σ(-a)) * sed_lim

t = np.linspace(0, .1,2000)
plt.plot(t,sed(t))
plt.show()





