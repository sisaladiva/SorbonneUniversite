import numpy as np
import pydot
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

def exp(rate):
    x=np.random.rand(rate.shape[0], rate.shape[1])
    N = np.where(rate==0,1e-200,rate)
    return -np.log(1-x)/N


def simulation(graph,sources, maxT):
    
    names,k,r =graph
    infectious = np.array([maxT]*len(names),dtype=float)
    infectious[sources] = 0
    result = np.copy(infectious)
    
    while np.any([infectious<maxT]):
        i = infectious.argmin()
        for j in range(len(result)):
            if result[j] > result[i]:
                
                if (np.random.binomial(1, k.get((i, j), 0)) == 1) :
                    delta = exp(np.array([[r.get((i,j),0)]]))
                    t = result[i]+delta
                    if t < result[j]: 
                        result[j]=t 
                        infectious[j]=t 
                        
        infectious[i] = maxT
    return np.array(result)


def getProbaMC(graph, sources, maxT, nbsimu=10000):
    num_nodes = len(graph[0])
    infections_count = np.zeros(num_nodes)

    for _ in range(nbsimu):
        times = simulation(graph, sources, maxT)

        # Mettre à jour les comptes d'infection
        for i in range(num_nodes):
            if 0 <= times[i] < maxT:
                infections_count[i] += 1

    
    probabilities = infections_count / nbsimu

    return probabilities



def getPredsSuccs(graph):
    names, k, r = graph

    predecessors = {i: [] for i in range(len(names))}
    successors = {i: [] for i in range(len(names))}
  
    for (u, v), k_uv in k.items():
        predecessors[v].append((u, k_uv, r.get((u, v), 0)))
        successors[u].append((v, k_uv, r.get((u, v), 0)))

    return predecessors,successors


def compute_ab(v, times, preds, max_T, eps=1e-10):
    preds=preds.get(v,[])
    t=times[v]
    if t==0:
        return (1,0)
    a= 0
    b = 0.0
    for i, k, r in preds:
        if 0 <= times[i] < t:
            alpha = k*r*np.exp(-r*(t - times[i]))
            beta = k*np.exp(-r*(t - times[i])) + 1 - k
            a += alpha/beta
            b +=  np.log(beta)
            
    if t >= max_T:
        a = 1.0
    return (max(eps, a), b)


def compute_ll(times,preds,maxT):
    sa = np.zeros(times.size)
    sb = np.zeros(times.size)
    for i in range(sa.size):
        sa[i], sb[i] = compute_ab(i, times, preds, maxT)
    ll = np.log(sa).sum() + sb.sum()
    return ll, sa, sb
 

def addVatT(v,times,newt,preds,succs,sa,sb,max_T,eps=1e-20):
    succs = succs.get(v, [])
    
    # Mettre à jour les temps d'infection pour le nœud v
    times[v] = newt
    
    # Calculer les nouvelles valeurs a et b pour le nœud v
    a, b = compute_ab(v, times, preds, max_T, eps)
    sa[v] = a
    sb[v] = b
    
    # Mettre à jour les valeurs a et b pour les successeurs de v
    for i, k, r in succs:
        if times[i] > newt:
            a, b = compute_ab(i, times, preds, max_T, eps)
            sa[i] = a
            sb[i] = b
            
            
def logsumexp(x):
    shapes = list(x.shape)
    shapes[-1] = 1
    x_max = x.max(-1).reshape(shapes)
    return x.max(-1) + np.log(np.exp(x - x_max).sum(-1))
                              















