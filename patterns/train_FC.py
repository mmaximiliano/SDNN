import sv_utils
import torch 
import numpy as np
import pylab as plt
import argparse
import pathlib
from matplotlib import pyplot
from matplotlib.pyplot import figure
from os.path import dirname, realpath
from singleNeuron_omega import preSpikes, nextSpikes, STDPLIFDensePopulation, convergence_level, get_metrics, get_metrics_long_pat


#--- Parse Arguments ---#
parser = argparse.ArgumentParser()
parser.add_argument("-folder", "--folder", dest="folder", default="none", action='store', help="folder name", type=str)
parser.add_argument("-neurons", "--neurons", dest="neurons", default=1, action='store', help="Number of neurons", type=int)
parser.add_argument("-w", "--w", dest="w", default=0, action='store', help="Initial weight", type=int)
parser.add_argument("-seed", "--seed", dest="seed", default=0, action='store', help="Initial weight", type=int)


args = parser.parse_args()
folder = args.folder
seed = args.seed
neurons = args.neurons
weight = (args.w)/10


for th in [1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5]:
	# Prepare string for fname
	str_th = str(th)
	str_th = str_th[0] + '-' + str_th[2]
	str_w = str(weight)
	str_w = str_w[0] + '-' + str_w[2]


	# Prepare learning
	Sin = torch.load('./sequences/massive_runs/1/last_layer_results/' + folder + '/training/' + str(seed) +'/layer_5_conv.pt')
	T = Sin.shape[0]
	N_out = neurons
	N_in = Sin.shape[1]

	FC_layer = STDPLIFDensePopulation(in_channels=N_in, out_channels=N_out,
                                        weight=weight, alpha=float(np.exp(-1e-3/10e-3)),
                                        beta=float(np.exp(-1e-3/2e-5)), delay=0,
                                        th=th, omega=0.25, a_plus=.009125, a_minus=.0125625,
                                        w_max=1.)


	# pre-procesamos PSpikes y NSpikes
	dt_ltp = 20  # Cantidad de timesteps que miro hacia atras
	dt_ltd = 70  # Cantidad de timesteps que miro hacia delante
	PSpikes = preSpikes(T, dt_ltp, torch.zeros(T, N_in), Sin)
	NSpikes = nextSpikes(T, dt_ltd, torch.zeros(T, N_in), Sin)

	# Realizamos el entrenamiento STDP
	Uprobe_fc = np.empty([T, N_out])
	Iprobe_fc = np.empty([T, N_out])
	Sprobe_fc = np.empty([T, N_out])


	print("Start training")
	n = 0
	t = 0
	c_l = 1.
	while (c_l > 0.01) and (t < 400000):
		state = FC_layer.forward(Sin[n].unsqueeze(0), PSpikes[n], NSpikes[n-1])
		Uprobe_fc[n] = state.U.data.numpy()
		Iprobe_fc[n] = state.I.data.numpy()
		Sprobe_fc[n] = state.S.data.numpy()
		n += 1
		if n >= T:
			n = 0
		c_l = convergence_level(FC_layer.fc_layer.weight[0])
		t += 1
        
        
	print("Run and save training output")        
	# Una vez finalizado el entrenamiento, obtenemos la salida sin entrenear
	Uprobe_fc = np.empty([T, N_out])
	Iprobe_fc = np.empty([T, N_out])
	Sprobe_fc = np.empty([T, N_out])
	for n in range(T):
		state = FC_layer.forward_no_learning(Sin[n].unsqueeze(0))
		Uprobe_fc[n] = state.U.data.numpy()
		Iprobe_fc[n] = state.I.data.numpy()
		Sprobe_fc[n] = state.S.data.numpy()



	# Save output
	fname = 'fc_output_' + str(neurons) + '_w' + str_w + '_th' + str_th
	Sin_train = torch.tensor(Sprobe_fc).float()
	path_dir = './sequences/massive_runs/1/fc_outputs/' + folder + '/training/' + str(seed) + '/'
	pathlib.Path(path_dir).mkdir(parents=True, exist_ok=True)
	torch.save(Sin_train, path_dir + fname + '.pt')


	print("Run and save testing output")
	Sin = torch.load('./sequences/massive_runs/1/last_layer_results/' + folder + '/testing/' + str(seed) + '/layer_5_conv.pt')
	T = Sin.shape[0]         
	# Una vez finalizado el entrenamiento, obtenemos la salida sin entrenear para la secuencia de testing
	Uprobe_fc = np.empty([T, N_out])
	Iprobe_fc = np.empty([T, N_out])
	Sprobe_fc = np.empty([T, N_out])
	for n in range(T):
		state = FC_layer.forward_no_learning(Sin[n].unsqueeze(0))
		Uprobe_fc[n] = state.U.data.numpy()
		Iprobe_fc[n] = state.I.data.numpy()
		Sprobe_fc[n] = state.S.data.numpy()



	# Save output
	fname = 'fc_output_' + str(neurons) + '_w' + str_w + '_th' + str_th
	Sin_train = torch.tensor(Sprobe_fc).float()
	path_dir = './sequences/massive_runs/1/fc_outputs/' + folder + '/testing/' + str(seed) + '/'
	pathlib.Path(path_dir).mkdir(parents=True, exist_ok=True)
	torch.save(Sin_train, path_dir + fname + '.pt')

	print(fname)
	print("------------------------------------------")
