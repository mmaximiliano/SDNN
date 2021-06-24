# -*- coding: utf-8 -*-
"""
Created on Thu Jun 24 08:34:30 2021

@author: pnegr
"""

import pan_functions
from torch.utils.data import DataLoader

root_dataset = 'C:/Record/DVS/N-MNIST/Train'

# seed random for same sequence
pan_functions.seed_torch(seed=0)

# parameters of the dataset - this are only have the purpose of testing - use defaults
nframes=5 # number of frames to divide the event stream
patt=[1,2,3] # pattern to copy throught the sequence
qsmp=5 # number of samples of each digit to include in the sequence
qrep=3 # number of times the patter is repeated on the sequence

train_seq = pan_functions.seqPatternNMNIST(root=root_dataset,nframes=nframes, 
                      patt=patt, qsmp=qsmp, qrep=qrep)
print('Dataset reading resulting on %d samples' % len(train_seq))
# the train loader should have batch_size equal 1 and shuffle equal False 
# to conserve the structure of the sequence
train_loader = DataLoader(train_seq, batch_size=1, shuffle=False)


for ts,x,y,p, target, digit in train_loader:
    print('%s -> sq [ %d, %d ] -> size %d -> t: %d' % (digit[0], ts.min(), ts.max(), 
                                                       ts.shape[1], target[0]))
    