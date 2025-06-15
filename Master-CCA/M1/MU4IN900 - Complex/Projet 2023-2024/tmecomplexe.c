def sac_a_dos_glouton(poids, utilites, capacite):
    # Nombre d'objets
    n = len(poids)
    
    # Calculer le rapport utilité/poids pour chaque objet
    ratio = [(utilites[i] / poids[i], poids[i], utilites[i]) for i in range(n)]
    
    # Trier les objets par rapport décroissant utilité/poids
    ratio.sort(reverse=True, key=lambda x: x[0])
    
    valeur_totale = 0
    poids_total = 0
    objets_selectionnes = []
    
    # Sélectionner les objets selon l'ordre décroissant des ratios
    for r in ratio:
        if poids_total + r[1] <= capacite:
            objets_selectionnes.append(r)
            poids_total += r[1]
            valeur_totale += r[2]
    
    return objets_selectionnes, valeur_totale, poids_total

# Exemple d'utilisation de l'algorithme glouton
poids = [14, 10, 8, 6, 5, 2]
utilites = [24, 19, 16, 13, 5, 3]
capacite = 20

objets_selectionnes, valeur_totale, poids_total = sac_a_dos_glouton(poids, utilites, capacite)

print("Objets sélectionnés:", objets_selectionnes)
print("Valeur totale:", valeur_totale)
print("Poids total:", poids_total)

