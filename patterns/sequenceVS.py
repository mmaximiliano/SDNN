import pan_functions
from torch.utils.data import DataLoader
import torch
import numpy as np
import os
import argparse
import cv2

# parse arguments
parser = argparse.ArgumentParser()

parser.add_argument("-f", "--f", dest = "fname", default= None, action='store', help="Name of the file", type=str)
parser.add_argument("-w", "--w", dest = "wait_delay", default= 2000, action='store', help="Wait delay between frames", type=int)

args = parser.parse_args()

fname = args.fname
wait_delay = args.wait_delay

# Obtengo dir del dataset
root = './sequences/'

# Obtengo la data de la secuencia
sequence = np.load(root + fname)
data = np.rec.array(sequence, dtype=[('x', np.uint16), ('y', np.uint16), ('p', np.uint8), ('ts', np.uint64)])
width = data.x.max() + 1
height = data.y.max() + 1

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
        cv2.waitKey(wait_delay)

    frame_start += frame_length
    frame_end += frame_length

cv2.destroyAllWindows()