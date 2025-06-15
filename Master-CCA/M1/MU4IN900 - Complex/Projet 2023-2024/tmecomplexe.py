import random  
import time   
import matplotlib.pyplot as plt  

def sac_a_dos_glouton_instance(poids, utilites, capacite):
    nbobjets = len(poids)
    ratio = [(utilites[i] / poids[i], poids[i], utilites[i], i) for i in range(nbobjets)]
   
    ratio.sort(reverse=True, key=lambda x: x[0])
    indices_tries = [r[3] for r in ratio]
    valeur_totale = 0
    poids_total = 0
    objets_selectionnes = []

    rat = [r[0] for r in ratio]
    print("Indices triés des objets selon le ratio (u/p) :", indices_tries)
    print("le ratio (u/p) :", rat)
    
    for r in ratio:
        if poids_total + r[1] <= capacite:
            objets_selectionnes.append(r[3])  # On stocke l'indice de l'objet sélectionné
            poids_total += r[1]
            valeur_totale += r[2]
    
    return objets_selectionnes, valeur_totale, poids_total


def generer_instances(nb_objets):
    poids = [random.randint(1, 20) for _ in range(nb_objets)]
    utilites = [random.randint(1, 30) for _ in range(nb_objets)]
    return poids, utilites



def mesurer_temps_glouton(nb_objets_list, capacite):
    temps_execution = []
    
    for nb_objets in nb_objets_list:
        poids, utilites = generer_instances(nb_objets)

        debut = time.time()
        sac_a_dos_glouton_instance(poids, utilites, capacite)
        fin = time.time()
        
        temps_execution.append(fin - debut)
    
    return temps_execution




nb_objets_list = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]  
capacite = 50  


temps_execution = mesurer_temps_glouton(nb_objets_list, capacite)

plt.plot(nb_objets_list, temps_execution, marker='o')
plt.title("Temps d'exécution de l'algorithme glouton en fonction du nombre d'objets")
plt.xlabel("Nombre d'objets")
plt.ylabel("Temps d'exécution (secondes)")
plt.grid(True)
plt.show()




