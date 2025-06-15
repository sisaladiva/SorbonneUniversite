import numpy as np
import matplotlib.pyplot as plt

def gen_data_lin(a, b, sig, N, Ntest):
    X_test = np.random.rand(Ntest)
    X_train = np.sort(np.random.rand(N))
    
    eps_test = np.random.randn(Ntest) * sig
    eps_train = np.random.randn(N) * sig
    
    y_test = a * X_test + b + eps_test
    y_train = a * X_train + b + eps_train

    
    return X_train, y_train, X_test, y_test


def modele_lin_analytique(X, Y):
    cov_XY = np.cov(X, Y, bias=True)[0, 1]  
    var_X = np.var(X, ddof=1)  
    mean_X = np.mean(X)  
    mean_Y = np.mean(Y)  
    
    ahat = cov_XY / var_X
    bhat = mean_Y - (cov_XY / var_X) * mean_X
    
    return ahat, bhat

def calcul_prediction_lin(X,ahat,bhat):
    X=np.array(X)
    return ahat*X+bhat

def erreur_mc(y, yhat):
    return ((y-yhat)**2).mean()

def dessine_reg_lin(X_train, y_train, X_test, y_test,a,b):
    plt.figure()
    plt.plot(X_test, y_test, 'r.',alpha=0.2,label="test")
    plt.plot(X_train, y_train, 'b',label="train")
    plt.plot(X_train, y_train, 'b.')
    plt.plot(X_test, calcul_prediction_lin(X_test,a,b), 'g', lw=3, label="prediction")
    plt.legend()

    
def make_mat_lin_biais(X): 
    N = len(X)
    return np.hstack((X.reshape(N,1),np.ones((N,1))))

def reglin_matriciel(Xe, y_train):

    A = Xe.T @ Xe
    B = Xe.T @ y_train

    w = np.linalg.solve(A,B)
    return w

def calcul_prediction_matriciel(Xe,w):
    return Xe @ w

def gen_data_poly2(a, b, c, sig, N, Ntest):
    X_train = np.sort(np.random.rand(N))
    X_test  = np.sort(np.random.rand(Ntest))
    y_train = a*X_train**2+b*X_train+c+np.random.randn(N)*sig
    y_test  = a*X_test**2 +b*X_test +c+np.random.randn(Ntest)*sig
    return X_train, y_train, X_test, y_test

def make_mat_poly_biais(X): 
    N = len(X)
    return np.hstack((X.reshape(N,1)**2, X.reshape(N,1),np.ones((N,1)) ))    


def dessine_poly_matriciel(Xp_train,yp_train,Xp_test,yp_test,w):
    
    yhat    = w[0]*Xp_train**2+w[1]*Xp_train+w[2]
    yhat_t  = w[0]*Xp_test**2 +w[1]*Xp_test +w[2]

    plt.figure()
    plt.plot(Xp_train, yhat, 'r')
    plt.plot(Xp_test, yp_test, 'r.',alpha=0.2,label="test")
    plt.plot(Xp_train, yp_train, 'b',label="train")
    plt.plot(Xp_train, yp_train, 'b.')
    plt.plot(Xp_test, yhat_t, 'g', lw=3, label="prediction")
    plt.legend()


def descente_grad_mc(X, y, eps=1e-4, nIterations=100):
    w = np.zeros(X.shape[1]) 
    allw = [w]
    for i in range(nIterations):
        w = w - eps*2*X.T@(X@w - y)
        allw.append(w) 
    allw = np.array(allw)
    return w, allw 
    
def application_reelle(X_train,y_train,X_test,y_test):

    w=reglin_matriciel(X_train,y_train)
    yhat   = np.dot(w,X_train.T)
    yhat_t = np.dot(w,X_test.T)
    print('Erreur moyenne au sens des moindres carrés (train):', erreur_mc(yhat, y_train))
    print('Erreur moyenne au sens des moindres carrés (test):', erreur_mc(yhat_t, y_test))

    return w,yhat,yhat_t


def normalisation(X_train, X_test):
    means = X_train.mean(axis=0)
    stds = X_train.std(axis=0)

    zero_var_cols_train = np.where(stds == 0)[0]
    zero_var_cols_test = np.where(X_test.std(axis=0) == 0)[0]

    X_train = np.delete(X_train, zero_var_cols_train, axis=1)
    X_test = np.delete(X_test, zero_var_cols_test, axis=1)
    means = np.delete(means, zero_var_cols_train)
    stds = np.delete(stds, zero_var_cols_train)

    Xn_train = (X_train - means) / stds
    Xn_test = (X_test - means) / stds
    
    # Ajout de la colonne de 1
    Xn_train = np.hstack((Xn_train, np.ones((Xn_train.shape[0], 1))))
    Xn_test = np.hstack((Xn_test, np.ones((X_test.shape[0], 1))))
    
    return Xn_train, Xn_test
