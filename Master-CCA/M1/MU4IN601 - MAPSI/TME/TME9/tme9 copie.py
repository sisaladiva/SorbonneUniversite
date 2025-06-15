import numpy as np
import matplotlib.pyplot as plt

def labels_tobinary(Y, cl):
    Y_binary = (Y == cl).astype(float)
    return Y_binary

def pred_lr(X, w, b):
    res= 1 / (1 + np.exp(-(np.dot(X, w.T) + b)))
    return res

def classify_binary(Y_pred):
    Y_pred_binary = (Y_pred > 0.5).astype(float)
    return Y_pred_binary

def accuracy(Y_pred, Y_true):
    accuracy = (Y_pred == Y_true).mean()
    return accuracy

def rl_gradient_ascent(X, Y, eta, niter_max):
    w = np.zeros(X.shape[1])
    b = 0
    accs = []
    it = 0

   
    for i in range(niter_max):
        Y_pred = 1 / (1 + np.exp(-(np.dot(X, w) + b)))
        dw = np.dot(X.T, (Y - Y_pred))
        db = np.sum(Y - Y_pred)


        w += eta * dw
        b += eta * db

        
        Y_pred_binary = (Y_pred > 0.5).astype(float)
        acc = (Y_pred_binary == Y).mean()
        accs.append(acc)

    
        it += 1

    return w, b, accs, it



def visualization(w):
    w_img = w.reshape((16, 16))
    plt.figure()
    plt.imshow(w_img, cmap='gray')
    plt.show()

def rl_gradient_ascent_one_against_all(X, Y, epsilon, niter_max):
    
    n_classes = np.unique(Y).size
    n_features = X.shape[1]

    
    W = np.zeros((n_classes, n_features))
    B = np.zeros(n_classes)

  
    for cl in range(n_classes):
       
        Y_binary = labels_tobinary(Y, cl)

        
        w, b, accs, it = rl_gradient_ascent(X, Y_binary, epsilon, niter_max)

      
        W[cl] = w
        B[cl] = b
        Y_pred = pred_lr(X, w, b)

        
        Y_pred_binary = classify_binary(Y_pred)

       
        acc = accuracy(Y_pred_binary, Y_binary)

       
        print(f"Classe:{cl}acctrain={acc*100:.2f}%")

    return W, b

    


def classif_multi_class(Y_pred):
    Y_pred_mc = np.argmax(Y_pred, axis=1)
    return Y_pred_mc
