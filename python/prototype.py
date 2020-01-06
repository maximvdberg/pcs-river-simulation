from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

colors_1 = ['green','gray','white']
cm_land = LinearSegmentedColormap.from_list('name', colors_1)
colors_2 = ['blue','black']
cm_water = LinearSegmentedColormap.from_list('name', colors_2)

def load_image(filename):
    img = Image.open(filename)
    img.load()
    data = np.asarray(img, dtype="int32")
    return data


def init():
    heightmap = load_image("test terrain/test Height Map (Merged).png")
    heightmap = heightmap[400:600,0:200]
    watermap = np.zeros(heightmap.shape,dtype="uint8")
    watermap[100:110,100:110] = 100

    return heightmap, watermap


def update(heightmap, watermap):
    watermap_new = np.zeros(watermap.shape)

    for i,row in enumerate(watermap):
        for j,waterlevel in enumerate(row):
            area = heightmap[max(0,i-1):i+2,max(0,j-1):j+2]
            # area = np.exp(heightmap[i,j] - area)
            area = area * (area < heightmap[i,j])

            area_water = 0 if not waterlevel else waterlevel * (area / sum(area.flatten()))
            watermap_new[max(0,i-1):i+2,max(0,j-1):j+2] += area_water

    return heightmap, watermap_new


def loop():
    heightmap, watermap = init()

    plt.ion()
    fig = plt.figure()
    ax = fig.add_subplot(111)

    for t in np.arange(1000):
        ax.imshow(heightmap, cmap=cm_land , vmin=np.iinfo("uint16").min, vmax=np.iinfo("uint16").max, interpolation='nearest')

        wm_plot = np.where(watermap != 0)
        ax.scatter(wm_plot[1],wm_plot[0], c=watermap[wm_plot], cmap='Blues', alpha=0.5)

        fig.canvas.draw()
        fig.canvas.flush_events()

        heightmap, watermap = update(heightmap, watermap)
