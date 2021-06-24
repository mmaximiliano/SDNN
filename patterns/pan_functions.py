# -*- coding: utf-8 -*-
"""
Created on Wed Jun 23 14:45:19 2021

@author: pnegr
"""

import numpy as np
import cv2
from torch.utils.data import Dataset
import os
import numpy as np
from random import shuffle, randint
import random
import torch
#############################################################################
def seed_torch(seed=1029):
    random.seed(seed)
    os.environ['PYTHONHASHSEED'] = str(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed(seed)
    torch.backends.cudnn.deterministic = True

def show_td(data, width, height, wait_delay=1):
    """Displays the TD events (change detection ATIS or DVS events)
    waitDelay: milliseconds
    """
    frame_length = 24e3
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
    return

def read_dataset(filename):
    """Reads in the TD events contained in the N-MNIST/N-CALTECH101 dataset file specified by 'filename'"""
    f = open(filename, 'rb')
    raw_data = np.fromfile(f, dtype=np.uint8)
    f.close()
    raw_data = np.uint32(raw_data)

    all_y = raw_data[1::5]
    all_x = raw_data[0::5]
    all_p = (raw_data[2::5] & 128) >> 7 #bit 7
    all_ts = ((raw_data[2::5] & 127) << 16) | (raw_data[3::5] << 8) | (raw_data[4::5])

    #Process time stamp overflow events
    time_increment = 2 ** 13
    overflow_indices = np.where(all_y == 240)[0]
    for overflow_index in overflow_indices:
        all_ts[overflow_index:] += time_increment

    #Everything else is a proper td spike
    td_indices = np.where(all_y != 240)[0]
    num_events = td_indices.size
    
    data = np.rec.array(None, dtype=[('x', np.uint16), ('y', np.uint16), ('p', np.uint8), ('ts', np.uint64)], shape=(num_events))
    data.x = all_x[td_indices]
    width = data.x.max() + 1
    data.y = all_y[td_indices]
    height = data.y.max() + 1
    data.ts = all_ts[td_indices]
    data.p = all_p[td_indices]
    return data, width, height

class seqPatternNMNIST(Dataset):
    def __init__(self, root, nframes=5, patt=[1,2,3], qsmp=3000, qrep=250):
        # generata a random list of all the files in nmnist with qsmp
        self.nframes = nframes
        samples_list_tmp = []
        target_list_tmp = []
        digit_list_tmp = []
        self.pattern_list = []
        self.patt = patt
        nros = os.listdir(root)
        for nro in nros:
            # get file list
            files = os.listdir(os.path.join(root,nro))
            assert(len(files) > qsmp)
            shuffle(files)
            for i in range(qsmp):
                samples_list_tmp.append(os.path.join(root,nro,files[i]))
                target_list_tmp.append(0)
                digit_list_tmp.append(nro)
            if int(nro) in self.patt:
                #print('%s in patt' % nro)
                shuffle(files)
                cumpatt = []
                for i in range(qrep):
                    cumpatt.append(os.path.join(root,nro,files[i]))
                self.pattern_list.append((cumpatt,nro))
        # now I need to shuffle the lists together
        c = list(zip(samples_list_tmp, target_list_tmp,digit_list_tmp))
        shuffle(c)
        samples_list_tmp, target_list_tmp, digit_list_tmp = zip(*c)
        self.samples_list, self.target_list,self.digit_list = list(samples_list_tmp), list(target_list_tmp), list(digit_list_tmp)
        #print(self.samples_list[0])
        #print('length of pattern lists ', len(self.pattern_list))
        # now, insert qrep times the pattern in samples list
        nsmp_tmp = len(self.samples_list)
        gap = int(nsmp_tmp / (qrep))
        #print('gap', gap)
        dx = int(0.1 * gap)
        #print('dx', dx)
        for k in range(1,qrep+1):
            kind = k*gap + randint(-dx,dx)
            #print('kind', kind)
            for i in range(len(self.pattern_list)):
                #print(self.pattern_list[i][0])
                self.samples_list.insert(kind+i,self.pattern_list[i][0][k-1])
                self.target_list.insert(kind+i,1)
                self.digit_list.insert(kind+i, self.pattern_list[i][1])
                
    def __len__(self):
        return len(self.samples_list)
    
    def __getitem__(self, index):
        #print('loading ',os.path.join(self.samples_list[index]))
        data, width, height =  read_dataset(self.samples_list[index])
        # transform data.ts into frames
        data.ts = np.round(data.ts / np.max(data.ts) * (self.nframes-1))
        # add relative index of the sequence
        data.ts = data.ts + index * self.nframes
        return np.array(data.ts, dtype=np.int64), np.array(data.x, dtype=np.int16), \
            np.array(data.y, dtype=np.int16), np.array(data.p, dtype=np.uint8), \
                self.target_list[index], self.digit_list[index]