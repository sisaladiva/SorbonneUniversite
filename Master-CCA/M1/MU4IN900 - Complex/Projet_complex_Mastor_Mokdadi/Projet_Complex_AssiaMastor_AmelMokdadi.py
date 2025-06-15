import random
import time
import copy
import matplotlib.pyplot as plt
import numpy as np
import math

#2.1.1
def supprsommet(G, sommet):
    G1 = copy.deepcopy(G)
    
    if sommet not in G:
        return G1
    
    G1.pop(sommet)

    for voisins in G1:
        if sommet in G1[voisins]:
            G1[voisins].remove(sommet)
    
    return G1

#2.1.2
def supprsommets(G, sommets):
    for i in range(len(sommets)):
        supprsommet(G,sommets[i])

    return G

#2.1.3
def degres (G):
    res=dict()
    for sommet in G:
        res[sommet]= len(G[sommet])

    return res

def degresmax(G):
    degsom = degres(G)
    sommetmax = None
    degresmax = -1

    for sommet, deg in degsom.items():
        if deg > degresmax:
            degresmax = deg
            sommetmax = sommet

    return sommetmax

#2.2.1
def aleatoire(n, p):
    G = dict()
    listesommets = []
    for i in range(n):
        listesommets.append(chr(random.randint(ord('A'), ord('Z'))))

    for sommet in listesommets:
        G[sommet] = [] 

    for i in range(len(listesommets)):
        for j in range(i+1, len(listesommets)):
            sommet1 = listesommets[i]
            sommet2 = listesommets[j]

            probabilite_arrete = random.random()
            if probabilite_arrete < p:
                G[sommet1].append(sommet2)
                G[sommet2].append(sommet1)  
    return G



# 3.2

def algo_couplage(G):
    C = set()
    for e in G.values():
        if not any(v in C for v in e):
            C.update(e)
    #print("La solution de l'algorithme de couplage est : ", C)
    return C

def algo_glouton(G):
    C = set()
    while G:
        v = max(G, key=lambda k: len(G[k]))
        C.add(v)
        del G[v]
        for u in list(G.keys()):
            G[u] = [x for x in G[u] if x != v]
            if not G[u]:
                del G[u]
    #print("La solution de l'algorithme glouton est : ", C)
    return C



def comparaisonqualite(Nmax, p, repetitions):
    valuersn= [int(Nmax * i/10) for i in range(1, 11)]
    couplage_qualite = []
    couplage_glouton = []

    for n in valuersn:
        couplage_qualite_val = []
        glouton_qualité_val = []

        for _ in range(repetitions):
            G = aleatoire(n, p)

            solution_couplage = algo_couplage(G)
            solution_glouton = algo_glouton(G)

            couplage_qualite_val.append(len(solution_couplage))
            glouton_qualité_val.append(len(solution_glouton))

        couplage_qualite.append(np.mean(couplage_qualite_val))
        couplage_glouton.append(np.mean(glouton_qualité_val))

    plt.plot(valuersn, couplage_qualite, label='Couplage', color='#FF69B4')
    plt.plot(valuersn, couplage_glouton, label='Glouton', color='#800080')
    plt.xlabel('Taille de l\'instance (n)')
    plt.ylabel('Qualité de la solution (taille de la couverture)')
    plt.title('Comparaison de la qualité de la solution (Moyenne sur {} répétitions)'.format(repetitions))
    plt.legend()
    plt.show()





def compare_temps_execution(Nmax, p, repetitions):
    n_values = [int(Nmax * i/10) for i in range(1, 11)]
    temps_couplage_moyen = []
    temps_glouton_moyen = []
    
    for n in n_values:
        temps_couplage = []
        temps_glouton = []

        for _ in range(repetitions):
            G = aleatoire(n, p)
            g1=copy.deepcopy(G)
            debut_couplage = time.time()
            algo_couplage(g1)
            fin_couplage = time.time()
            temps_couplage.append(fin_couplage - debut_couplage)
            g2=copy.deepcopy(G)
            debut_glouton = time.time()
            algo_glouton(g2)
            fin_glouton = time.time()
            temps_glouton.append(fin_glouton - debut_glouton)

        temps_couplage_moyen.append(np.mean(temps_couplage))
        temps_glouton_moyen.append(np.mean(temps_glouton))

    plt.plot(n_values, temps_couplage_moyen, label='Couplage', color='#FF69B4')
    plt.plot(n_values, temps_glouton_moyen, label='Glouton', color='#800080')
    plt.xlabel('Taille de l\'instance (n)')
    plt.ylabel('Temps d\'exécution moyen (s)')
    plt.title('Comparaison des temps d\'exécution (Moyenne sur {} répétitions)'.format(repetitions))
    plt.legend()
    plt.show()





