
class SingleNeuron:
    """
        __author__ = Maximiliano Martino
        __email__ = maxii.martino@gmail.com

        This class implements a STDP-based Single Spiking Neuron

        The input consists of M_in channels where the information on each channel
        is coded in spike times

        The training is done in an unsupervised manner following an STDP rule.

        The neuron used is a leaky integrate-and-fire (L-IAF)  The membrance U of the
        neurons follows:

        𝑈_𝑖[𝑛+1]= 𝛼𝑈𝑖[𝑛]+𝐼𝑖[𝑛]−𝑆𝑖[𝑛]
        𝐼_𝑖[𝑛+1]= 𝛽𝐼𝑖[𝑛]+ ∑𝑗 𝑊𝑖𝑗 𝑆𝑗[𝑛] + 𝑏

        Where i and j correspond to post-synaptic and pre-synaptic neurons respectively.
        S are the spikes times from the previous Neuron.

        After every voltage update (each time step) the weights are updated following:

        dw(i, j) = a_plus * w(i, j) * (1-w(i, j)) if t_j-t_i <= 0
        dw(i, j) = a_minus * w(i, j) * (1-w(i, j)) if t_j-t_i > 0        [1]

       where i and j correspond to post and pre synaptic neurons respectively and a is
       the learning rate.
       Note that weights will always be in the interval [0, 1].
    """

    def __init__(self, inputSpikeTrain, weight_params, stdp_params):
        self.total_time = inputSpikeTrain.size



















