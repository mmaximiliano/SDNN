import torch
import numpy as np
import argparse
import pathlib
from singleNeuron_omega import preSpikes, nextSpikes, STDPLIFDensePopulation, convergence_level, get_metrics, get_metrics_long_pat


#--- Parse Arguments ---#
parser = argparse.ArgumentParser()
parser.add_argument("-folder", "--folder", dest="folder", default="none", action='store', help="folder name", type=str)
parser.add_argument("-neurons", "--neurons", dest="neurons", default=1, action='store', help="Number of neurons", type=str)
parser.add_argument("-w", "--w", dest="w", default="0", action='store', help="Initial weight", type=str)
parser.add_argument("-seed", "--seed", dest="seed", default=0, action='store', help="Initial weight", type=int)
parser.add_argument("-umbral", "--umbral", dest="umbral", default="0", action='store', help="Initial weight", type=str)


args = parser.parse_args()
folder = args.folder
seed = str(args.seed)
neurons = args.neurons
weight = args.w
umbral = args.umbral

# Cargamos la salida de la FC
Sin = torch.load('./sequences/massive_runs/1/fc_outputs/' + folder + '/training/' + seed + '/fc_output_' + neurons + '_w' + weight + '_th' + umbral + '.pt')
T = Sin.shape[0]
N_out = 1
N_in = Sin.shape[1]

for th in [1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5]:
    # Prepare string for fname
    str_th = str(th)
    str_th = str_th[0] + '-' + str_th[2]

    # Entrenamos a la Single Neuron con la salida de la FC
    print("--- Start training and learning ---")
    singleNeuron = STDPLIFDensePopulation(in_channels=N_in, out_channels=N_out,
                                          weight=0.75, alpha=float(np.exp(-1e-3 / 10e-3)),
                                          beta=float(np.exp(-1e-3 / 2e-5)), delay=0,
                                          th=th, a_plus=.0010125, a_minus=.00125625,
                                          w_max=1.)

    # Pre-procesamos PSpikes y NSpikes
    dt_ltp = 15  # Cantidad de timesteps que miro hacia atras
    dt_ltd = 60  # Cantidad de timesteps que miro hacia delante
    PSpikes = preSpikes(T, dt_ltp, torch.zeros(T, N_in), Sin)
    NSpikes = nextSpikes(T, dt_ltd, torch.zeros(T, N_in), Sin)

    # Realizamos el entrenamiento STDP
    Uprobe = np.empty([T, N_out])
    Iprobe = np.empty([T, N_out])
    Sprobe = np.empty([T, N_out])

    n = 0
    t = 0
    c_l = 1.
    while (c_l > 0.01) and (t < 300000):
        state = singleNeuron.forward(Sin[n].unsqueeze(0), PSpikes[n], NSpikes[n - 1])
        Uprobe[n] = state.U.data.numpy()
        Iprobe[n] = state.I.data.numpy()
        Sprobe[n] = state.S.data.numpy()
        n += 1
        if n >= T:
            n = 0
        c_l = convergence_level(singleNeuron.fc_layer.weight[0])
        t += 1

    # Una vez finalizado el entrenamiento, obtenemos la salida sin entrenear
    print("--- Output and save training sequence ---")
    Uprobe = np.empty([T, N_out])
    Iprobe = np.empty([T, N_out])
    Sprobe = np.empty([T, N_out])
    for k in range(T):
        state = singleNeuron.forward_no_learning(Sin[k].unsqueeze(0))
        Uprobe[k] = state.U.data.numpy()
        Iprobe[k] = state.I.data.numpy()
        Sprobe[k] = state.S.data.numpy()

    # Save output
    fname = 'sn_output_' + '_th' + str_th
    Sin_train = torch.tensor(Sprobe).float()
    path_dir = './sequences/massive_runs/1/sn_outputs/' + folder + '/training/' + str(seed) + '/neurons-' + neurons + '/w-' + weight + 'th-' + umbral + '/'
    pathlib.Path(path_dir).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_train, path_dir + fname + '.pt')


    # Una vez finalizado el entrenamiento, obtenemos la salida de la secuencia de testing sin entrenear
    print("--- Output and save testing sequence ---")
    Sin = torch.load('./sequences/massive_runs/1/fc_outputs/' + folder + '/testing/' + seed + '/fc_output_' + neurons + '_w' + weight + '_th' + umbral + '.pt')
    T = Sin.shape[0]

    #singleNeuron.th = 3.5

    # Una vez finalizado el entrenamiento, obtenemos la salida sin entrenear
    Uprobe = np.empty([T, N_out])
    Iprobe = np.empty([T, N_out])
    Sprobe = np.empty([T, N_out])
    for j in range(T):
        state = singleNeuron.forward_no_learning(Sin[j].unsqueeze(0))
        Uprobe[j] = state.U.data.numpy()
        Iprobe[j] = state.I.data.numpy()
        Sprobe[j] = state.S.data.numpy()

    # Save output
    Sin_train = torch.tensor(Sprobe).float()
    path_dir = './sequences/massive_runs/1/sn_outputs/' + folder + '/testing/' + str(seed) + '/neurons-' + neurons + '/w-' + weight + 'th-' + umbral + '/'
    pathlib.Path(path_dir).mkdir(parents=True, exist_ok=True)
    torch.save(Sin_train, path_dir + fname + '.pt')

    print(fname)
    print("------------------------------------------")
