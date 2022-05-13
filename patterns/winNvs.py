import pan_functions
from torch.utils.data import DataLoader
import torch
import numpy as np
import os
import argparse
import cv2

# parse arguments
parser = argparse.ArgumentParser()

parser.add_argument("-n", "--n", dest = "n", default= 0, action='store', help="Number to show", type=int)
parser.add_argument("-w", "--w", dest = "wevents", default= 100, action='store', help="Number of events", type=int)
parser.add_argument("-f", "--f", dest = "nframes", default= 5, action='store', help="Number of frames", type=int)
parser.add_argument("-wd", "--wd", dest = "wdelay", default= 5000, action='store', help="Wait Delay between frames", type=int)
parser.add_argument("-s", "--s", dest = "nSample", default= 0, action='store', help="Number Sample", type=int)

args = parser.parse_args()

n = args.n
wevents = args.wevents
nframes = args.nframes
wdelay = args.wdelay
nSample = args.nSample

# Obtengo dir del dataset
root = './N-MNIST/Train'

# Obtengo los numeros ordenados
nros = os.listdir(root)
nros.sort()

# Agarro la carpeta del numero indicado
nro = nros[n+1]
files = os.listdir(os.path.join(root,nro))

# Agarro el sample indicado del numero
sample = os.path.join(root,nro,files[nSample])

# Read sample
data, width, height =  pan_functions.read_dataset(sample)

# transform data into windows of Wevents:
# Sort events by ts
data = np.sort(data, order='ts')
# Collapse Wevents to the same ts:
time = 0
data[0].ts = 0
for i in range(1, data.shape[0]):
	if i % wevents == 0:
		time+=1
	data[i].ts = time

print(data.shape)
# Drop last events
data = data[:wevents * nframes]
print(data.shape)
# transform data.ts into frames
#data.ts = np.round(data.ts / np.max(data.ts) * (nframes-1))

# Positive polarity is white pix
# Zero polarity is black pix

# Displays the TD events (change detection ATIS or DVS events) waitDelay: milliseconds
frame_length = 1
t_max = data.ts[-1]
frame_start = data[0].ts
frame_end = data[0].ts + frame_length
td_img = np.ones((height, width), dtype=np.uint8)
while frame_start <= t_max:
    frame_data = data[(data.ts >= frame_start) & (data.ts < frame_end)]
    if frame_data.size > 0:
        td_img.fill(128)

        #with timer.Timer() as em_playback_timer:
        for datum in np.nditer(frame_data):
            td_img[datum['y'].item(0), datum['x'].item(0)] = datum['p'].item(0)
        #print 'prepare td frame by iterating events took %s seconds'
        #%em_playback_timer.secs

        td_img = np.piecewise(td_img, [td_img == 0, td_img == 1, td_img == 128], [0, 255, 128])
        cv2.imshow('img', td_img)
        cv2.waitKey(wdelay)

    frame_start += frame_length
    frame_end += frame_length

cv2.destroyAllWindows()