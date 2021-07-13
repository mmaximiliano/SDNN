"""
__author__ = Maximiliano Martino
__email__ = maxii.martino@gmail.com

SDNN Implementation based on Kheradpisheh, S.R., et al. 'STDP-based spiking deep neural networks 
for object recognition'. arXiv:1611.01421v1 (Nov, 2016)
"""
import torch
from SDNN_cuda import SDNN
import numpy as np
from math import floor

import time


def main():

    # Flags
    learn_SDNN = True  # This flag toggles between Learning STDP and classify features
                        # or just classify by loading pretrained weights for the face/motor dataset
    if learn_SDNN:
        set_weights = False  # Loads the weights from a path (path_set_weigths) and prevents any SDNN learning
        save_weights = True  # Saves the weights in a path (path_save_weigths)
    else:
        set_weights = True  # Loads the weights from a path (path_set_weigths) and prevents any SDNN learning
        save_weights = False  # Saves the weights in a path (path_save_weigths)

    # ------------------------------- Learn, Train and Test paths-------------------------------#
    # Sequences directories
    spike_times_pat_seq = '../../patterns/sequences/'

    # Results directories
    path_set_weigths = '../results/'
    path_save_weigths = '../results/'
    path_spikes_out = '../results/'

    # ------------------------------- SDNN -------------------------------#
    # SDNN_cuda parameters
    frame_time = 10
    network_params = [{'Type': 'input', 'num_filters': 1, 'pad': (0, 0), 'H_layer': 34, 'W_layer': 34},
                      {'Type': 'conv', 'num_filters': 4, 'filter_size': 4, 'th': 10.,
                       'alpha': 1., 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 4, 'filter_size': 5, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 4, 'filter_size': 3, 'th': 0.,
                       'alpha': 1., 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 4, 'filter_size': 2, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 4, 'filter_size': 4, 'th': 2.,
                       'alpha': 1., 'beta': 0., 'delay': 0}]
    weight_params = {'mean': 0.8, 'std': 0.01}

    max_learn_iter = [0, 1000, 0, 1000, 0, 1490, 0]
    stdp_params = {'max_learn_iter': max_learn_iter,
                   'stdp_per_layer': [0, 10, 0, 4, 0, 2],
                   'max_iter': sum(max_learn_iter),
                   'a_minus': np.array([0, .003, 0, .0003, 0, .0003], dtype=np.float32),
                   'a_plus': np.array([0, .004, 0, .0004, 0, .0004], dtype=np.float32),
                   'offset_STDP': [0, floor(network_params[1]['filter_size']),
                                   0,
                                   floor(network_params[3]['filter_size']/8),
                                   0,
                                   floor(network_params[5]['filter_size'])]}

    # Create network
    first_net = SDNN(network_params, weight_params, stdp_params, frame_time,
                     spike_times_pat_seq=spike_times_pat_seq, device='GPU')

    # Set the weights or learn STDP
    if set_weights:
        weight_path_list = [path_set_weigths + 'weight_' + str(i) + '.npy' for i in range(len(network_params) - 1)]
        first_net.set_weights(weight_path_list)
    else:
        first_net.train_SDNN()

    # Save the weights
    if save_weights:
        weights = first_net.get_weights()
        for i in range(len(weights)):
            np.save(path_save_weigths + 'weight_'+str(i), weights[i])

    # ------------------------------- Classify -------------------------------#
    Sin_tmp = first_net.train_features()
    Sin = torch.tensor(Sin_tmp)
    torch.save(Sin, path_spikes_out + 'Sin-out.pt')

    print('DONE')


if __name__ == '__main__':
    start = time.time()
    main()
    end = time.time()
    print(end-start)
