import numpy as np
import matplotlib.pyplot as plt



σ = lambda x: 1 / (1 + np.exp(-x))

cor_act   = 0.30
cor_lim   = 0.05
cor_slope = 30.0
a = cor_slope * cor_act
cor = lambda x: (σ(cor_slope * x - a) - σ(-a)) / (1 - σ(-a)) * cor_lim

t = np.linspace(0, 0.5,2000)
plt.plot(t,cor(t))
plt.show()




sed_act   = 0.03
sed_lim   = 0.95
sed_slope = 150.0
a = sed_slope * sed_act
sed = lambda x: sed_lim - (σ(sed_slope * x - a) - σ(-a)) / (1 - σ(-a)) * sed_lim

t = np.linspace(0, 0.5,2000)
plt.plot(t,sed(t))
plt.show()
