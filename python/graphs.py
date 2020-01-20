import numpy as np
import matplotlib.pyplot as plt



σ = lambda x: 1 / (1 + np.exp(-x))

cor_act = 10
cor_lim = 0.7
cor_slope = 2
a = cor_slope * cor_act
cor = lambda x: (σ(cor_slope * x - a) - σ(-a)) / (1 - σ(-a)) * cor_lim

t = np.linspace(0, 20,2000)
plt.plot(t,cor(t))
plt.show()
