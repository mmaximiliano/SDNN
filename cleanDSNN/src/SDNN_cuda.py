import numpy as np
from math import floor, ceil
from os import listdir
from sys import exit
from numba import cuda
from cuda_utils import *

from timeit import default_timer as timer


class SDNN:
    """ 
        __author__ = Maximiliano Martino
        __email__ = maxii.martino@gmail.com
        
        This class implements a STDP-based Spiking Convolutional Deep Neural Network 
        for image or video recognition. This implementation is based on the implementation on [1]

        The input consists of up to M_in channels where the information on each channel 
        is coded in the spike times following a rank-order coding. 
        The input, of size H_in x W_in x M_in, passes through a set of convolutional-pooling
        layers which extract the features of the image.

        The training is done on each convolutional layer in an unsupervised manner following 
        an STDP rule. Only the convolution layer weights are updated following this rule.
        There is no training in the pooling layers.

        The neurons used are non-leaky integrate-and-fire (NL-IAF). The voltage V of the 
        neurons follows:

        V_i(t) = V_i(t-1)+ sum_j( W_i,j * S_j(t-1))         [1]

        Where i and j correspond to post-synaptic and pre-synaptic neurons respectively.
        S are the spikes times from the previous layer.

        After every voltage update (each time step) the weights are updated following:

        dw(i, j) = a_plus * w(i, j) * (1-w(i, j)) if t_j-t_i <= 0
        dw(i, j) = a_minus * w(i, j) * (1-w(i, j)) if t_j-t_i > 0        [1]

       where i and j correspond to post and pre synaptic neurons respectively and a is 
       the learning rate.
       Note that weights will always be in the interval [0, 1].

       The learning is done layer by layer. No layer will learn until the previous has 
       finished learning.


       References:
       [1] Kheradpisheh, S.R., et al. STDP-based spiking deep neural networks for object recognition.
            arXiv:1611.01421v1 (Nov, 2016)
    """

    def __init__(self, network_params, weight_params, stdp_params, frame_time,
                 spike_times_pat_seq=None, device='GPU'):
        """
            Initialisaition of SDNN

            Input:            
            - network_params: A list of dictionaries with the following keys:                
                -'Type': A string specifying which kind of layer this is (either 'input', 'conv' and 'pool')
                -'num_filters': an int specifying the depth (number of filters) of this layer
                -'filter_size': an int specifying the height and width of the filter window for 
                                the previous layer to this layer (only on 'conv' and  'pool')
                -'th': an np.float32 specifying the threshold of this layer (only on 'conv' and  'pool')
                -'stride': an int specifying the stride for this layer (only on 'pool')
                -'pad': an int specifying the pad for this layer (only on 'input')
                -'H_layer': an int specifying the height of this layer (only on 'input')
                -'W_layer': an int specifying the width of this layer (only on 'input') 
            - weight_params: A dictionary with the following keys:                
                - 'mean': the mean for initialising the weights
                - 'std': the std for initialising the weights
            - stdp_params: A dictionary with the following keys:                                
                - 'max_iter': an int specifyng the maximum number of iterations allowed on learning
                - 'max_learn_iter': a list of ints specifying the maximum number of iterations allowed for training each layer (len = number of layers)
                - 'stdp_per_layer': a list of ints specifying the maximum number of STDP updates per layer (len = number of layers)
                - 'offset_STDP': a list of ints specifying the STDP ofset per leayer updates per layer (len = number of layers)
                - 'a_minus': an np.float32 numpy array specifying the learning rate when no causality 
                - 'a_plus': an np.float32 numpy array specifying the learning rate when there is causality 
            - total_time: An int specifying the number of time steps per image
            - spike_times_learn: A list of strings with a valid absolute or relative path to the folders with 
                                 the learning .jpg images OR 
                                 An uint8 array with the learning spike times of shape (N_lr, H_in, W_in, M_in). 
                                 Axis 0 is each of the images
            - spike_times_train: A list of strings with a valid absolute or relative path to the folders with 
                                 the training .jpg images OR 
                                 An uint8 array with the training spike times of shape (N_tr, H_in, W_in, M_in). 
                                 Axis 0 is each of the images
            - spike_times_test: A list of strings with a valid absolute or relative path to the folders with 
                                 the testing .jpg images OR 
                                 An uint8 array with the testing spike times of shape (N_ts, H_in, W_in, M_in). 
                                 Axis 0 is each of the images   
            - DoG_params: None OR A dictionary with the following keys:
                -'img_size': A tuple of integers with the dimensions to which the images are to be resized 
                -'DoG_size': An int with the size of the DoG filter window size
                -'std1': A float with the standard deviation 1 for the DoG filter
                -'std2': A float with the standard deviation 2 for the DoG filter
        """

        # --------------------------- Network Initialisation -------------------#
        # Frame time and number of layers
        self.num_layers = len(network_params)
        self.learnable_layers = []
        self.frame_time = frame_time

        # Layers Initialisation
        self.network_struc = []
        self.init_net_struc(network_params)
        self.layers = []
        self.init_layers()

        # Weights Initialisation
        self.weight_params = weight_params
        self.weights = []
        self.init_weights()

        # Dimension Check
        self.check_dimensions()

        # ---------------------------Learning Paramters -------------------#
        # Learning layer parameters
        self.max_iter = stdp_params['max_iter']
        self.learning_layer = self.learnable_layers[0]
        self.max_learn_iter = stdp_params['max_learn_iter']
        self.curr_lay_idx = 0
        self.counter = 0
        self.curr_seq = 0

        #STDP params
        self.stdp_per_layer = stdp_params['stdp_per_layer']
        self.stdp_a_minus = stdp_params['a_minus']
        self.stdp_a_plus = stdp_params['a_plus']
        self.offsetSTDP = stdp_params['offset_STDP']

        # --------------------------- CUDA Parameters -------------------#
        self.device = device
        if self.device == 'GPU':
            self.thds_per_dim = 10  # (Use 8 if doesn't work)

        # --------------------------- Input spike times -------------------#
        # Generate Iterators with the full path to the images in each set OR reference the spike times
        self.spike_times_pat_seq = spike_times_pat_seq
        self.num_img_learn = 1  # len(listdir(spike_times_pat_seq)) Ver como automatizar
        self.sequence = np.load(self.spike_times_pat_seq + "seq_0.npy")
        # duration of sequences divided by duration of frame = Number of frames
        self.num_frame_train = int(self.sequence.shape[2] / self.frame_time)

        # --------------------------- Output features -------------------#
        self.features_train = []


