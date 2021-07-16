import pan_functions
from torch.utils.data import DataLoader
import torch
import numpy as np
import os
import argparse

# parse arguments
parser = argparse.ArgumentParser()

parser.add_argument("-n", "--n", dest = "n", default= 0, action='store', help="Number to show", type=int)
parser.add_argument("-f", "--f", dest = "nframes", default= 5, action='store', help="Number of frames", type=int)

args = parser.parse_args()

n = args.n
nframes = args.nframes

# Obtengo dir del dataset
root = './N-MNIST/Train'

# Obtengo los numeros ordenados
nros = os.listdir(root)
nros.sort()

# Agarro la carpeta del numero indicado
nro = nros[n]
files = os.listdir(os.path.join(root,nro))

# Agarro un sample del numero
sample = os.path.join(root,nro,files[0])

# Read sample
data, width, height =  pan_functions.read_dataset(sample)

print("Size of Data: " + str(data.shape))

# transform data.ts into frames
data.ts = np.round(data.ts / np.max(data.ts) * (nframes-1))

# Positive polarity is white pix
# Zero polarity is black pix

pan_functions.show_td(data, width, height, wait_delay=5000)