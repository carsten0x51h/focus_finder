import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import math

from libtiff import TIFF

def gkern(kernlen=21, sigma=1, factor=65535):
    """Returns a 2D Gaussian kernel array."""
    gkern1d = signal.gaussian(kernlen, std=sigma).reshape(kernlen, 1)
    gkern2d = factor*np.outer(gkern1d, gkern1d)
    return gkern2d


imageDimensionPx = 121


imageDimensions = [50, 51, 100, 101]
sigmaValues = [1, 2, 3, 4, 5]
factorValues = ["norm", 32767, 65535]


for dim in imageDimensions:
    for sigma in sigmaValues:
        for factor in factorValues:
            if factor == "norm":
                factor = 1 / (2.0*math.pi*sigma*sigma);
                normStr = "_normalized";
                kern = gkern(dim, sigma, factor).astype(np.float32) #.astype('float')
            else:
                normStr = "_factor_" + str(factor);
                kern = gkern(dim, sigma, factor).astype('uint16')

            oddEvenStr = '_even_' if dim % 2 == 0 else '_odd_'
            tiff = TIFF.open('gaussian_2d_sigma' + str(sigma) + normStr + oddEvenStr + str(dim) + 'x' + str(dim) + '.tiff', mode='w')
            tiff.write_image(kern)
            tiff.close()