def tempsexecbranch(Nmax, p, repetitions):
    n_values = [int(Nmax * i/10) for i in range(1, 11,2)]
    
    temps_glouton_moyen = []
    temps_glouton_couplage_moyen = []
    temps_glouton_ameliore_moyen = []  
    temps_glouton_ameliore_max_degre_moyen = []  

    for n in n_values:
        
        temps_glouton = []
        temps_glouton_couplage = []
        temps_glouton_ameliore = []  
        temps_glouton_ameliore_max_degre = []  

        for _ in range(repetitions):
            G = aleatoire(n, p)
            debut_glouton = time.time()
            branchement(G)
            fin_glouton = time.time()
            temps_glouton.append(fin_glouton - debut_glouton)

            g2=copy.deepcopy(G)
            debut_glouton_couplage = time.time()
            branchementcouplage(g2)
            fin_glouton_couplage = time.time()
            temps_glouton_couplage.append(fin_glouton_couplage - debut_glouton_couplage)

            g3=copy.deepcopy(G)
            debut_glouton_ameliore = time.time()
            branchement_ameliore(g3)  
            fin_glouton_ameliore = time.time()
            temps_glouton_ameliore.append(fin_glouton_ameliore - debut_glouton_ameliore)

            g4=copy.deepcopy(G)
            debut_glouton_ameliore_max_degre = time.time()
            branchementcouplageAmelioredegmax(g4)  
            fin_glouton_ameliore_max_degre = time.time()
            temps_glouton_ameliore_max_degre.append(fin_glouton_ameliore_max_degre - debut_glouton_ameliore_max_degre)
            
        temps_glouton_moyen.append(np.mean(temps_glouton))
        temps_glouton_couplage_moyen.append(np.mean(temps_glouton_couplage))
        temps_glouton_ameliore_moyen.append(np.mean(temps_glouton_ameliore))
        temps_glouton_ameliore_max_degre_moyen.append(np.mean(temps_glouton_ameliore_max_degre))

    
    plt.yscale('log')
    plt.plot(n_values, temps_glouton_moyen, label='Branchement', color='#FF69B4')
    plt.plot(n_values, temps_glouton_couplage_moyen, label='Branchement avec Couplage', color='#CD9575')
    plt.plot(n_values, temps_glouton_ameliore_moyen, label='Branchement couplage Amélioré', color='#873260')  
    plt.plot(n_values, temps_glouton_ameliore_max_degre_moyen, label='Branchement Amélioré avec degré max)', color='#008080')  # Nouvelle courbe
    plt.xlabel('Taille de l\'instance (n)')
    plt.ylabel('Temps d\'exécution moyen (s)')
    plt.title('Comparaison des temps d\'exécution (Moyenne sur {} répétitions)'.format(repetitions))
    plt.legend()
    plt.show()



#4.1

def branchement(graphe):
    pile = []  
    meilleure_solution = None  

    arcs = [(u, v) for u in graphe for v in graphe[u]]

    etat_initial = {
        "graphe": graphe,
        "solution": set(),
        "arcs_non_couverts": arcs
    }
    
    pile.append(etat_initial)

    while pile:
        etat = pile.pop()  

        
        if not etat["arcs_non_couverts"]:
            if meilleure_solution is None or len(etat["solution"]) < len(meilleure_solution):
                meilleure_solution = etat["solution"]
            continue

        
        arc_a_explorer = etat["arcs_non_couverts"].pop()
        u, v = arc_a_explorer
        
        
        nouvel_etat_avec_u = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({u}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if u not in arc]
        }
       
        nouvel_etat_avec_v = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({v}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if v not in arc]
        }
        
        pile.append(nouvel_etat_avec_u)
        pile.append(nouvel_etat_avec_v)
    
    return meilleure_solution


