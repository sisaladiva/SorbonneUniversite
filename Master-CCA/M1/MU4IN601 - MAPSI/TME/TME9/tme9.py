import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import pickle as pkl



def labels_tobinary(Y, cl):
    Y_binary=[1 if label == cl else 0 for label in Y]
    return Y_binary 
    

def pred_lr(X, w,b):
    res = 1 / (1 + np.exp(-(np.dot(X, w) + b)))
    return res

def classify_binary(Y_pred):
    Y_pred_binary = (Y_pred > 0.5).astype(float)
    return Y_pred_binary

def accuracy(Y_predb, Y_c):
    accuracy = (np.array([1 if Y_predb[i] == Y_c[i] else 0 for i in range (len(Y_predb))])).mean()
    return accuracy

def rl_gradient_ascent(X,Y,eta = 1e-4, niter_max=300):
    N,d = np.shape(X)
    w = np.zeros(d)
    b = 0
    acc = []
    
    for i in range(niter_max):
        pred = 1 / (1 + np.exp(-(np.dot(X,w)+b)))
        wNew= w + eta *np.dot(X.T , (Y - pred)) 
        b = b + eta * (Y - pred).sum()
        
        Y_p = classify_binary(pred_lr(X, w,b))
        acc.append( accuracy(Y_p,Y) )
            
        if np.abs(wNew - w).max() < 1e-5:
            return w,b, acc, i
        w = wNew
    
    return w,b, acc, i 
    

def visualization(w):
    plt.figure()
    plt.imshow(w.reshape(16,16), cmap='gray')

def rl_gradient_ascent_one_against_all(X,Y,epsilon = 1e-4, niter_max=1000):
    N,d = np.shape(X)
    classes = np.unique(Y)

    W = []
    B = []
    for c in classes:
        Y_tmp = np.where(Y == c, 1., 0.)
        w , b, acc, it = rl_gradient_ascent(X,Y_tmp,epsilon,niter_max)
        W.append(w)
        B.append(b)
        print("classe ",c, ": accuracy = ",np.round(acc[-1]*100, 2),"%")
    return np.asarray(W).T, np.asarray(B).T

def classif_multi_class(Y_pred):
    classes = []
    for Y in Y_pred:
        classes.append(np.argmax(Y))
    return classes

def normalize(X):
    l,c = np.shape(X)
    return np.array([np.array(x-np.ones(c)) for x in X])

def pred_lr_multi_class(X, w, b):
    z = np.dot(X, w) + b
    exp_z = np.exp(z - np.max(z, axis=1, keepdims=True))
    return exp_z / np.sum(exp_z, axis=1, keepdims=True)

def to_categorical(y, num_classes):
    """ 1-hot encodes a tensor """
    return np.eye(num_classes, dtype='uint8')[y]


def rl_gradient_ascent_multi_class(X, Y, eta=0.2, numEp=1000, verbose=1):
    m, n = X.shape
    num_classes = len(np.unique(Y))

    
    W = np.ones((n, num_classes))
    b = np.ones((1,num_classes))
    Yc = to_categorical(Y, num_classes)
    
    
    for episode in range(numEp):

        Y_hat = pred_lr_multi_class(X, W, b)
        dW = np.dot(X.T, Yc -Y_hat)/m
        db = np.mean(Yc-Y_hat , axis=0, keepdims=True)

        W += eta * dW
        b += eta * db

        if verbose and (episode % 100 == 0 or episode == 999):
            Y_pred = np.argmax(pred_lr_multi_class(X, W, b), axis=1)
            acc = accuracy(Y, Y_pred)
            print("epoch ",episode,"accuracy train={0:.2f} %".format(acc*100))

    return W, b


def rl_gradient_ascent_multi_class_batch(X, Y, tbatch = 500, eta=0.2, numEp=1000, verbose=1):
    m, n = X.shape
    num_classes = len(np.unique(Y))

    W = np.zeros((n, num_classes))
    b = np.zeros((1,num_classes))


    # Entraînement avec la descente de gradient stochastique
    num_batche = m // tbatch
    for episode in range(numEp):
        indices = np.random.permutation(m)  

        for batch_num in range(num_batche):
            start = batch_num * tbatch
            end = (batch_num + 1) * tbatch
            Xbatch = X[indices[start:end],:]
            Ybatch = Y[indices[start:end]]
            
            Y_hat = pred_lr_multi_class(Xbatch, W, b)
            Yc = to_categorical(Ybatch, num_classes)
            dW = np.dot(Xbatch.T, Yc -Y_hat)/len(Xbatch)
            db = np.mean(Yc-Y_hat , axis=0, keepdims=True)

            W += eta * dW
            b += eta * db

        if verbose and (episode % 20 == 0 or episode == 99):
            Y_pred = np.argmax(pred_lr_multi_class(X, W, b), axis=1)
            acc = accuracy(Y, Y_pred)
            print("epoch ",episode,"accuracy train={0:.2f} %".format(acc*100))

    return W, b









