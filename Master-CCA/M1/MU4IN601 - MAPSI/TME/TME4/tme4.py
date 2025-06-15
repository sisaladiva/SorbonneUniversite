import numpy as np
import os
import pickle as pkl



def normale_bidim (x, mu, Sig):
    N= len(x)
    determinant= np.linalg.det(Sig)
    inversesig= np.linalg.inv(Sig)

    vraisemblance = (1 / ((2 * np.pi) ** (N/2) * determinant ** 0.5))* np.exp(-0.5 * np.dot(np.dot((x - mu).T, inversesig), x - mu))
    return vraisemblance

def estimation_nuage_haut_gauche ():
    mu4  = np.array([4.25, 80.])
    Sig4 = np.array([[0.25, 0.5],[0.5, 50.]])
    return mu4, Sig4

def init (X):
    pi= np.array([0.5,0.5])
    mu= np.mean(X, axis=0)
    mu= np.stack((mu+1,mu-1))
    Sig=np.cov(X, rowvar=0)
    Sig=np.stack((Sig,Sig))

    return pi, mu, Sig


def Q_i(X, pi, mu, Sig):
    Q = []
    for i in range(pi.size):
        Q.append(pi[i] * (np.array([normale_bidim(x,mu[i],Sig[i]) for x in X])))
    return np.array(Q) / np.array(Q).sum(axis=0)



def update_param(X, Q, pi, mu, Sig):
    pi_u = []
    mu_u = []
    Sig_u = []

    for qi in Q:
        num = qi.sum()
        pi_u.append(num / Q.sum())

        a = (qi.reshape(-1, 1) * X).sum(axis=0)
        mu_u.append(a / qi.sum())

        # Calcul de la covariance
        cov_sum = np.zeros((X.shape[1], X.shape[1]))
        for i in range(X.shape[0]):
            cov_sum += qi[i] * np.outer(X[i] - mu_u[-1], X[i] - mu_u[-1])
        
        Sig_u.append(cov_sum / qi.sum())

    pi_u = np.array(pi_u)
    mu_u = np.array(mu_u)
    Sig_u = np.array(Sig_u)

    return pi_u, mu_u, Sig_u


def EM(X, nIterMax=100, saveParam=None, initFunc=None):
    if initFunc is not None:
        pi, mu, Sig = initFunc(X)
    else:
        pi, mu, Sig = init(X)
        
    eps = 1e-3
    
    for i in range(nIterMax):
        q = Q_i(X, pi, mu, Sig)
        pi_u, mu_u, Sig_u = update_param(X, q, pi, mu, Sig)
        
        if saveParam is not None:                                           
            if not os.path.exists(saveParam[:saveParam.rfind('/')]):     
                os.makedirs(saveParam[:saveParam.rfind('/')])
            pkl.dump({'pi': pi_u, 'mu': mu_u, 'Sig': Sig_u},
                     open(saveParam+str(i)+".pkl",'wb'))
                     
        if np.abs(mu_u - mu).sum() < eps:
            break

        pi, mu, Sig = pi_u, mu_u, Sig_u
        
    assert not np.array_equal(mu, mu_u), "Failed convergence"
    
    return i, pi_u, mu_u, Sig_u


def init_4(X):
    pi = np.array([0.25, 0.25, 0.25, 0.25])
    mu = np.mean(X, axis=0)
    mu = np.stack((mu + [1,1], mu + [-1,1], mu + [1,-1], mu + [-1,-1]))
    Sig = np.asarray([np.cov(X.T) for _ in range(4)])
    return pi, mu, Sig

def bad_init_4(X):
    pi = np.array([0.25, 0.25, 0.25, 0.25])
    mu = np.mean(X, axis=0)
    mu = np.array([[4, 2], [3, 4], [0, 0], [-5, 0]]) + mu
    Sig = np.asarray([np.cov(X.T) for _ in range(4)])

    return pi, mu, Sig

def init_B(Xu):
    n = 10
    pi = np.ones(n) / n
    theta = np.array([Xu[i*3:(i+1)*3, :].mean(axis=0) for i in range(n)])
    return pi, theta


def logpobsBernoulli(X, theta):
    seuil = 1e-5
    theta = np.clip(theta, seuil, 1 - seuil)
    logp = np.sum(X * np.log(theta) + (1 - X) * np.log(1 - theta))
    return np.array(logp)

def Q_i_B(X, pi, theta):
    v = np.array([[logpobsBernoulli(X[j], theta[i]) for i in range(len(theta))] for j in range(len(X)) ])
    c = np.max(v)
    log_sum = c + np.log( (np.exp(v - c) * pi).sum(axis=1) )
    q = []
    for i in range(len(theta)):
        q.append(v[:, i] + np.log(pi[i]) - log_sum)
    q = np.array(q)
    return np.exp(q)

def update_param_B(X, q, pi, theta):
    pi_u = []
    theta_u = []
    d = q.sum()
    for Q_y_i in q:
        num = Q_y_i.sum()
        pi_u.append(num/d)
        
        s = (Q_y_i.reshape(-1,1) * X).sum(axis=0)
        theta_u.append(s/Q_y_i.sum())

    pi_u = np.array(pi_u)
    theta_u = np.array(theta_u)
    return pi_u, theta_u

def EM_B(X, nIterMax=100, saveParam=None):
    epsilon = 1e-3
    pi, theta= init_B(X)
    for i in range(100):
        q = Q_i_B(X, pi, theta)
        pi_u, theta_u = update_param_B(X, q, pi, theta)
        if (np.abs(theta-theta_u).max() < epsilon):
            break
        else:
            pi, theta = pi_u, theta_u
    else:
        print('Non convergence en 100 itérations')
    return i, pi, theta


def calcul_purete(X, Y, pi, theta):
    Y2 = Q_i_B(X, pi, theta)
    Y_hat = np.argmax(Y2,axis=0)

    Y_hat_c = [Y[Y_hat == c] for c in range(len(pi))]
    tab = [np.unique(y, return_counts=True) for y in Y_hat_c]
    maj = [v[0][np.argmax(v[1])] for v in tab]
    #purete = np.array([tab[c][1][np.nonzero(tab[c][0]==maj[c])][0]/sum(tab[c][1]) for c in range(len(pi))])
    purete = np.array([tab[c][1][np.min(np.nonzero(tab[c][0]==maj[c]))]/np.sum(tab[c][1]) for c in range(len(pi))])
    poids = np.array([np.sum(tab[c][1]) for c in range(len(pi))])
    return purete, poids
