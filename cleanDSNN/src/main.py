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
import argparse
import pathlib

import time


def main():

    # Flags
    learn_SDNN = True   # This flag toggles between Learning STDP and classify features
                        # or just classify by loading pretrained weights
    free_spikes = False  # This flag toggles whether we allowed neurons to spikes every timestep or once per timeframe
    c_learning = True   # This flag toggles whether we learn by a fixed amount of iterationts or when weights converges

    if learn_SDNN:
        set_weights = False  # Loads the weights from a path (path_set_weigths) and prevents any SDNN learning
        save_weights = True  # Saves the weights in a path (path_save_weigths)
    else:
        set_weights = True  # Loads the weights from a path (path_set_weigths) and prevents any SDNN learning
        save_weights = False  # Saves the weights in a path (path_save_weigths)

    #--- Parse Arguments ---#
    parser = argparse.ArgumentParser()
    parser.add_argument("-fl", "--fl", dest="file_name", default="none", action='store', help="File name", type=str)
    parser.add_argument("-s", "--s", dest="seed", default=0, action='store', help="Seed iteration", type=int)

    args = parser.parse_args()
    file_name = args.file_name
    seed = str(args.seed)

    # ------------------------------- Learn, Train and Test paths-------------------------------#
    # Sequences directories
    spike_times_pat_seq = '../../patterns/sequences/massive_runs/1/' + file_name
    path_seq_train = spike_times_pat_seq + "training/" + seed + file_name
    path_seq_test = spike_times_pat_seq + "testing/" + seed + "test_" + file_name
    path_seq_dif_sample = '../../patterns/sequences/' + "all_nums_dif_sample.npy"

    # Results directories
    path_set_weigths = '../results/1/' + file_name + '/weights/' + seed + '/'
    path_save_weigths = '../results/1/' + file_name + '/weights/' + seed + '/'
    path_spikes_out_training = '../results/1/' + file_name + '/training/' + seed + '/'
    path_spikes_out_testing = '../results/1/' + file_name + '/testing/' + seed + '/'
    path_spikes_out_dif_sample = '../results/1/' + file_name + 'dif_sample/' + seed + '/'

    # ------------------------------- SDNN -------------------------------#
    # SDNN_cuda parameters
    frame_time = 15
    network_params = [{'Type': 'input', 'num_filters': 1, 'pad': (0, 0), 'H_layer': 34, 'W_layer': 34},
                      {'Type': 'conv', 'num_filters': 32, 'filter_size': 7, 'th': 15.,
                       'alpha': .99, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 32, 'filter_size': 2, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 80, 'filter_size': 5, 'th': 10.,
                       'alpha': .99, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 80, 'filter_size': 3, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 80, 'filter_size': 3, 'th': 2.,
                       'alpha': .99, 'beta': 0., 'delay': 0}]
    weight_params = {'mean': 0.8, 'std': 0.01}

    max_learn_iter = [0, 6500, 0, 11000, 0, 13000, 0]
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
                     spike_times_pat_seq=path_seq_train, c_learning=c_learning, device='GPU')

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

    # Running Training sequence
    Sin_tmp_train = first_net.train_features(path_seq_train)
    fname = 'layer_' + str(5) + '_' + str(network_params[5]['Type'])
    Sin_train = torch.tensor(Sin_tmp_train[5])
    pathlib.Path(path_spikes_out_training).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_train, path_spikes_out_training + fname + '.pt')

    # Running Testing sequence
    Sin_tmp_test = first_net.train_features(path_seq_test)
    fname = 'layer_' + str(5) + '_' + str(network_params[5]['Type'])
    Sin_test = torch.tensor(Sin_tmp_test[5])
    pathlib.Path(path_spikes_out_testing).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_test, path_spikes_out_testing + fname + '.pt')

    # Running Dif Sample sequence
    Sin_tmp_dif_sample = first_net.train_features(path_seq_dif_sample)
    fname = 'layer_' + str(5) + '_' + str(network_params[5]['Type'])
    Sin_dif_sample = torch.tensor(Sin_tmp_dif_sample[5])
    pathlib.Path(path_spikes_out_dif_sample).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_dif_sample, path_spikes_out_dif_sample + fname + '.pt')

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
