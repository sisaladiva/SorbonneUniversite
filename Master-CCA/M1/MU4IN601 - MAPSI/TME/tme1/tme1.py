import numpy as np
import matplotlib.pyplot as plt

def analyse_rapide(d):
    
    # calcul de la moyenne
    mean = np.mean(d)
    print("mean : ", mean)
    
    # calcul de l'écart type
    std = np.std(d)
    print("std : ",std)
    
    # calcul des 10 quatiles
    q = [np.quantile(d,i/10, axis=None) for i in range(10)]
    print("quantiles : ",q)

    
def discretisation_histogramme(d,n=10):
    
    #calcul de l'echelle suivant l'axe x
    echelle = (np.max(d)-np.min(d))/n
    print(echelle)
    
    #calcul des bornes
    bornes = [np.min(d)+echelle*i for i in range(n)]
    print("bornes : ",bornes)
    
    #calcul d'effectifs
    effectifs = []
    for i in range(len(bornes)-1):
        effectifs.append(np.where((d>bornes[i]) & (d<bornes[i+1]), 1, 0).sum())
    print("effectifs :" , effectifs)
    
    # l'histogramme
    plt.figure()
    plt.bar(bornes[:-1], effectifs,width=echelle, color="red")
    plt.xlabel("numero de l'intervalle")
    plt.ylabel("le nombre d'effectif")
    plt.title("discrétisation de la distance")
    plt.show()

    
def discretisation_prix_au_km(data,n):
    
    d = data[:,-4]
    #calcul de l'echelle suivant l'axe x
    print("echelle que je cherche", (max(d)-min()/n)
    print ("max", max(d))
    print ("min", min(d))
    print ("n",n)
    
    echelle = (d.max() - d.min())/n
    print("jhjh", echelle)
    
    #calcul des bornes
    bornes = [np.min(d)+echelle*i for i in range(n)]
    print("bornes : ",bornes)
    
    #calcul d'effectifs
    effectifs =[len(np.where((d>bornes[i]) & (d<bornes[i+1]))[0]) for i in range(len(bornes)-1)]

    print("effectifs :" , effectifs)
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
