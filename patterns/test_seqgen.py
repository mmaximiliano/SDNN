# -*- coding: utf-8 -*-
"""
Created on Thu Jun 24 08:34:30 2021

@author: pnegr
"""

import pan_functions
from torch.utils.data import DataLoader
import torch
import numpy as np
from os.path import dirname, realpath

root_dataset = './N-MNIST/Train'
path = dirname(realpath(__file__))
path_seq_pat = path + '/sequences/'

# seed random for same sequence
pan_functions.seed_torch(seed=0)

# parameters of the dataset - this are only have the purpose of testing - use defaults
nframes=5 # number of frames to divide the event stream
patt=[1,2,3] # pattern to copy throught the sequence
qsmp=50 # number of samples of each digit to include in the sequence
qrep=50 # number of times the patter is repeated on the sequence

train_seq = pan_functions.seqPatternNMNIST(root=root_dataset,nframes=nframes, 
                      patt=patt, qsmp=qsmp, qrep=qrep)
print('Dataset reading resulting on %d samples' % len(train_seq))
# the train loader should have batch_size equal 1 and shuffle equal False 
# to conserve the structure of the sequence
train_loader = DataLoader(train_seq, batch_size=1, shuffle=False)

# Calculate total time
time = (nframes*len(patt)*qrep) + (10*qsmp*nframes)
spike_times = np.zeros([34, 34, time]) # Create matrix of spike times
pat_times = np.zeros(time) # Indicator of the pattern in the sequence

for ts,x,y,p, target, digit in train_loader:
    x = torch.squeeze(x)
    y = torch.squeeze(y)
    p = torch.squeeze(p)
    ts = torch.squeeze(ts)
    #print("Max index: " + str(y.max()) + str(x.max()) + str(ts.max()))
    for i in range(p.shape[0]):  # iterate over the polarity array
        if p[i]:  # if polarity is positive
            index = (y[i], x[i], ts[i])
            spike_times[index] = 1
        if target[0]:
            pat_times[ts[i]] = 1
    #print('%s -> sq [ %d, %d ] -> size %d -> t: %d' % (digit[0], ts.min(), ts.max(), 
    #                                                   ts.shape[0], target[0]))

print("Sequence length: " + str(time))
np.save(path_seq_pat + 'seq_0', spike_times)
np.save(path_seq_pat + 'pat_i_0', pat_times)