# --------------------------- Initialisation functions ------------------------#
    # Network Structure Initialization
    def init_net_struc(self, network_params):
        """
            Network structure initialisation 
        """

        for i in range(self.num_layers):
            d_tmp = {}
            if network_params[i]['Type'] == 'input':
                d_tmp['Type'] = network_params[i]['Type']
                d_tmp['H_layer'] = network_params[i]['H_layer']
                d_tmp['W_layer'] = network_params[i]['W_layer']
                d_tmp['num_filters'] = network_params[i]['num_filters']
                d_tmp['pad'] = network_params[i]['pad']
                d_tmp['shape'] = (d_tmp['H_layer'], d_tmp['W_layer'], d_tmp['num_filters'])
            elif network_params[i]['Type'] == 'conv':
                d_tmp['Type'] = network_params[i]['Type']
                d_tmp['th'] = network_params[i]['th']
                d_tmp['filter_size'] = network_params[i]['filter_size']
                d_tmp['num_filters'] = network_params[i]['num_filters']
                d_tmp['pad'] = np.array([0, 0])
                d_tmp['stride'] = 1
                d_tmp['offset'] = floor(d_tmp['filter_size']/2)
                d_tmp['H_layer'] = int(1 + floor((self.network_struc[i-1]['H_layer']+2*d_tmp['pad'][0]-d_tmp['filter_size'])/d_tmp['stride']))
                d_tmp['W_layer'] = int(1 + floor((self.network_struc[i-1]['W_layer']+2*d_tmp['pad'][1]-d_tmp['filter_size'])/d_tmp['stride']))
                d_tmp['shape'] = (d_tmp['H_layer'], d_tmp['W_layer'], d_tmp['num_filters'])
                d_tmp['alpha'] = network_params[i]['alpha']
                d_tmp['beta'] = network_params[i]['beta']
                d_tmp['delay'] = network_params[i]['delay']
                self.learnable_layers.append(i)
            elif network_params[i]['Type'] == 'pool':
                d_tmp['Type'] = network_params[i]['Type']
                d_tmp['th'] = network_params[i]['th']
                d_tmp['filter_size'] = network_params[i]['filter_size']
                d_tmp['num_filters'] = network_params[i]['num_filters']
                d_tmp['pad'] = np.array([0, 0])
                d_tmp['stride'] = network_params[i]['stride']
                d_tmp['offset'] = floor(d_tmp['filter_size']/2)
                d_tmp['H_layer'] = int(1 + floor((self.network_struc[i-1]['H_layer']+2*d_tmp['pad'][0]-d_tmp['filter_size'])/d_tmp['stride']))
                d_tmp['W_layer'] = int(1 + floor((self.network_struc[i-1]['W_layer']+2*d_tmp['pad'][1]-d_tmp['filter_size'])/d_tmp['stride']))
                d_tmp['shape'] = (d_tmp['H_layer'], d_tmp['W_layer'], d_tmp['num_filters'])
            else:
                exit("unknown layer specified: use 'input', 'conv' or 'pool' ")
            self.network_struc.append(d_tmp)

    # Weights Initialization
    def init_weights(self):
        """
            Weight Initialization
        """
        mean = self.weight_params['mean']
        std = self.weight_params['std']
        for i in range(1, self.num_layers):
            HH = self.network_struc[i]['filter_size']
            WW = self.network_struc[i]['filter_size']
            MM = self.network_struc[i - 1]['num_filters']
            DD = self.network_struc[i]['num_filters']
            w_shape = (HH, WW, MM, DD)
            if self.network_struc[i]['Type'] == 'conv':
                weights_tmp = (mean + std * np.random.normal(size=w_shape))
                weights_tmp[weights_tmp >= 1.] = 0.99
                weights_tmp[weights_tmp <= 0.] = 0.01
            elif self.network_struc[i]['Type'] == 'pool':
                weights_tmp = np.ones((HH, WW, MM))/(HH*WW)
            else:
                continue
            self.weights.append(weights_tmp.astype(np.float32))

    # Dimension Checker
    def check_dimensions(self):
        """
            Checks the dimensions of the SDNN
        """
        for i in range(1, self.num_layers):
            H_pre, W_pre, M_pre = self.network_struc[i - 1]['shape']
            if self.network_struc[i]['Type'] == 'conv':
                HH, WW, MM, DD = self.weights[i-1].shape
            else:
                HH, WW, MM = self.weights[i-1].shape
            H_post, W_post, D_post = self.network_struc[i]['shape']
            stride = self.network_struc[i]['stride']
            H_pad, W_pad = self.network_struc[i]['pad']

            assert floor((H_pre + 2*H_pad - HH) / stride) + 1 == H_post, 'Error HEIGHT: layer %s to layer %s . ' \
                                                                    'Width does not work' % (i-1,  i)
            assert floor((W_pre + 2*W_pad - WW) / stride) + 1 == W_post, 'Error WIDTH: layer %s to layer %s . ' \
                                                                    'Width does not work' % (i-1,  i)
            assert MM == M_pre, 'Error in DEPTH of PREVIOUS map'
            if self.network_struc[i]['Type'] == 'conv':
                assert DD == D_post, 'Error in DEPTH of CURRENT map'

    # Initialise layers
    def init_layers(self):
        """
            Initialise layers         
        """
        for i in range(self.num_layers):
            d_tmp = {}
            H, W, D = self.network_struc[i]['shape']
            print("Layer " + str(i) + " Type: " + str(self.network_struc[i]['Type']) +
                  " -> H: " + str(H) + " W: " + str(W) + " D: " + str(D))
            d_tmp['S'] = np.zeros((H, W, D, self.frame_time)).astype(np.uint8)
            d_tmp['V'] = np.zeros((H, W, D, self.frame_time)).astype(np.float32)
            d_tmp['K_STDP'] = np.ones((H, W, D)).astype(np.uint8)
            d_tmp['K_inh'] = np.ones((H, W)).astype(np.uint8)
            if self.network_struc[i]['Type'] == 'conv':
                d_tmp['I'] = np.zeros((H, W, D, self.frame_time)).astype(np.float32)
                d_tmp['C'] = np.zeros((H, W, D)).astype(np.float32)  # Delay counter
            self.layers.append(d_tmp)
        return

    # Layers reset
    def reset_layers(self):
        """
            Reset layers         
        """
        for i in range(self.num_layers):
            H, W, D = self.network_struc[i]['shape']
            self.layers[i]['S'] = np.zeros((H, W, D, self.frame_time)).astype(np.uint8)
            self.layers[i]['V'] = np.zeros((H, W, D, self.frame_time)).astype(np.float32)
            self.layers[i]['K_STDP'] = np.ones((H, W, D)).astype(np.uint8)
            self.layers[i]['K_inh'] = np.ones((H, W)).astype(np.uint8)
            if self.network_struc[i]['Type'] == 'conv':
                self.layers[i]['I'] = np.zeros((H, W, D, self.frame_time)).astype(np.float32)
                self.layers[i]['C'] = np.zeros((H, W, D)).astype(np.float32)  # Reset delay counter
        return

    # Layers reset only spikes
    def reset_layers_spikes(self):
        """
            Reset layers
        """
        for i in range(self.num_layers):
            H, W, D = self.network_struc[i]['shape']
            self.layers[i]['S'] = np.zeros((H, W, D, self.frame_time)).astype(np.uint8)
            self.layers[i]['K_STDP'] = np.ones((H, W, D)).astype(np.uint8)
            self.layers[i]['K_inh'] = np.ones((H, W)).astype(np.uint8)
        return

    # Weights getter
    def get_weights(self):
        return self.weights

    # Weights setter
    def set_weights(self, path_list):
        """
            Sets the weights to the values specified in path_list

            Input:
            - path_list: A list of strings specifying the addresses to the weights to be set. These weights must be 
                         stored as *.npy                    
        """
        self.weights = []
        for id in range(self.num_layers-1):
            weight_tmp = np.load(path_list[id])
            self.weights.append(weight_tmp.astype(np.float32))
        return