#4.2
def calcul_bornes(G):
    if not G:  
        return -1, -1, -1
    g1=copy.deepcopy(G)
    sommetmax = degresmax(g1)
    delta = len(G[sommetmax])
    n = len(G.keys())
    m = len(G.values())
    b1 = -1
    if delta != 0:
        b1 = np.ceil(m / delta)
    b2 = len(algo_couplage(g1))
    b3 = -1
    if ((2 * n - 1) ** 2 - 8 * m) > 0:
        b3 = (2 * n - 1 - math.sqrt((2 * n - 1) ** 2 - 8 * m)) / 2

    return b1, b2, b3


def extraire_sous_graphe(graphe, liste_sommets):
    sous_graphe = {}  
    
    for sommet in liste_sommets:
        voisins = [voisin for voisin in graphe[sommet] if voisin in liste_sommets]
        sous_graphe[sommet] = voisins
    
    return sous_graphe


def branchementcouplage(graphe):
    solution_couplage = algo_couplage(graphe)
    graphecouple=extraire_sous_graphe(graphe, solution_couplage)
    
    pile = []  
    meilleure_solution = None  
    bornes = calcul_bornes(graphe)

    
    
    arcs = [(u, v) for u in graphe for v in graphe[u]]

    
    etat_initial = {
        "graphe": graphecouple,
        "solution": set(),
        "arcs_non_couverts": arcs
    }

    pile.append(etat_initial)
    
    while pile:
        etat = pile.pop()  
        
        if len(etat["solution"]) >= max(bornes):
            continue


        
        if not etat["arcs_non_couverts"]:
            if meilleure_solution is None or len(etat["solution"]) < len(meilleure_solution):
                meilleure_solution = etat["solution"]
            continue
        
        
        arc_a_explorer = etat["arcs_non_couverts"].pop()
        u, v = arc_a_explorer
        
        
        
        nouvel_etat_avec_u = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({u}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if u not in arc]
        }
        
       
        nouvel_etat_avec_v = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({v}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if v not in arc]
        }
        
        
        pile.append(nouvel_etat_avec_u)
        pile.append(nouvel_etat_avec_v)

    return meilleure_solution


#4.3.1
def branchement_ameliore(graphe):
    g1 = copy.deepcopy(graphe)
    solution_couplage = algo_couplage(g1)
    graphecouple = extraire_sous_graphe(graphe, solution_couplage)
    bornes = calcul_bornes(graphe)
    pile = []
    meilleure_solution = None

    
    arcs = [(u, v) for u in graphe for v in graphe[u]]

    
    etat_initial = {
        "graphe": graphecouple,
        "solution": set(),
        "arcs_non_couverts": arcs
    }

    pile.append(etat_initial)

    while pile:
        etat = pile.pop()  

        
        if not etat["arcs_non_couverts"]:
            if meilleure_solution is None or len(etat["solution"]) < len(meilleure_solution):
                meilleure_solution = etat["solution"]
            continue

        if len(etat["solution"]) >= max(bornes):
            continue

        
        arc_a_explorer = etat["arcs_non_couverts"].pop()
        u, v = arc_a_explorer

        
        nouvel_etat_avec_u = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({u}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if u not in arc]
        }

        
        nouvel_etat_avec_v = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({v}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if v not in arc]
        }

        for voisin in etat["graphe"].get(u, []):
            nouvel_etat_avec_v["solution"].add(voisin)
            nouvel_etat_avec_v["graphe"].pop(voisin, None)
        
        pile.append(nouvel_etat_avec_u)
        pile.append(nouvel_etat_avec_v)

    return meilleure_solution


#4.3.2

def construire_graphe(arcs):
    G = {}
    for u, v in arcs:
        if u not in G:
            G[u] = []
        if v not in G:
            G[v] = []
        G[u].append(v)
        G[v].append(u)
    return G

