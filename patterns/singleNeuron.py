import torch.nn as nn
import torch
import numpy as np
from collections import namedtuple

NeuronState = namedtuple('NeuronState', ['U', 'I', 'S', 'D', 'C'])

class LIFDensePopulation(nn.Module):
    """
        __author__ = Maximiliano Martino
        __email__ = maxii.martino@gmail.com

        This class implements a STDP-based Single Spiking Neuron

        The input consists of M_in channels where the information on each channel
        is coded in spike times

        The training is done in an unsupervised manner following an STDP rule.

        The neuron used is a leaky integrate-and-fire (L-IAF)  The membrance U of the
        neurons follows:

        π_π[π+1]= πΌππ[π]+πΌπ[π]βππ[π]
        πΌ_π[π+1]= π½πΌπ[π]+ βπ πππ ππ[π] + π

        Where i and j correspond to post-synaptic and pre-synaptic neurons respectively.
        S are the spikes times from the previous Neuron.

        After every voltage update (each time step) the weights are updated following:

        dw(i, j) = a_plus * w(i, j) * (1-w(i, j)) if t_j-t_i <= 0
        dw(i, j) = a_minus * w(i, j) * (1-w(i, j)) if t_j-t_i > 0        [1]

       where i and j correspond to post and pre synaptic neurons respectively and a is
       the learning rate.
       Note that weights will always be in the interval [0, 1].
    """

class LIFDensePopulation(nn.Module):
    NeuronState = namedtuple('NeuronState', ['U', 'I', 'S', 'D', 'C'])
    def __init__(self, in_channels, out_channels,  weight=.13, bias=False, alpha = .9, beta=.85, delay=0, th=1.):
        super(LIFDensePopulation, self).__init__()
        self.fc_layer = nn.Linear(in_channels, out_channels)
        self.in_channels = in_channels
        self.out_channels = out_channels
        self.alpha = alpha
        self.beta = beta
        self.delay = delay
        self.th = th
        self.state = state = self.NeuronState(U=torch.zeros(1, out_channels),
                                              I=torch.zeros(1, out_channels),
                                              S=torch.zeros(1, out_channels),
                                              D=delay,
                                              C=torch.zeros(1, out_channels))
        self.fc_layer.weight.data.uniform_(weight, weight)
        self.fc_layer.bias.data.uniform_(0., 0.)

    def forward(self, Sin_t):
        state = self.state
        U = self.alpha*state.U + state.I - (state.S * state.U)
        I = self.beta*state.I + self.fc_layer(Sin_t)
        # update the neuronal state
        S = (U > self.th).float()
        delayedS = S
        C = state.C

        if state.D != 0:
            S = S * (state.C <= 0).float()

            delayedC = (state.C > 0).float()
            delayedS = (state.C == 1).float()

            pause = (state.C > 0).float()
            play = (pause == 0).float()

            prevU = state.U * pause
            U = U * play
            U = U + prevU

            prevI = state.I * (pause + (S-delayedS))

            I = I * (play - S)
            I = I + prevI

            C = state.C - 1
            C = C * delayedC
            C = C + (state.D * S)

        self.state = NeuronState(U=U, I=I, S=delayedS, D=state.D, C=C)
        return self.state

