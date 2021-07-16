import pan_functions
from torch.utils.data import DataLoader
import torch
import numpy as np
import os
import argparse
import cv2

# parse arguments
parser = argparse.ArgumentParser()

parser.add_argument("-he", "--he", dest = "height", default= None, action='store', help="height", type=int)
parser.add_argument("-w", "--w", dest = "width", default= 2000, action='store', help="width", type=int)
parser.add_argument("-d", "--d", dest = "depth", default= 2000, action='store', help="depth", type=int)
parser.add_argument("-f", "--f", dest = "fname", default= None, action='store', help="Name of the file", type=str)
parser.add_argument("-wd", "--wd", dest = "wait_delay", default= 2000, action='store', help="Wait delay between frames", type=int)

args = parser.parse_args()

height = args.height
width = args.width
depth = args.depth
fname = args.fname
wait_delay = args.wait_delay

# Obtengo dir del dataset
root = '../cleanDSNN/results/'

# Obtengo la data de la secuencia
sequence = torch.load(root + fname)
sequence = sequence.numpy()

# Convierto la secuencia a un frame
sequence = np.transpose(sequence)
sequence = np.reshape(sequence, (height, width, depth, sequence.shape[1]))

# Obtengo los mapas de activacion
data = []
for i in range(depth):
    activation_map = np.squeeze(sequence[:,:,i:i+1, :])
	# Obtengo los indices de los spikes
    activation_map = np.argwhere(activation_map>0)
	# Le doy formato para mostrar el frame
    for m in activation_map:
        data.append( (m[0], m[1], 1, m[2]) )

data = np.array(data, np.dtype('uint16, uint16, uint8, uint64'))

# Creo structured array y lo ordeno por ts
data = np.rec.array(data, dtype=[('x', np.uint16), ('y', np.uint16), ('p', np.uint8), ('ts', np.uint64)])
data = np.sort(data, order='ts')

# Displays the TD events (change detection ATIS or DVS events) waitDelay: milliseconds
frame_length = 10
t_max = data.ts[-1]
frame_start = data[0].ts
frame_end = data[0].ts + frame_length
td_img = np.ones((height, width), dtype=np.uint8)
while frame_start < t_max:
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
        cv2.waitKey(wait_delay)

    frame_start = frame_end + 1
    frame_end = frame_end + frame_length + 1

cv2.destroyAllWindows()