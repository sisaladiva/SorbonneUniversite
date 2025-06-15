import random
from array import array
import numpy as np
import matplotlib.pyplot as plt


def bernoulli (p):
    i= random.random()
    if i<p:
        return 1
    if i>=p:
        return 0


def binomiale(n,p):
    res=0
    for i in range(n):
        bern=bernoulli(p)
        if ( bern==1):
            res=res+1
    return res


def galton(l,n,p):
    tableau=[]
    for i in range(l):
        tableau.append(binomiale(n,p))

    tableaufinal=np.array(tableau, dtype=float)
    return tableaufinal



def histo_galton(l, n, p):

    tableau=galton(l,n,p)
    nb_tot = len(np.unique(tableau))
    
    plt.hist(tableau, bins=nb_tot, color='pink')
    plt.title('Histogramme de la répartition des billes dans la planche')
    plt.xlabel('Valeurs')
    plt.ylabel('Fréquence')
    
    plt.show()

def normale(k, sigma):
    if k % 2 == 1:
        x = np.linspace(-2 * sigma, 2 * sigma, k)
        y = (1 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-0.5 * (x / sigma)**2)
        return y
    
    raise ValueError("Erreur !!! le k n'est pas impair je ne peux rien faire pour vous")  
   
def proba_affine(k, slope):
    if k % 2 == 1:
        if slope != 0:
            if abs(slope) > (2/(k*k)):
                raise ValueError("Erreur !! La pente n'est pas forcément positive !!")
        
        y=[]
        for i in range(k):
            y.append( 1 / k + (i - (k - 1) / 2) * slope)
        
        return y
    raise ValueError("Erreur !!! le k n'est pas impair je ne peux rien faire pour vous.")


def Pxy(a,b):

    tableaufinal=[]
    for i in range (len(a)):
        tableaudei=[]
        for j in range (len(b)):
            tableaudei.append(a[i]*b[j])
        tableaufinal.append(tableaudei)
    return np.array(tableaufinal)


def calcYZ(p_xyzt):
    tabfinal=np.zeros((2,2))

    for x in range(2):
        for y in range(2):
            for z in range(2):
                for t in range (2):
                    tabfinal[y][z]=tabfinal[y][z]+p_xyzt[x][y][z][t]
    return tabfinal

def calcXTcondYZ (p_xyzt):
    tabfinal=np.zeros((2,2,2,2))
    
    for x in range(2):
        for y in range(2):
            for z in range (2):
                for t in range (2):
                    tabfinal[x][y][z][t]= p_xyzt[x][y][z][t]/(calcYZ(p_xyzt)[y][z])

    return tabfinal


def calcX_etTcondYZ(p_xyzt):
    P_YZ = calcYZ(p_xyzt)
    P_XTcondYZ = calcXTcondYZ(p_xyzt)
    P_XcondYZ = np.sum(P_XTcondYZ, axis=3)  
    P_TcondYZ = np.sum(P_XTcondYZ, axis=0)  
    
    return P_XcondYZ, P_TcondYZ

def testXTindepCondYZ (p_xyzt, epsilon):
    P_XTcondYZ= calcXTcondYZ(p_xyzt)
    P_XcondYZ, P_TcondYZ = calcX_etTcondYZ(p_xyzt)
    
    for x in range(0, 2):
        for y in range(0, 2):
            for z in range(0, 2):
                for t in range(0, 2):
                    P_XTconYZ = P_XTcondYZ[x][y][z][t]
                    multiplication = P_XcondYZ[x][y][z] * P_TcondYZ[t][y][z]
                    if abs(P_XTconYZ - multiplication) < epsilon:
                        return 1
                    
    return 0



def testXindepYZ(p_xyzt, epsilon):
    p_xyz = np.zeros((2,2,2))
    for x in range(2):
        for y in range(2):
            for z in range(2):
                p_xyz[x][y][z]= p_xyzt[x][y][z].sum()

    p_x = np.zeros(2)
    p_x[0] = p_xyz[0].sum()
    p_x[1] = p_xyz[1].sum()
    p_yz= p_xyz[0]+p_xyz[1]
    
    for x in range(2):
        for y in range(2):
            for z in range(2):
                val1 = p_xyz[x][y][z]
                val2 = p_x[x] * p_yz[y][z]
                if val1!=val2:
                    return 0
   
    return 1


def conditional_indep(pjointe,x,y,zd, epsilon):
    if (len(zd))==0:
        p_xyzd= pjointe.margSumIn([x,y])
        if ((p_xyzd- pjointe.margSumIn(x)*pjointe.margSumIn(y)).abs().max() < epsilon ):
            return True
        return False
    
    p_xyzd= pjointe.margSumIn([x, y]+zd)
    p_xy_zd= p_xyzd/pjointe.margSumIn(zd)
    p_x_zd= pjointe.margSumIn([x]+zd)/pjointe.margSumIn(zd)
    p_y_zd= pjointe.margSumIn([y]+zd)/pjointe.margSumIn(zd)
    if (p_xy_zd- p_x_zd* p_y_zd).abs().max()<epsilon:
        return True
    return False


def compact_conditional_proba (p,x):
    epsilon=1e-10
    
    K = list(p.names)
    K.remove(x)
    for i in K[:]:
        K_i = K.copy()
        K_i.remove(i)
        if conditional_indep(p, i, x,K_i,epsilon):
            K.remove(i)

    if K == []:
        return p.margSumIn(K+[x]).putFirst(x)
    
    return ((p.margSumIn(K+[x])) / p.margSumIn(K)).putFirst(x)



def create_bayesian_network(p, epsilon):
    liste = []
    Pli = p.var_names
    for X_i in reversed(Pli):
        q = compact_conditional_proba(p, X_i)
        liste.append(q)
        p = p.margSumOut(X_i)
    return liste


def calcNbParams(Pjointe):
    l=create_bayesian_network(Pjointe, 0.001)
    taille_jointe=Pjointe.toarray().size
    taille_rb=sum([x.toarray().size for x in l])
    return taille_jointe, taille_rb


