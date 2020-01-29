import numpy as np
import matplotlib.pyplot as plt



σ = lambda x: 1 / (1 + np.exp(-x))

t = np.linspace(-2*np.pi, 2*np.pi,2000)
plt.figure(figsize=(12,6))
plt.plot(t,σ(t), color='orange')
plt.ylabel("Probability")
plt.xlabel("System state (p.d.u.)")
plt.title("The base logistic function")
plt.gcf().subplots_adjust(bottom=0.25,left=0.25)
plt.grid()
plt.show()

cor_act   = 1.0
cor_lim   = 0.5
cor_slope = 6.0
a = cor_slope * cor_act
cor = lambda x: (σ(cor_slope * x - a) - σ(-a)) / (1 - σ(-a)) * cor_lim

t = np.linspace(0, 0.5,2000)
plt.plot(t,cor(t))
plt.show()




sed_act   = 0.05
sed_lim   = 0.000
sed_slope = 160.0
a = sed_slope * sed_act
sed = lambda x: sed_lim - (σ(sed_slope * x - a) - σ(-a)) / (1 - σ(-a)) * sed_lim

t = np.linspace(0, .2,2000)
plt.plot(t,sed(t))
plt.show()





