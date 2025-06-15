import numpy as np
import utils

def discretise( X, d) :
    intervalle=360/d
    res =[]
    sign=[]
    for i in range(d+1):
        sign.append(np.array(intervalle *i))

    for i in X:
        res.append(np.digitize(i, sign, right=False) - 1)
    return np.array(res, dtype='object')

def groupByLabel(Y):
    indices=dict()

    for i in np.unique(Y):
        indices[i], =np.where(Y==i)
    return indices

def learnMarkovModel( X, d):
    A=np.zeros((d,d))
    Pi = np.zeros(d)
    X = [discretise(x,d) for x in X]
    for i in X:
        Pi[i[0]]=Pi[i[0]] +1
        for j in range(1, len(i)):
            A[i[j-1],i[j]] +=1
    A =A /np.maximum(A.sum(1).reshape(d, 1),1)
    Pi =Pi/Pi.sum()

    return Pi,A

def learn_all_MarkovModels (X,Y,d):

    x=discretise (X,d)
    indice = groupByLabel(Y)
    modeles=dict()


    for i, j in indice.items():
        modeles[i]=learnMarkovModel([X[m] for m in j],d)

    return modeles


def stationary_distribution_freq(Xd, d):
    nb = np.zeros(d)
    total = 0

    for i in Xd:
        for j in i:
            nb[j] += 1
            total += 1

    freq = nb / total
    return np.array(freq)

def stationary_distribution_sampling(pi, A, N):
    d = len(pi)
    nb = np.zeros(d)

    tmp = np.random.choice(d, p=pi)

    for i in range(N):
        nb[tmp] += 1
        prob = A[tmp]
        tmp = np.random.choice(d, p=prob)

    sampling = nb / nb.sum()
    return sampling.tolist()

def stationary_distribution_fixed_point(A, epsilon):
    d = A.shape[0]
    pi = np.ones(d) / d

    while True:
        new_pi = np.dot(pi, A)
        if np.square(np.subtract(pi, new_pi)).mean() < epsilon:
            break
        pi = new_pi

    return new_pi

def stationary_distribution_fixed_point_VP(A):
    epsilon =1e-10
    d = A.shape[0]
    AT= A.transpose()
    valpropres, vectpropres = np.linalg.eig(AT)

    i=np.argmin(np.abs(valpropres -1))
    res= np.real(vectpropres[:,i])
    res /=np.sum(res)
    return res.reshape(-1,1).tolist()

def logL_Sequence( s, Pi, A):
    s= s.astype(int)
    res=np.log(Pi[s[0]])
    for i in range(len(s)-1):
        res= res + np.log(A[s[i]][s[i+1]])
    return res

def compute_all_ll(Xd, models):
   return np.array([[logL_Sequence(Xd[i], models[classe][0], models[classe][1]) for i in range(len(Xd))] for classe in models.keys()])

def accuracy(ll,Y):
    ynb = np.zeros(Y.shape)
    for n, char in enumerate(np.unique(Y)):
        ynb[Y == char] = n
    p = ll.argmax(0) 
    return np.where(p!= ynb, 0.,1.).mean()


def learn_all_MarkovModels_Laplace (x,y,d):
    labelu = np.unique(y)
    res = {} 
    
    for label in labelu:
        ind = np.where(y == label)[0]
        xlabel = [x[i] for i in ind]
        Pi = np.ones(d)
        A = np.ones((d, d))

        for xi in xlabel:
            xi = discretise(xi, d)
            xi = [int(value) for value in xi]
            Pi[xi[0]] = Pi[xi[0]]+ 1
            for i in range(len(xi) - 1):
                A[xi[i], xi[i + 1]] = A[xi[i], xi[i + 1]] + 1

        A = A / np.maximum(A.sum(1).reshape(d, 1), 1)
        Pi = Pi / Pi.sum()

        res[label] = (Pi, A)

    return res

