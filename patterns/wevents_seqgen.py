# -*- coding: utf-8 -*-
"""
Created on Mon Jul 19 08:34:30 2021

@author: mmartino
"""

import pan_functions
from torch.utils.data import DataLoader
import torch
import numpy as np
import argparse
from os.path import dirname, realpath

# parse arguments
parser = argparse.ArgumentParser()

parser.add_argument("-p", "--p", nargs='+', dest="patt_list", default=[1], action='store', help="Pattern list", type=int)
parser.add_argument("-nn", "--nn", nargs='+', dest="noiseNums", default=None, action='store', help="Noisy Numbers", type=int)
parser.add_argument("-w", "--w", dest="nevents", default=120, action='store', help="Number of events", type=int)
parser.add_argument("-pf", "--pf", dest="pfreq", default=3, action='store', help="Frequency of the pattern", type=int)
parser.add_argument("-nd", "--nd", dest="nDigits", default=1000, action='store', help="Number of Digits", type=int)
parser.add_argument("-fname", "--fname", dest="fname", default="default_seq", action='store',
                    help="File name of the sequence", type=str)

args = parser.parse_args()

noiseNums = args.noiseNums
fname = args.fname

root_dataset = './N-MNIST/Train'
path = dirname(realpath(__file__))
path_seq_pat = path + '/sequences/'

# seed random for same sequence
pan_functions.seed_torch(seed=0)

# parameters of the dataset
nframes = 15            # frames per digit
wevents = args.nevents  # number of events to take per ts
patt = args.patt_list   # pattern to copy throught the sequence
nDigits = args.nDigits  # Amount of digits to include in the random sequence
pfreq = args.pfreq      # Every 'pfreq' digits, I insert the pattern
qsmp = 27               # number of samples of each digit to include in the sequence
qrep = 30               # number of times the patter is repeated on the sequence

# Pablo's function
# train_seq = pan_functions.seqPatternEvents(root=root_dataset,nframes=nframes,
#                      wevents=wevents, patt=patt, qsmp=qsmp, qrep=qrep)
# time = (nframes*len(patt)*qrep) + (10*qsmp*nframes)
# print(str(time))

# Maxi's function
#train_seq = pan_functions.fixedPattern(root=root_dataset, nframes=nframes,
#                                       wevents=wevents, patt=patt, nDigits=nDigits, pfreq=pfreq, noiseNums=noiseNums)
train_seq = pan_functions.randomPattern(root=root_dataset, nframes=nframes,
                                        wevents=wevents, patt=patt, nDigits=nDigits, pfreq=pfreq, noiseNums=noiseNums)

# Calculate total time
time = len(train_seq) * nframes
print("timesteps: " + str(time))

print('Dataset reading resulting on %d samples' % len(train_seq))
# the train loader should have batch_size equal 1 and shuffle equal False 
# to conserve the structure of the sequence
train_loader = DataLoader(train_seq, batch_size=1, shuffle=False)

spike_times = np.zeros([34, 34, time])  # Create matrix of spike times
pat_times = np.zeros(time)  # Indicator of the pattern in the sequence
# Save complete sequence
data = []

for ts, x, y, p, target, digit in train_loader:
    x = torch.squeeze(x)
    y = torch.squeeze(y)
    p = torch.squeeze(p)
    ts = torch.squeeze(ts)
    # print("Max index: " + str(y.max()) + str(x.max()) + str(ts.max()))
    for i in range(p.shape[0]):  # iterate over the polarity array
        # if p[i]:  # if polarity is positive
        index = (y[i], x[i], ts[i])
        spike_times[index] = 1
        if target[0]:
            pat_times[ts[i]] = 1
        # get info to show sequence:
        data.append((x[i].item(), y[i].item(), p[i].item(), ts[i].item()))
    # print('%s -> sq [ %d, %d ] -> size %d -> t: %d' % (digit[0], ts.min(), ts.max(),
    #                                                   ts.shape[0], target[0]))

# print("Sequence length: " + str(time))
data = np.array(data, np.dtype('uint16, uint16, uint8, uint64'))

np.save(path_seq_pat + fname, spike_times)
np.save(path_seq_pat + 'pat_' + fname, pat_times)
np.save(path_seq_pat + 'data_' + fname, data)