# A STDP LIF neuron model derived from the LIF Neuron
# It overrides the synaptic weights update operations (previously empty) to perform LTP or LTD
class STDPLIFDensePopulation(LIFDensePopulation):
    def __init__(self, in_channels, out_channels, weight=.13, bias=False, alpha=.9, beta=.85, delay=0, th=1.,
                 a_plus=0.03125, a_minus=0.0265625, tau_plus=16.8, tau_minus=33.7, w_max=1., norm=False):

        # STDP parameters
        self.a_plus = a_plus
        self.tau_plus = tau_plus
        self.a_minus = a_minus
        self.tau_minus = tau_minus
        self.w_max = w_max
        self.norm = norm

        # Call the parent contructor
        super(STDPLIFDensePopulation, self).__init__(in_channels, out_channels, weight, bias=False,
                                                     alpha = alpha, beta = beta , delay = delay, th = th)

    # Long Term synaptic Potentiation
    def LTP_op(self, nState, PSpikes):

        # Solo consideramos los ultimos dt spikes provenientes de cada synapse en la memoria
        # Como caso de prueba supongamos que solo miramos 1 t hacia atras
        last_spikes_op = PSpikes # Distancia en tiempo al ultimo spike
        # Reward all last synapse spikes that happened after the previous neutron spike
        rewards_op = torch.where(torch.logical_and(last_spikes_op, nState.S),
                                 self.a_plus * torch.exp(torch.negative(last_spikes_op)/self.tau_plus).double(),
                                 0.)

        # Obtenemos los nuevos pesos
        new_w_op = torch.add(self.fc_layer.weight[0], rewards_op)

        # Actualizamos el valor de los nuevos pesos
        # Nos aseguramos que esten en el rango [0,1]
        with torch.no_grad():
            self.fc_layer.weight[0] = torch.clip(new_w_op, 0.0, self.w_max)

    # Long Term synaptic Depression
    def LTD_op(self, nState, NSpikes, PSpikes):
        next_spikes_op = NSpikes # Distancia en tiempo al ultimo spike
        # Infligimos penalties en los nuevos spikes de sinapsis que no hicieron spike
        # La penalidad es la misma para todos los nuevos spikes, e inversamentente exponencial
        # al tiempo desde el ultimo spike
        # de momento asumimos que le pifiamos por dt
        penalties_op = torch.where(torch.logical_and(nState.S,
                                                     torch.logical_and(next_spikes_op,
                                                                       torch.logical_not(PSpikes))),
                                   self.a_minus * torch.exp(torch.negative(NSpikes/self.tau_minus)).double(),
                                   0.)

        # Evaluate new weights
        new_w_op = torch.subtract(self.fc_layer.weight[0], penalties_op)
        # Update with new weights clamped to [0,1]
        with torch.no_grad():
            self.fc_layer.weight[0] = torch.clip(new_w_op, 0.0, self.w_max)

    def normalize(self, norm):
        if norm:
            with torch.no_grad():
                self.fc_layer.weight = nn.Parameter(torch.div(self.fc_layer.weight, torch.sum(self.fc_layer.weight)))

    # Redefinimos forward, donde PSpikes son los dt spikes previos
    def forward(self, Sin_t, PSpikes, NSpikes):
        nState = super(STDPLIFDensePopulation, self).forward(Sin_t)  # Calculo el estado actual
        self.LTP_op(nState, PSpikes)
        self.LTD_op(nState, NSpikes, PSpikes)
        self.normalize(self.norm)
        return self.state

    def forward_no_learning(self, Sin_t):
        nState = super(STDPLIFDensePopulation, self).forward(Sin_t)  # Calculo el estado actual
        return self.state


# -------- Definimos las funciones para pre-procesar los spikes ---------- #
def preSpikes(t, dt, PSpikes, Sin):
    for i in range(t):
        PSpikes[i] = torch.where(Sin[i] == 1,
                                 i,
                                 0)
    for i in range(1, t):
        PSpikes[i] = torch.where(torch.logical_and(PSpikes[i] == 0,
                                                   torch.logical_and(i - PSpikes[i-1] >= 0,
                                                                     i - PSpikes[i-1] <= dt)),
                                 PSpikes[i-1],
                                 PSpikes[i])
    for i in range(1, t):
        PSpikes[i] = torch.where(PSpikes[i] > 0,
                                 i - PSpikes[i],  # Fijarse si no hay que poner (i-1)
                                 PSpikes[i])
    return PSpikes

def nextSpikes(t, dt, NSpikes, Sin):
    for i in range(t-1 , -1, -1):
        NSpikes[i] = torch.where(Sin[i] == 1,
                                 i,
                                 0)
    for i in range(t-2, -1, -1):
        NSpikes[i] = torch.where(torch.logical_and(NSpikes[i] == 0,
                                                   torch.logical_and(NSpikes[i+1] - i >= 0,
                                                                     NSpikes[i+1] - i <= dt)),
                                 NSpikes[i+1],
                                 NSpikes[i])
    for i in range(1, t):
        NSpikes[i] = torch.where(NSpikes[i] > 0,
                                 NSpikes[i] - i,  # Fijarse si no hay que poner (i-1)
                                 NSpikes[i])
    return NSpikes

def convergence_level(w):
    c_l = 0.
    for i in w:
        c_l += i.item()*(1-i.item())
    c_l = c_l/len(w)
    return c_l

def get_metrics(T, pat_times, Sprobe):
    tp, tn, fp, fn, p, n = 0, 0, 0, 0, 0, 0
    for i in range(0, T, 15):
        pat = pat_times[i:i+15]
        spikes = Sprobe[i:i+15]
        if np.count_nonzero(pat):
            p+=1
        else:
            n+=1
        if (not np.count_nonzero(pat)) and (not np.count_nonzero(spikes)):
            tn+=1
        elif (np.count_nonzero(pat)) and (np.count_nonzero(spikes)):
            tp+=1
        elif (not np.count_nonzero(pat)) and (np.count_nonzero(spikes)):
            fp+=1
        elif (np.count_nonzero(pat)) and (not np.count_nonzero(spikes)):
            fn+=1
    accuracy =(tp+tn)/(p+n)
    precision = tp/(tp+fp)
    recall = tp/(tp+fn)
    f1 = (2*tp)/(2*tp+fp+fn)
    fake_alarms = fp/n
    missed_alarms = fn/p

    return accuracy, precision, recall, f1, fake_alarms, missed_alarms