# --------------------------- STDP Learning functions ------------------------#
    # Propagate and STDP once
    def prop_step(self, nlayer, stdp=True):
        """
            Propagates one image through the SDNN network and carries out the STDP update on the learning layer
        """

        # Propagate
        for t in range(1, self.frame_time):
            for i in range(1, nlayer):

                H, W, D = self.network_struc[i]['shape']
                H_pad, W_pad = self.network_struc[i]['pad']
                stride = self.network_struc[i]['stride']
                th = self.network_struc[i]['th']

                w = self.weights[i-1]
                s = self.layers[i - 1]['S'][:, :, :, t - 1]  # Input spikes
                s = np.pad(s, ((H_pad, H_pad), (W_pad, W_pad), (0, 0)), mode='constant')  # Pad the input
                S = self.layers[i]['S'][:, :, :, t]  # Output spikes
                V = self.layers[i]['V'][:, :, :, t - 1]  # Output voltage before
                K_inh = self.layers[i]['K_inh']  # Lateral inhibition matrix

                blockdim = (self.thds_per_dim, self.thds_per_dim, self.thds_per_dim)
                griddim = (int(ceil(H / blockdim[0])) if int(ceil(H / blockdim[2])) != 0 else 1,
                           int(ceil(W / blockdim[1])) if int(ceil(W / blockdim[2])) != 0 else 1,
                           int(ceil(D / blockdim[2])) if int(ceil(D / blockdim[2])) != 0 else 1)

                if self.network_struc[i]['Type'] == 'conv':
                    # Set conv params
                    alpha = self.network_struc[i]['alpha']
                    beta = self.network_struc[i]['beta']
                    delay = self.network_struc[i]['delay']
                    C = self.layers[i]['C'][:, :, :]  # Output delay counter before
                    I = self.layers[i]['I'][:, :, :, t - 1]  # Output voltage before
                    V, I, S, C = self.convolution(S, I, V, C, s, w, stride, th, alpha, beta, delay,
                                                      blockdim, griddim)
                    self.layers[i]['V'][:, :, :, t] = V
                    self.layers[i]['I'][:, :, :, t] = I
                    self.layers[i]['C'][:, :, :] = C

                    S, K_inh = self.lateral_inh(S, V, K_inh, blockdim, griddim)
                    self.layers[i]['S'][:, :, :, t] = S
                    self.layers[i]['K_inh'] = K_inh

                elif self.network_struc[i]['Type'] == 'pool':
                    S = self.pooling(S, s, w, stride, th, blockdim, griddim)
                    self.layers[i]['S'][:, :, :, t] = S

                    if i < 3:
                        S, K_inh = self.lateral_inh(S, V, K_inh, blockdim, griddim)
                        self.layers[i]['S'][:, :, :, t] = S
                        self.layers[i]['K_inh'] = K_inh
                if t == (self.frame_time-1):
                    print("Layer " + str(i) + ' ' + str(self.network_struc[i]['Type']) + " spikes: "
                          + str(np.count_nonzero(self.layers[i]['S'])))

            if stdp:
                # STDP learning
                lay = self.learning_layer
                if self.network_struc[lay]['Type'] == 'conv':

                    # valid are neurons in the learning layer that can do STDP and that have fired in the current t
                    S = self.layers[lay]['S'][:, :, :, t]  # Output spikes
                    V = self.layers[lay]['V'][:, :, :, t]  # Output voltage
                    K_STDP = self.layers[lay]['K_STDP']  # Lateral inhibition matrix
                    valid = S*V*K_STDP

                    if np.count_nonzero(valid) > 0:

                        H, W, D = self.network_struc[lay]['shape']
                        stride = self.network_struc[lay]['stride']
                        offset = self.offsetSTDP[lay]
                        a_minus = self.stdp_a_minus[lay]
                        a_plus = self.stdp_a_plus[lay]

                        s = self.layers[lay - 1]['S'][:, :, :, :t]  # Input spikes
                        ssum = np.sum(s, axis=3)
                        s = np.pad(ssum, ((H_pad, H_pad), (W_pad, W_pad), (0, 0)), mode='constant')  # Pad the input
                        w = self.weights[lay - 1]

                        maxval, maxind1, maxind2 = self.get_STDP_idxs(valid, H, W, D, lay)

                        blockdim = (self.thds_per_dim, self.thds_per_dim, self.thds_per_dim)
                        griddim = (int(ceil(H / blockdim[0])) if int(ceil(H / blockdim[2])) != 0 else 1,
                                   int(ceil(W / blockdim[1])) if int(ceil(W / blockdim[2])) != 0 else 1,
                                   int(ceil(D / blockdim[2])) if int(ceil(D / blockdim[2])) != 0 else 1)

                        w, K_STDP = self.STDP(S.shape, s, w, K_STDP,
                                              maxval, maxind1, maxind2,
                                              stride, offset, a_minus, a_plus, blockdim, griddim)
                        self.weights[lay - 1] = w
                        self.layers[lay]['K_STDP'] = K_STDP

    # Train all images in training set
    def train_SDNN(self):
        """
            Trains the SDNN with the learning set of sequences
            
            We iterate over the set of sequences a maximum of self.max_iter times
        """

        print("-----------------------------------------------------------")
        print("-------------------- STARTING LEARNING---------------------")
        print("-----------------------------------------------------------")

        frame = 0
        for i in range(self.max_iter):
            print("----------------- Learning Progress  {}%----------------------".format(str(i) + '/'
                                                                                          + str(self.max_iter)
                                                                                          + ' ('
                                                                                          + str(100 * i / self.max_iter)
                                                                                          + ')'))
            # Dentro del total de iteraciones veo cuantas le corresponden a cada layer
            # Me fijo si ya realice todas las iteraciones de este layer
            if self.counter > self.max_learn_iter[self.learning_layer]:
                self.curr_lay_idx += 1  # Paso al siguiente layer
                self.learning_layer = self.learnable_layers[self.curr_lay_idx]  # Actualizo el learning layer actual
                self.counter = 0  # Reseteo el contador para este layer
            self.counter += 1  # Caso contrario aumento el contador

            self.reset_layers()  # Reset all layers for the new frame

            st = self.sequence[:, :, frame:frame+self.frame_time]  # Agarro un frame de 10 timestep
            st = np.expand_dims(st, axis=2)
            self.layers[0]['S'] = st  # (H, W, M, time)
            self.prop_step(nlayer=self.learning_layer+1, stdp=True)
            # Tomo el siguiente frame de la secuencia
            if frame >= (self.sequence.shape[2] - (2*self.frame_time)):
                frame = 0
            else:
                frame += self.frame_time

            if i % 500 == 0:
                self.stdp_a_plus[self.learning_layer] = min(2.*self.stdp_a_plus[self.learning_layer], 0.15)
                self.stdp_a_minus[self.learning_layer] = 0.75*self.stdp_a_plus[self.learning_layer]

            # Miro la cantidad de img/seq que tengo por procesar
            if self.curr_seq+1 < self.num_img_learn:
                self.curr_seq += 1
            else:
                self.curr_seq = 0
        print("----------------- Learning Progress  {}%----------------------".format(str(self.max_iter) + '/'
                                                                                      + str(self.max_iter)
                                                                                      + ' ('
                                                                                      + str(100)
                                                                                      + ')'))
        print("-----------------------------------------------------------")
        print("------------------- LEARNING COMPLETED --------------------")
        print("-----------------------------------------------------------")

    # Find STDP update indices and potentials
    def get_STDP_idxs(self, valid, H, W, D, layer_idx):
        """
            Finds the indices and potentials of the post-synaptic neurons to update. 
            Only one update per map (if allowed) 
        """

        i = layer_idx
        STDP_counter = 1

        mxv = np.amax(valid, axis=2)
        mxi = np.argmax(valid, axis=2)

        maxind1 = np.ones((D, 1)) * -1
        maxind2 = np.ones((D, 1)) * -1
        maxval = np.ones((D, 1)) * -1

        while np.sum(np.sum(mxv)) != 0.:
            # for each layer a certain number of neurons can do the STDP per image
            if STDP_counter > self.stdp_per_layer[i]:
                break
            else:
                STDP_counter += 1

            maximum = np.amax(mxv, axis=1)
            index = np.argmax(mxv, axis=1)

            index1 = np.argmax(maximum)
            index2 = index[index1]

            maxval[mxi[index1, index2]] = mxv[index1, index2]
            maxind1[mxi[index1, index2]] = index1
            maxind2[mxi[index1, index2]] = index2

            mxv[mxi == mxi[index1, index2]] = 0
            mxv[max(index1 - self.offsetSTDP[layer_idx], 0):min(index1 + self.offsetSTDP[layer_idx], H) + 1,
                max(index2 - self.offsetSTDP[layer_idx], 0):min(index2 + self.offsetSTDP[layer_idx], W) + 1] = 0

        maxval = np.squeeze(maxval).astype(np.float32)
        maxind1 = np.squeeze(maxind1).astype(np.int16)
        maxind2 = np.squeeze(maxind2).astype(np.int16)

        return maxval, maxind1, maxind2

