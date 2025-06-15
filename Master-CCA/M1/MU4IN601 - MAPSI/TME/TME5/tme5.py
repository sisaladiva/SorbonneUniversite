import numpy as np
import utils
import scipy.stats as stats

def sufficient_statistics( data, dico, x,y,z):
    t = utils.create_contingency_table ( data, dico, x, y, z )
    nb_z = t.shape[0]
    nb_x, nb_y = t[0][1].shape

    s = []
    for N_z, T in t:
        N_xz = T.sum(axis=1) 
        N_yz = T.sum(axis=0)
        if N_z==0: 
            N_z=1
            nb_z = nb_z-1
        den = np.where(( np.outer(N_xz,N_yz)/N_z )==0, 1 , np.outer(N_xz,N_yz)/N_z ) 

        s.append((T - ( np.outer(N_xz,N_yz)/N_z ))**2 / den)
    s = np.array( s )
    dof = ( nb_x - 1 ) * ( nb_y - 1 ) * nb_z
    return s.sum(), dof


def indep_score ( data, dico, x,y,z):
    s, dof = sufficient_statistics(data, dico, x,y,z)
    return stats.chi2.sf ( s, dof )


def best_candidate ( data, dico, x, z, risque ):
    res = []                                                         
    if x==0:                                                                
        return []
    
    for y in range(x):
        if (y == x) or (y == z):
            res.append(1)                                            
            continue
        
        ind=indep_score( data, dico, x, y, z )
        res.append(ind)

        
    res = np.array(res)    
    nonindep = np.where(res > risque, 1, res)        

                                                                             
    if np.min(nonindep) == 1:                                         
        return []
    else:
        return [np.argmin(nonindep)]                                  

def create_parents (data , dico , x , alpha):
    
    taille=len(data)
    parents=[]

    for i in range(taille):
        p= best_candidate(data, dico , x, parents,alpha)
        if p:
            parents=parents+p
        else:
            break
        
    return parents

def learn_BN_structure (data, dico, alpha):
    tab = []
    
    for i in range(len(data)):
        c = create_parents(data, dico, i, alpha)
        tab.append(c)
    return tab