def branchementcouplageAmelioredegmax(graphe):
    g1=copy.deepcopy(graphe)
    solution_couplage = algo_couplage(g1)
    graphecouple = extraire_sous_graphe(graphe, solution_couplage)
    pile = []
    meilleure_solution = None
    bornes = calcul_bornes(graphe)

    arcs = [(u, v) for u in graphe for v in graphe[u]]
 

    
    etat_initial = {
        "graphe": graphecouple,
        "solution": set(),
        "arcs_non_couverts": arcs
    }

    pile.append(etat_initial)

    while pile:
        etat = pile.pop()  
    
       
        if not etat["arcs_non_couverts"]:
            if meilleure_solution is None or len(etat["solution"]) < len(meilleure_solution):
                meilleure_solution = etat["solution"]
            continue

    
        
        if len(etat["solution"]) >= max(bornes):
            continue

        graphe_res = construire_graphe(etat["arcs_non_couverts"])
        sommet_degre_max = degresmax(graphe_res)
        arret_a_explorer = (sommet_degre_max, graphe_res[sommet_degre_max][0])

        u, v = arret_a_explorer

       
        nouvel_etat_avec_v = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({v}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if v not in arc]
        }


        
        nouvel_etat_sans_u = {
            "graphe": etat["graphe"],
            "solution": etat["solution"].union({u}),
            "arcs_non_couverts": [arc for arc in etat["arcs_non_couverts"] if u not in arc]
        }

        for voisin in graphe[u]:
            if voisin in nouvel_etat_sans_u["graphe"]:
                nouvel_etat_sans_u["graphe"].pop(voisin)

        pile.append(nouvel_etat_avec_v)
        pile.append(nouvel_etat_sans_u)

    return meilleure_solution


#4.4s
def calculer_rapport_approximation(Nmax, p, repetitions):
    valuersn = [int(Nmax * i / 10) for i in range(1, 11)]
    rapport_approximation_couplage = []
    rapport_approximation_glouton = []
    pire_rapport_couplage = 0 
    pire_rapport_glouton = float('-inf')  
    
    for n in valuersn:
        rapport_couplage_val = []
        rapport_glouton_val = []
        for _ in range(repetitions):
            G = aleatoire(n, p)
            g1 = copy.deepcopy(G)
            g2 = copy.deepcopy(G)
            g3=copy.deepcopy(G)
            
            if (branchement_ameliore(g3)!= None):
                solution_optimale = branchementcouplageAmelioredegmax(g3)
                tailleopt=len(solution_optimale)
            else:
                tailleopt=0
            solution_couplage = algo_couplage(g1)
            solution_glouton = algo_glouton(g2)

            
            rapport_couplage = len(solution_couplage) / tailleopt if tailleopt != 0 else float('inf')
            rapport_glouton = len(solution_glouton) / tailleopt if tailleopt != 0 else float('inf')
             

            rapport_couplage = len(solution_couplage) / len(solution_optimale) if len(solution_optimale) != 0 else float('inf')
            rapport_glouton = len(solution_glouton) / len(solution_optimale) if len(solution_optimale) != 0 else float('inf')

           
            pire_rapport_couplage = max(pire_rapport_couplage, rapport_couplage)
           
            
            pire_rapport_glouton = max(pire_rapport_glouton, rapport_glouton)

            rapport_couplage_val.append(rapport_couplage)
            rapport_glouton_val.append(rapport_glouton)

        rapport_approximation_couplage.append(np.mean(rapport_couplage_val))
        rapport_approximation_glouton.append(np.mean(rapport_glouton_val))
        
    
    pire_rapport_couplage = float('-inf')  
    pire_rapport_glouton = float('-inf')

    
    for i in rapport_approximation_couplage:
        if i != float('inf') and i > pire_rapport_couplage:
            pire_rapport_couplage = i
    for i in rapport_approximation_glouton:
        if i != float('inf') and i > pire_rapport_glouton:
            pire_rapport_glouton = i
    
    print(f"Pire rapport d'approximation pour le couplage : {pire_rapport_couplage}")
    print(f"Pire rapport d'approximation pour le glouton : {pire_rapport_glouton}")

    
    plt.plot(valuersn, rapport_approximation_couplage, label='Rapport Couplage', color='#FF69B4')
    plt.plot(valuersn, rapport_approximation_glouton, label='Rapport Glouton', color='#800080')
    plt.xlabel('Taille de l\'instance (n)')
    plt.ylabel('Rapport d\'approximation')
    plt.title('Comparaison du rapport d\'approximation (Moyenne sur {} répétitions)'.format(repetitions))
    plt.legend()
    plt.show()