# --------------------------- Propagation functions ------------------------#

    # Get training features
    def train_features(self):
        """
            Gets the train features by propagating the set of training images
            Returns:
                - X_train: Training features of size (N, M)
                            where N is the number of training samples
                            and M is the number of maps in the last layer
        """
        self.network_struc[3]['th'] = 7.
        print("-----------------------------------------------------------")
        print("----------- EXTRACTING TRAINING FEATURES ------------------")
        print("-----------------------------------------------------------")

        frame = 0
        for i in range(self.num_frame_train):
            print("------------ Train features Extraction Progress  {}%----------------".format(str(i) + '/'
                                                                                                + str(self.num_frame_train)
                                                                                                + ' ('
                                                                                                + str(100 * i / self.num_frame_train)
                                                                                                + ')'))

            start = timer()

            self.reset_layers()  # Reset all layers for the new frame

            st = self.sequence[:, :, frame:frame+self.frame_time]  # Agarro un frame de 10 timestep
            st = np.expand_dims(st, axis=2)
            self.layers[0]['S'] = st  # (H, W, M, time)
            # Propagates one image through the SDNN network.
            # Here no STDP takes place and we always reach the last layer.
            self.prop_step(nlayer=self.num_layers, stdp=True)

            # Tomo el siguiente frame de la secuencia
            if frame >= (self.sequence.shape[2] - (2*self.frame_time)):
                frame = 0
            else:
                frame += self.frame_time

            S_tmp = self.layers[self.num_layers-1]['S']
            S = np.reshape(S_tmp, (S_tmp.shape[0]*S_tmp.shape[1]*S_tmp.shape[2], self.frame_time))
            S = np.transpose(S)
            S = S.astype(np.float32)

            self.features_train.append(S)


            dt = timer() - start


        print("------------ Train features Extraction Progress  {}%----------------".format(str(self.num_frame_train)
                                                                                            + '/'
                                                                                            + str(self.num_frame_train)
                                                                                            + ' ('
                                                                                            + str(100)
                                                                                            + ')'))
        print("-----------------------------------------------------------")
        print("------------- TRAINING FEATURES EXTRACTED -----------------")
        print("-----------------------------------------------------------")

        # Transform features to numpy array
        Sin = np.concatenate(self.features_train, axis=0)
        print("Sin Shape: " + str(Sin.shape))
        print("Sin Spikes: " + str(np.count_nonzero(Sin)))
        # Clear Features
        self.features_train = []
        return Sin

