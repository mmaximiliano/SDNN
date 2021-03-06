"""
__author__ = Maximiliano Martino
__email__ = maxii.martino@gmail.com
"""

from SDNN_cuda import SDNN
from Classifier import Classifier
import torch
from singleNeuron import preSpikes, nextSpikes, STDPLIFDensePopulation
import numpy as np
from os.path import dirname, realpath
from math import floor

import time


def main():

    # Flags
    learn_SDNN = True  # This flag toggles between Learning STDP and classify features
                        # or just classify by loading pretrained weights for the face/motor dataset
    SVM = False  # This flag toggles between using a max global pooling + linear SVM classifier
                # or using a single nueron to detect patterns
    if learn_SDNN:
        set_weights = False  # Loads the weights from a path (path_set_weights) and prevents any SDNN learning
        save_weights = True  # Saves the weights in a path (path_save_weights)
        save_features = True  # Saves the features and labels in the specified path (path_features)
    else:
        set_weights = True  # Loads the weights from a path (path_set_weights) and prevents any SDNN learning
        save_weights = False  # Saves the weights in a path (path_save_weights)
        save_features = False  # Saves the features and labels in the specified path (path_features)

    # ------------------------------- Learn, Train and Test paths-------------------------------#
    path = dirname(dirname(realpath(__file__)))
    if SVM:
        # Image sets directories
        spike_times_learn = [path + '/datasets/LearningSet/Face/', path + '/datasets/LearningSet/Motor/']
        spike_times_train = [path + '/datasets/TrainingSet/Face/', path + '/datasets/TrainingSet/Motor/']
        spike_times_test = [path + '/datasets/TestingSet/Face/', path + '/datasets/TestingSet/Motor/']
    else:
        spike_times_pat_seq = '../../patterns/sequences/'

    # Results directories
    path_set_weights = path + '/results/weights/'
    path_save_weights = path + '/results/weights/'
    path_features = path + '/results/'

    # ------------------------------- SDNN -------------------------------#
    # SDNN_cuda parameters
    if SVM:
        DoG_params = {'img_size': (250, 160), 'DoG_size': 7, 'std1': 1., 'std2': 2.}  # img_size is (col size, row size)
        total_time = 15
        network_params = [{'Type': 'input', 'num_filters': 1, 'pad': (0, 0), 'H_layer': DoG_params['img_size'][1],
                           'W_layer': DoG_params['img_size'][0]},
                          {'Type': 'conv', 'num_filters': 4, 'filter_size': 5, 'th': 10.,
                           'alpha': 1., 'beta': 1., 'delay': 0},
                          {'Type': 'pool', 'num_filters': 4, 'filter_size': 7, 'th': 0., 'stride': 6},
                          {'Type': 'conv', 'num_filters': 20, 'filter_size': 17, 'th': 60.,
                           'alpha': 1., 'beta': 1., 'delay': 0},
                          {'Type': 'pool', 'num_filters': 20, 'filter_size': 5, 'th': 0., 'stride': 5},
                          {'Type': 'conv', 'num_filters': 20, 'filter_size': 5, 'th': 2.,
                           'alpha': .95, 'beta': .95, 'delay': 0}]
        max_learn_iter = [0, 3000, 0, 5000, 0, 6000, 0]
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
        weight_params = {'mean': 0.8, 'std': 0.01}

        # Create network -> TENER EN CUENTA EL TOTAL TIME (BUSCAR LA FORMA DE ESTANDARIZARLO)
        first_net = SDNN(network_params, weight_params, stdp_params, 295,
                         DoG_params=DoG_params, spike_times_learn=spike_times_learn,
                         spike_times_train=spike_times_train, spike_times_test=spike_times_test,
                         svm=SVM, device='GPU')
    else:
        # Seteo Total time de las secuencias
        # Potencialmente esto hay que cambiarlo, o buscar la forma de automatizarlo
        total_time = 10
        seq_frames = 3250/5
        network_params = [{'Type': 'input', 'num_filters': 1, 'pad': (0, 0), 'H_layer': 34,
                           'W_layer': 34},
                          {'Type': 'conv', 'num_filters': 4, 'filter_size': 5, 'th': 10.,
                           'alpha': .95, 'beta': .95, 'delay': 0},
                          {'Type': 'pool', 'num_filters': 4, 'filter_size': 6, 'th': 0., 'stride': 3},
                          {'Type': 'conv', 'num_filters': 20, 'filter_size': 6, 'th': 60.,
                           'alpha': .95, 'beta': .95, 'delay': 0}]
        max_learn_iter = [0, 1745, 0, 1745, 0]
        stdp_params = {'max_learn_iter': max_learn_iter,
                       'stdp_per_layer': [0, 10, 0, 4],
                       'max_iter': sum(max_learn_iter),
                       'a_minus': np.array([0, .003, 0, .0003], dtype=np.float32),
                       'a_plus': np.array([0, .004, 0, .0004], dtype=np.float32),
                       'offset_STDP': [0, floor(network_params[1]['filter_size']),
                                       0,
                                       floor(network_params[3]['filter_size']/8)]}
        weight_params = {'mean': 0.8, 'std': 0.01}

        # Create network
        first_net = SDNN(network_params, weight_params, stdp_params, total_time,
                         svm=SVM, spike_times_pat_seq=spike_times_pat_seq, device='GPU')


    # Set the weights or learn STDP
    if set_weights:
        weight_path_list = []
        for i in range(len(network_params) - 1):
            if (network_params[i]['Type'] == 'P_conv') | \
                    (network_params[i]['Type'] == 'P_pool') | \
                    (network_params[i]['Type'] == 'PG_pool') | \
                    ((i >= 1) & (network_params[i-1]['Type'] == 'P_conv')) | \
                    ((i >= 1) & (network_params[i-1]['Type'] == 'P_pool')):
                weight_path_list.append(str(path_set_weights + 'weight_P0_' + str(i) + '.npy'))
                weight_path_list.append(str(path_set_weights + 'weight_P1_' + str(i) + '.npy'))
            else:
                weight_path_list.append(str(path_set_weights + 'weight_' + str(i) + '.npy'))
        first_net.set_weights(weight_path_list)
    else:
        first_net.train_SDNN()

    # Save the weights
    if save_weights:
        weights = first_net.get_weights()
        for i in range(len(weights)):
            if (network_params[i]['Type'] == 'P_conv') | \
                    (network_params[i]['Type'] == 'P_pool') | \
                    (network_params[i]['Type'] == 'PG_pool') | \
                    ((i >= 1) & (network_params[i-1]['Type'] == 'P_conv')) | \
                    ((i >= 1) & (network_params[i-1]['Type'] == 'P_pool')):
                np.save(path_save_weights + 'weight_P0_' + str(i), weights[i][0])
                np.save(path_save_weights + 'weight_P1_' + str(i), weights[i][1])
            else:
                np.save(path_save_weights + 'weight_'+str(i), weights[i])

    # ------------------------------- Classify -------------------------------#
    if SVM:
        # Get features
        X_train, y_train = first_net.train_features()
        X_test, y_test = first_net.test_features()

        # Save X_train and X_test
        if save_features:
            np.save(path_features + 'X_train', X_train)
            np.save(path_features + 'y_train', y_train)
            np.save(path_features + 'X_test', X_test)
            np.save(path_features + 'y_test', y_test)

        classifier_params = {'C': 1.0, 'gamma': 'auto'}
        train_mean = np.mean(X_train, axis=0)
        train_std = np.std(X_train, axis=0)
        X_train -= train_mean
        X_test -= train_mean
        X_train /= (train_std + 1e-5)
        X_test /= (train_std + 1e-5)
        svm = Classifier(X_train, y_train, X_test, y_test, classifier_params, classifier_type='SVM')
        train_score, test_score = svm.run_classiffier()
        print('Train Score: ' + str(train_score))
        print('Test Score: ' + str(test_score))

        print('DONE')

    else:
        print("Pattern classification - NOT IMPLEMENTED YET")
        Sin_tmp = first_net.train_features()
        T = Sin_tmp.shape[0]
        N_out = 1
        N_in = Sin_tmp.shape[1]

        singleNeuron = STDPLIFDensePopulation(in_channels=N_in, out_channels=N_out,
                                              weight=0.7, alpha=float(np.exp(-1e-3/10e-3)),
                                              beta=float(np.exp(-1e-3/2e-5)), delay=0,
                                              th=25., a_plus=0.003125, a_minus=0.00865625,
                                              w_max=1.)
        Sin = torch.tensor(Sin_tmp)
        torch.save(Sin, path_features + 'Sin-out.pt')
        # Pre-procesamos PSpikes y NSpikes
        dt_ltp = 5   # Cantidad de timesteps que miro hacia atras
        dt_ltd = 10  # Cantidad de timesteps que miro hacia delante
        PSpikes = preSpikes(T, dt_ltp, torch.zeros(T, N_in), Sin)
        NSpikes = nextSpikes(T, dt_ltd, torch.zeros(T, N_in), Sin)

        # Realizamos el entrenamiento STDP
        Uprobe = np.empty([T, N_out])
        Iprobe = np.empty([T, N_out])
        Sprobe = np.empty([T, N_out])
        for n in range(T):
            state = singleNeuron.forward(Sin[n].unsqueeze(0), PSpikes[n], NSpikes[n-1])
            Uprobe[n] = state.U.data.numpy()
            Iprobe[n] = state.I.data.numpy()
            Sprobe[n] = state.S.data.numpy()

        torch.save(Sprobe, path_features + 'Sprobe.pt')
        torch.save(Uprobe, path_features + 'Uprobe.pt')

if __name__ == '__main__':
    start = time.time()
    main()
    end = time.time()
    print("Total Time: " + str(end-start))
                                                        































