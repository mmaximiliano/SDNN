import torch 
import numpy as np
import pylab as plt
from matplotlib import pyplot
from matplotlib.pyplot import figure
from os.path import dirname, realpath
import pathlib
import sv_utils
from singleNeuron import preSpikes, nextSpikes, STDPLIFDensePopulation, convergence_level, get_metrics, get_metrics_long_pat

for exp in ['/0_0_0']:
    for sample in ['p_18', 'p_18_5', 'p_18_20']:
        for th in [3.0]:
            for run in [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]:
                    #---------- Load snn spikes ------------#
                    path = './patterns/sequences/massive_runs/18/'
                    Sin_out_seq = torch.load(path + '0_0_0/last_layer_results/' + sample + '/training/'+ str(run) +'/layer_5_conv.pt')                
                    Sin_out_delay = torch.load(path + '15_0_0/last_layer_results/' + sample + '/training/'+ str(run) +'/layer_5_conv.pt')                
                    combined_out = torch.cat((Sin_out_seq, Sin_out_delay), 1)

                    #--------- Train Single Neuron ------------#
                    Sin = combined_out.clone().detach()
                    T = Sin.shape[0]
                    N_out = 1
                    N_in = Sin.shape[1]
                    th = th

                    singleNeuron = STDPLIFDensePopulation(in_channels=N_in, out_channels=N_out,
                                                            weight=0.70, alpha=float(np.exp(-1e-3/10e-3)),
                                                            beta=float(np.exp(-1e-3/2e-5)), delay=0,
                                                            th=th, a_plus=.009125, a_minus=.0125625,
                                                            w_max=1.)


                    # Pre-procesamos PSpikes y NSpikes
                    dt_ltp = 20  # Cantidad de timesteps que miro hacia atras
                    dt_ltd = 70  # Cantidad de timesteps que miro hacia delante
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
                        state = singleNeuron.forward(Sin[n].unsqueeze(0), PSpikes[n], NSpikes[n-1])
                        Uprobe[n] = state.U.data.numpy()
                        Iprobe[n] = state.I.data.numpy()
                        Sprobe[n] = state.S.data.numpy()
                        n += 1
                        if n >= T:
                            n = 0
                        c_l = convergence_level(singleNeuron.fc_layer.weight[0])
                        t += 1


                    #----------- Test Single Neuron Output -------------#
                    Sin_out_seq = torch.load(path + '0_0_0/last_layer_results/' + sample + '/testing/'+ str(run) +'/layer_5_conv.pt')                                
                    Sin_out_delay = torch.load(path + '15_0_0/last_layer_results/' + sample + '/testing/'+ str(run) +'/layer_5_conv.pt')                
                    combined_out = torch.cat((Sin_out_seq, Sin_out_delay), 1)
                    
                    pat_times = np.load(path + 'pat_sequences/' + sample + '/testing/'+ str(run) +'/pat_test_' + sample + '.npy')                    
                        
                    Sin = combined_out.clone().detach()
                    T = Sin.shape[0]
                    N_out = 1
                    N_in = Sin.shape[1]

                    # Corremos la secuencia de testing sin aprendizaje
                    Uprobe = np.empty([T, N_out])
                    Iprobe = np.empty([T, N_out])
                    Sprobe = np.empty([T, N_out])
                    for n in range(T):
                        state = singleNeuron.forward_no_learning(Sin[n].unsqueeze(0))
                        Uprobe[n] = state.U.data.numpy()
                        Iprobe[n] = state.I.data.numpy()
                        Sprobe[n] = state.S.data.numpy()

                    print('Exp: ' + exp + ' Sample: '+ sample + ' Run: '  + str(run) + ' th: ' + str(th) )
                    accuracy, precision, recall, f1, fake_alarms, missed_alarms, fake_spikes_numbers = get_metrics_long_pat(T - 15, pat_times, Sprobe, 30, [])
                    print("accuracy: " + str(accuracy))
                    print("precision: " + str(precision))
                    print("recall: " + str(recall))
                    print("f1: " + str(f1))
                    print("fake_alarms: " + str(fake_alarms))
                    print("missed_alarms: " + str(missed_alarms))
                    print('  ')
            
                    #----------- Save Results -----------#
                    str_th = str(th)
                    str_th = str_th[0] + '_' + str_th[2]
                    location = path + 'parallel_results/metrics/' + sample + '/' + str(run) + '/'
                    metrics = np.array([accuracy, precision, recall, f1, fake_alarms, missed_alarms], dtype=float)
                    pathlib.Path(location).mkdir(parents=True, exist_ok=True)
                    np.save(location + 'test_metrics_' + str_th + '.npy', metrics)
            
                    #----------- Test Dif Sample Single Neuron Output -------------#
                    Sin_out_seq = torch.load(path + '0_0_0/last_layer_results/' + sample + '/dif_sample/'+ str(run) +'/layer_5_conv.pt')                                
                    Sin_out_delay = torch.load(path + '15_0_0/last_layer_results/' + sample + '/dif_sample/'+ str(run) +'/layer_5_conv.pt')                
                    combined_out = torch.cat((Sin_out_seq, Sin_out_delay), 1)
                    
                    pat_times = np.load(path + 'pat_sequences/p_18_dif_sample/pat_p_18_dif_sample.npy')                    
                        
                    Sin = combined_out.clone().detach()
                    T = Sin.shape[0]
                    N_out = 1
                    N_in = Sin.shape[1]
                    

                    # Corremos la secuencia de Dif_sample (sin restricciones) sin aprendizaje
                    Uprobe = np.empty([T, N_out])
                    Iprobe = np.empty([T, N_out])
                    Sprobe = np.empty([T, N_out])
                    for n in range(T):
                        state = singleNeuron.forward_no_learning(Sin[n].unsqueeze(0))
                        Uprobe[n] = state.U.data.numpy()
                        Iprobe[n] = state.I.data.numpy()
                        Sprobe[n] = state.S.data.numpy()

                    print('(Dif Sample) Exp: ' + exp + ' Sample: '+ sample + ' Run: '  + str(run) + ' th: ' + str(th) )
                    accuracy, precision, recall, f1, fake_alarms, missed_alarms, fake_spikes_numbers = get_metrics_long_pat(T - 15, pat_times, Sprobe, 30, [])
                    print("accuracy: " + str(accuracy))
                    print("precision: " + str(precision))
                    print("recall: " + str(recall))
                    print("f1: " + str(f1))
                    print("fake_alarms: " + str(fake_alarms))
                    print("missed_alarms: " + str(missed_alarms))
                    print('  ')
        
                    #----------- Save Results -----------#
                    location = path + 'metrics/' + sample + '/' + str(run) + '/'
                    metrics = np.array([accuracy, precision, recall, f1, fake_alarms, missed_alarms], dtype=float)
                    pathlib.Path(location).mkdir(parents=True, exist_ok=True)
                    np.save(location + 'dif_sample_metrics_' + str_th + '.npy', metrics)
                