# --------------------------- CUDA interfacing functions ------------------------#
    def convolution(self, S, I, V, C, s, w, stride, th, alpha, beta, delay, blockdim, griddim):
        """
            Cuda Convolution Kernel call
            Returns the updated potentials and spike times
        """
        d_S = cuda.to_device(np.ascontiguousarray(S).astype(np.uint8))
        d_I = cuda.to_device(np.ascontiguousarray(I).astype(np.float32))
        d_V = cuda.to_device(np.ascontiguousarray(V).astype(np.float32))
        d_C = cuda.to_device(np.ascontiguousarray(C).astype(np.float32))
        d_s = cuda.to_device(np.ascontiguousarray(s).astype(np.uint8))
        d_w = cuda.to_device(np.ascontiguousarray(w).astype(np.float32))
        V_out = np.empty(d_V.shape, dtype=d_V.dtype)
        I_out = np.empty(d_I.shape, dtype=d_I.dtype)
        S_out = np.empty(d_S.shape, dtype=d_S.dtype)
        C_out = np.empty(d_C.shape, dtype=d_C.dtype)
        conv_step[griddim, blockdim](d_S, d_I, d_V, d_C, d_s, d_w, stride, th, alpha, beta, delay)
        d_V.copy_to_host(V_out)
        d_I.copy_to_host(I_out)
        d_S.copy_to_host(S_out)
        d_C.copy_to_host(C_out)
        return V_out, I_out, S_out, C_out

    def lateral_inh(self, S, V, K_inh, blockdim, griddim):
        """
            Cuda Lateral Inhibition Kernel call
            Returns the updated spike times and inhibition matrix
        """
        d_S = cuda.to_device(np.ascontiguousarray(S).astype(np.uint8))
        d_V = cuda.to_device(np.ascontiguousarray(V).astype(np.float32))
        d_K_inh = cuda.to_device(np.ascontiguousarray(K_inh).astype(np.uint8))
        S_out = np.empty(d_S.shape, dtype=d_S.dtype)
        K_inh_out = np.empty(d_K_inh.shape, dtype=d_K_inh.dtype)
        lateral_inh[griddim, blockdim](d_S, d_V, d_K_inh)
        d_S.copy_to_host(S_out)
        d_K_inh.copy_to_host(K_inh_out)
        return S_out, K_inh_out

    def pooling(self, S, s, w, stride, th, blockdim, griddim):
        """
            Cuda Pooling Kernel call
            Returns the updated spike times
        """
        d_S = cuda.to_device(np.ascontiguousarray(S).astype(np.uint8))
        d_s = cuda.to_device(np.ascontiguousarray(s).astype(np.uint8))
        d_w = cuda.to_device(np.ascontiguousarray(w).astype(np.float32))
        S_out = np.empty(d_S.shape, dtype=d_S.dtype)
        pool[griddim, blockdim](d_S, d_s, d_w, stride, th)
        d_S.copy_to_host(S_out)
        return S_out

    def STDP(self, S_sz, s, w, K_STDP, maxval, maxind1, maxind2, stride, offset, a_minus, a_plus, blockdim, griddim):
        """
            Cuda STDP-Update Kernel call
            Returns the updated weight and STDP allowed matrix
        """
        d_S_sz = cuda.to_device(np.ascontiguousarray(S_sz).astype(np.int32))
        d_s = cuda.to_device(np.ascontiguousarray(s).astype(np.uint8))
        d_w = cuda.to_device(w.astype(np.float32))
        d_K_STDP = cuda.to_device(K_STDP.astype(np.uint8))
        w_out = np.empty(d_w.shape, dtype=d_w.dtype)
        K_STDP_out = np.empty(d_K_STDP.shape, dtype=d_K_STDP.dtype)
        STDP_learning[griddim, blockdim](d_S_sz, d_s, d_w, d_K_STDP,  # Input arrays
                      maxval, maxind1, maxind2,  # Indices
                      stride, int(offset), a_minus, a_plus)  # Parameters
        d_w.copy_to_host(w_out)
        d_K_STDP.copy_to_host(K_STDP_out)
        return w_out, K_STDP_out
