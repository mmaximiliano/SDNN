"""
__author__ = Maximiliano Martino
__email__ = maxii.martino@gmail.com

SDNN Implementation based on Kheradpisheh, S.R., et al. 'STDP-based spiking deep neural networks 
for object recognition'. arXiv:1611.01421v1 (Nov, 2016)
"""
import os
#os.environ["CUDA_VISIBLE_DEVICES"] = "0,3"  # specify which GPU(s) to be used
os.environ["CUDA_VISIBLE_DEVICES"] = "0"
import torch
from SDNN_cuda import SDNN
import numpy as np
from math import floor
import argparse
import pathlib
import singleNeuron as sn
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
    parser.add_argument("-fname", "--fname", dest="file_name", default="none", action='store', help="File name", type=str)
    parser.add_argument("-s", "--s", dest="seed", default=0, action='store', help="Seed iteration", type=int)

    args = parser.parse_args()
    file_name = args.file_name
    seed = str(args.seed)

    # ------------------------------- Learn, Train and Test paths-------------------------------#
    # Sequences directories
    spike_times_pat_seq = '../../patterns/sequences/massive_runs/8/' + file_name
    path_seq_train = spike_times_pat_seq + "/training/" + seed + '/' + file_name + '.npy'
    path_seq_test = spike_times_pat_seq + "/testing/" + seed + '/' + "test_" + file_name + '.npy'
    path_seq_dif_sample = '../../patterns/sequences/' + "all_nums_dif_sample.npy"

    # Sequences pattern directories
    path_pat_test = spike_times_pat_seq + "/testing/" + seed + '/' + "pat_test_" + file_name + '.npy'
    path_pat_dif_sample = '../../patterns/sequences/' + "pat_all_nums_dif_sample.npy"

    # Results directories
    path_set_weigths = '../results/8/' + file_name + '/weights/' + seed + '/'
    path_save_weigths = '../results/8/' + file_name + '/weights/' + seed + '/'
    path_spikes_out_training = '../results/8/' + file_name + '/training/' + seed + '/'
    path_spikes_out_testing = '../results/8/' + file_name + '/testing/' + seed + '/'
    path_spikes_out_dif_sample = '../results/8/' + file_name + '/dif_sample/' + seed + '/'
    path_save_metrics = '../results/8/' + file_name + '/metrics/' + seed + '/'
    # ------------------------------- SDNN -------------------------------#
    # SDNN_cuda parameters
    frame_time = 15
    network_params = [{'Type': 'input', 'num_filters': 1, 'pad': (0, 0), 'H_layer': 34, 'W_layer': 34},
                      {'Type': 'conv', 'num_filters': 32, 'filter_size': 7, 'th': 15.,
                       'alpha': .99, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 32, 'filter_size': 2, 'th': 0., 'stride': 2},
                      {'Type': 'conv', 'num_filters': 80, 'filter_size': 5, 'th': 10.,
                       'alpha': .99, 'beta': 0., 'delay': 0},
                      {'Type': 'pool', 'num_filters': 80, 'filter_size': 4, 'th': 0., 'stride': 2},
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
                pathlib.Path(path_save_weigths).mkdir(parents=True, exist_ok=True)
                np.save(path_save_weigths + 'weight_'+str(i), weights[i])

    # ------------------------------- Run Testing Sequence & Save Results -------------------------------#
    print("Starting testing without training.")

    # Running Training sequence
    print("Test train sequence")
    Sin_tmp_train = first_net.train_features(path_seq_train)
    fname = 'layer_' + str(5) + '_' + str(network_params[5]['Type'])
    Sin_train = torch.tensor(Sin_tmp_train[5])
    pathlib.Path(path_spikes_out_training).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_train, path_spikes_out_training + fname + '.pt')

    # Running Testing sequence
    print("Test test sequence")
    Sin_tmp_test = first_net.train_features(path_seq_test)
    fname = 'layer_' + str(5) + '_' + str(network_params[5]['Type'])
    Sin_test = torch.tensor(Sin_tmp_test[5])
    pathlib.Path(path_spikes_out_testing).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_test, path_spikes_out_testing + fname + '.pt')

    # Running Dif Sample sequence
    print("Test dif_sample sequence")
    Sin_tmp_dif_sample = first_net.train_features(path_seq_dif_sample)
    fname = 'layer_' + str(5) + '_' + str(network_params[5]['Type'])
    Sin_dif_sample = torch.tensor(Sin_tmp_dif_sample[5])
    pathlib.Path(path_spikes_out_dif_sample).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_dif_sample, path_spikes_out_dif_sample + fname + '.pt')

    # -------------------------- Single Neuron Output -------------------------- #
    print("Start Single Neuron Training and Testing for each th value")
    for th in [1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 99.0, 98.0]:
        if th == 99.0:
            str_th = "2_5-3_0"
        elif th == 98.0:
            str_th = "2_5-3_5"
        else:
            str_th = str(th)
            str_th = str_th[0] + '_' + str_th[2]

        more_th = False
        new_th = th
        if th == 99.0:
            th = 2.5
            new_th = 3.0
            more_th = True
        elif th == 98.0:
            th = 2.5
            new_th = 3.5
            more_th = True

        # ===== Train Single Neuron ===== #
        Sin = Sin_train.clone().detach()
        T = Sin.shape[0]
        N_out = 1
        N_in = Sin.shape[1]

        singleNeuron = sn.STDPLIFDensePopulation(in_channels=N_in, out_channels=N_out,
                                                weight=0.75, alpha=float(np.exp(-1e-3/10e-3)),
                                                beta=float(np.exp(-1e-3/2e-5)), delay=0,
                                                th=th, a_plus=.03125, a_minus=.04125,
                                                w_max=1.)

        # Pre-procesamos PSpikes y NSpikes
        dt_ltp = 20  # Cantidad de timesteps que miro hacia atras
        dt_ltd = 70  # Cantidad de timesteps que miro hacia delante
        PSpikes = sn.preSpikes(T, dt_ltp, torch.zeros(T, N_in), Sin)
        NSpikes = sn.nextSpikes(T, dt_ltd, torch.zeros(T, N_in), Sin)

        # Realizamos el entrenamiento STDP
        Uprobe = np.empty([T, N_out])
        Iprobe = np.empty([T, N_out])
        Sprobe = np.empty([T, N_out])

        n = 0
        t = 0
        c_l = 1.
        while (c_l > 0.01) and (t < 550000):
            state = singleNeuron.forward(Sin[n].unsqueeze(0), PSpikes[n], NSpikes[n-1])
            Uprobe[n] = state.U.data.numpy()
            Iprobe[n] = state.I.data.numpy()
            Sprobe[n] = state.S.data.numpy()
            n += 1
            if n >= T:
                n = 0

            if more_th and (t == 400000):
                singleNeuron.th = new_th
            c_l = sn.convergence_level(singleNeuron.fc_layer.weight[0])
            t += 1

        # ===================== Test Single Neuron ===================== #
        pat_times = np.load(path_pat_test)
        Sin = Sin_test
        T = Sin.shape[0]
        N_out = 1

        if more_th:
            singleNeuron.th = 3.75

        # Realizamos el testing
        Uprobe = np.empty([T, N_out])
        Iprobe = np.empty([T, N_out])
        Sprobe = np.empty([T, N_out])
        for n in range(T):
            state = singleNeuron.forward_no_learning(Sin[n].unsqueeze(0))
            Uprobe[n] = state.U.data.numpy()
            Iprobe[n] = state.I.data.numpy()
            Sprobe[n] = state.S.data.numpy()

        # Corremos metricas
        # remove last part of sequence since its wrong
        accuracy, precision, recall, f1, fake_alarms, missed_alarms = sn.get_metrics(T - 15, pat_times, Sprobe)

        metrics = np.array([accuracy, precision, recall, f1, fake_alarms, missed_alarms], dtype=float)
        pathlib.Path(path_save_metrics).mkdir(parents=True, exist_ok=True)
        np.save(path_save_metrics + 'test_metrics_' + str_th + '.npy', metrics)

        # ===================== Dif Sample Single Neuron ===================== #
        pat_times = np.load(path_pat_dif_sample)
        Sin = Sin_dif_sample
        T = Sin.shape[0]
        N_out = 1

        if more_th:
            singleNeuron.th = 3.75

        # Realizamos el testing
        Uprobe = np.empty([T, N_out])
        Iprobe = np.empty([T, N_out])
        Sprobe = np.empty([T, N_out])
        for n in range(T):
            state = singleNeuron.forward_no_learning(Sin[n].unsqueeze(0))
            Uprobe[n] = state.U.data.numpy()
            Iprobe[n] = state.I.data.numpy()
            Sprobe[n] = state.S.data.numpy()

        # Corremos metricas
        # remove last part of sequence since its wrong
        accuracy, precision, recall, f1, fake_alarms, missed_alarms = sn.get_metrics(T - 15, pat_times, Sprobe)

        metrics = np.array([accuracy, precision, recall, f1, fake_alarms, missed_alarms], dtype=float)
        pathlib.Path(path_save_metrics).mkdir(parents=True, exist_ok=True)
        np.save(path_save_metrics + 'dif_sample_metrics_' + str_th + '.npy', metrics)

    print('Done.')

if __name__ == '__main__':
    start = time.time()
    main()
    end = time.time()
    print(end-start)
