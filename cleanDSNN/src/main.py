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
    learn_SDNN = True   # This flag toggles between Learning STDP and classify features
                        # or just classify by loading pretrained weights
    free_spikes = False  # This flag toggles whether we allowed neurons to spikes every timestep or once per timeframe

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
    frame_time = 15
    network_params = [{'Type': 'input', 'num_filters': 1, 'pad': (0, 0), 'H_layer': 34, 'W_layer': 34},
                      {'Type': 'conv', 'num_filters': 8, 'filter_size': 7, 'th': 15.,
                       'alpha': .95, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 8, 'filter_size': 2, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 20, 'filter_size': 5, 'th': 10.,
                       'alpha': .95, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 20, 'filter_size': 3, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 20, 'filter_size': 3, 'th': 2.,
                       'alpha': .95, 'beta': 0., 'delay': 0}]
    weight_params = {'mean': 0.8, 'std': 0.01}

    max_learn_iter = [0, 3000, 0, 5500, 0, 7000, 0]
    stdp_params = {'max_learn_iter': max_learn_iter,
                   'stdp_per_layer': [0, 10, 0, 4, 0, 2],
                   'max_iter': sum(max_learn_iter),
                   'a_minus': np.array([0, .003, 0, .0003, 0, .0003], dtype=np.float32),
                   'a_plus': np.array([0, .004, 0, .0004, 0, .0004], dtype=np.float32),
                   'offset_STDP': [0, floor(network_params[1]['filter_size']),
                                   0,
                                   floor(network_params[3]['filter_size']/5),
                                   0,
                                   floor(network_params[5]['filter_size'])]}

    # Create network
    first_net = SDNN(network_params, weight_params, stdp_params, frame_time, free_spikes,
                     spike_times_pat_seq=spike_times_pat_seq, device='GPU')

    # Set the weights or learn STDP
    if set_weights:
        if free_spikes:
            weight_path_list = [path_set_weigths + 'delayed_weight_' + str(i) + '.npy' for i in range(len(network_params) - 1)]
        else:
            weight_path_list = [path_set_weigths + 'weight_' + str(i) + '.npy' for i in range(len(network_params) - 1)]
        first_net.set_weights(weight_path_list)
    else:
        first_net.train_SDNN()

    # Save the weights
    if save_weights:
        weights = first_net.get_weights()
        for i in range(len(weights)):
            if free_spikes:
                np.save(path_save_weigths + 'delayed_weight_'+str(i), weights[i])
            else:
                np.save(path_save_weigths + 'weight_'+str(i), weights[i])

    # ------------------------------- Classify -------------------------------#
    Sin_tmp = first_net.train_features()
    for i in range(1, len(network_params)):
        if network_params[1]['delay'] != 0:
            fname = 'delayed_layer_' + str(i) + '_' + str(network_params[i]['Type'])
        else:
            fname = 'layer_' + str(i) + '_' + str(network_params[i]['Type'])
        Sin = torch.tensor(Sin_tmp[i])
        torch.save(Sin, path_spikes_out + fname + '.pt')

    print('DONE')


if __name__ == '__main__':
    start = time.time()
    main()
    end = time.time()
    print(end-start)
