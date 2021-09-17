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
    learn_SDNN = False   # This flag toggles between Learning STDP and classify features
                        # or just classify by loading pretrained weights
    free_spikes = False  # This flag toggles whether we allowed neurons to spikes every timestep or once per timeframe
    c_learning = True   # This flag toggles whether we learn by a fixed amount of iterationts or when weights converges

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
                      {'Type': 'conv', 'num_filters': 24, 'filter_size': 7, 'th': 15.,
                       'alpha': .99, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 24, 'filter_size': 2, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 60, 'filter_size': 5, 'th': 10.,
                       'alpha': .99, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 60, 'filter_size': 3, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 60, 'filter_size': 3, 'th': 2.,
                       'alpha': .99, 'beta': 0., 'delay': 0}]
    weight_params = {'mean': 0.8, 'std': 0.01}

    max_learn_iter = [0, 6500, 0, 11000, 0, 13000, 0]
    stdp_params = {'max_learn_iter': max_learn_iter,
                   'stdp_per_layer': [0, 20, 0, 8, 0, 4],
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
                     spike_times_pat_seq=spike_times_pat_seq, c_learning=c_learning, device='GPU')

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
            if network_params[1]['delay'] != 0:
                np.save(path_save_weigths + 'delayed_weight_'+str(i), weights[i])
            else:
                np.save(path_save_weigths + 'weight_'+str(i), weights[i])

    # ------------------------------- Run Testing Sequence & Save Results -------------------------------#
    Sin_tmp = first_net.train_features()
    for i in range(1, len(network_params)):
        if network_params[1]['delay'] != 0:
            fname = 'delayed_layer_' + str(i) + '_' + str(network_params[i]['Type'])
        else:
            fname = 'layer_' + str(i) + '_' + str(network_params[i]['Type'])
        Sin = torch.tensor(Sin_tmp[i])
        torch.save(Sin, path_spikes_out + fname + '.pt')

    print('DONE')

    # -------------------------- Single Neuron Output -------------------------- #
    #Sin_tmp = torch.tensor(Sin_tmp[len(network_params)-1])
    #T = Sin_tmp.shape[0]
    #N_out = 1
    #N_in = Sin_tmp.shape[1]
    #th = 3.8

    #singleNeuron = STDPLIFDensePopulation(in_channels=N_in, out_channels=N_out,
    #                                      weight=0.75, alpha=float(np.exp(-1e-3 / 10e-3)),
    #                                      beta=float(np.exp(-1e-3 / 2e-5)), delay=0,
    #                                      th=th, a_plus=.03225, a_minus=.045625,
    #                                      w_max=1.)
    #Sin = Sin_tmp.clone().detach()

    # Pre-procesamos PSpikes y NSpikes
    #dt_ltp = 10  # Cantidad de timesteps que miro hacia atras
    #dt_ltd = 50  # Cantidad de timesteps que miro hacia delante
    #PSpikes = preSpikes(T, dt_ltp, torch.zeros(T, N_in), Sin)
    #NSpikes = nextSpikes(T, dt_ltd, torch.zeros(T, N_in), Sin)

    # Realizamos el entrenamiento STDP
    #Uprobe = np.empty([T, N_out])
    #Iprobe = np.empty([T, N_out])
    #Sprobe = np.empty([T, N_out])
    #for n in range(T):
    #    state = singleNeuron.forward(Sin[n].unsqueeze(0), PSpikes[n], NSpikes[n - 1])
    #    Uprobe[n] = state.U.data.numpy()
    #    Iprobe[n] = state.I.data.numpy()
    #    Sprobe[n] = state.S.data.numpy()

if __name__ == '__main__':
    start = time.time()
    main()
    end = time.time()
    print(end-start)
