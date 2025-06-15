import matplotlib.pyplot as plt
import csv
import numpy as np

#-------------------------------------------------------------------------------
#                              LIRE LES DONNEES CSV
#-------------------------------------------------------------------------------

# Fonction générique pour lire un fichier CSV
def lire_donnees_csv(fichier, has_header=True):
    x_vals = []
    y_vals = []
    
    with open(fichier, 'r') as file:
        reader = csv.reader(file)
        if has_header:
            next(reader)  # Ignorer la ligne d'en-tête
        
        for row in reader:
            x_vals.append(int(row[0]))  # Colonne X (par exemple : nombre de nœuds)
            y_vals.append(float(row[1]))  # Colonne Y (par exemple : temps ou hauteur)

    return x_vals, y_vals

#-------------------------------------------------------------------------------
#                              TRACER LES GRAPHIQUES
#-------------------------------------------------------------------------------

# Fonction générique pour tracer un graphique
def tracer_graphique(x, y, titre, xlabel, ylabel, couleur='b', label=None):
    plt.figure(figsize=(10, 6))
    plt.plot(x, y, marker='o', color=couleur, label=label if label else ylabel)

    # Ajouter des titres et des labels
    plt.title(titre, fontsize=14)
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    
    # Options supplémentaires
    plt.grid(True, linestyle='--', alpha=0.6)
    if label:
        plt.legend(fontsize=12)

    # Ajuster le layout
    plt.tight_layout()
    
    # Afficher le graphique
    plt.show()

#-------------------------------------------------------------------------------
#                              AFFICHER LES GRAPHIQUES SPECIFIQUES
#-------------------------------------------------------------------------------

# 1. Nombre de nœuds vs Rotations totales (Arbre Cartésien)
fichier_rotations = 'resultats_rotations.csv'
x_rot, y_rot = lire_donnees_csv(fichier_rotations)
tracer_graphique(
    x_rot, y_rot,
    "Nombre de nœuds vs Rotations totales (Arbre Cartésien)",
    "Nombre de nœuds", "Rotations totales", couleur='b'
)


# 4. Hauteur des arbres cartésiens
fichier_hauteur_cart = 'hauteurs_arbre.csv'
x_hauteur_cart, y_hauteur_cart = lire_donnees_csv(fichier_hauteur_cart)
tracer_graphique(
    x_hauteur_cart, y_hauteur_cart,
    "Hauteur des arbres cartésiens en fonction du nombre de nœuds",
    "Nombre de nœuds", "Hauteur de l'arbre", couleur='purple'
)

# 5. Hauteur des arbres BST
fichier_hauteur_bst = 'hauteurs_arbrebst.csv'
x_hauteur_bst, y_hauteur_bst = lire_donnees_csv(fichier_hauteur_bst)
tracer_graphique(
    x_hauteur_bst, y_hauteur_bst,
    "Hauteur des arbres BST en fonction du nombre de nœuds",
    "Nombre de nœuds", "Hauteur de l'arbre", couleur='orange'
)

# 6. Temps de recherche (Arbre BST)
fichier_temps_recherche_bst = 'temps_recherche_bst.csv'
x_recherche_bst, y_recherche_bst = lire_donnees_csv(fichier_temps_recherche_bst)
tracer_graphique(
    x_recherche_bst, y_recherche_bst,
    "Temps de recherche en fonction du nombre de nœuds (Arbre BST)",
    "Nombre de nœuds", "Temps de recherche (secondes)", couleur='cyan'
)

# 7. Temps de recherche (Arbre Cartésien)
fichier_temps_recherche_cart = 'tempsrecherche.csv'
x_recherche_cart, y_recherche_cart = lire_donnees_csv(fichier_temps_recherche_cart)
tracer_graphique(
    x_recherche_cart, y_recherche_cart,
    "Temps de recherche en fonction du nombre de nœuds (Arbre Cartésien)",
    "Nombre de nœuds", "Temps de recherche (secondes)", couleur='magenta'
)








# Fonction pour lire les données du fichier CSV
def lire_donnees(fichier_csv):
    tailles = []
    temps = []

    with open(fichier_csv, newline='', encoding='utf-8') as csvfile:
        lecteur = csv.reader(csvfile)
        next(lecteur)  # Sauter l'en-tête
        for ligne in lecteur:
            tailles.append(int(ligne[0]))  # Nombre de nœuds
            temps.append(float(ligne[1]))  # Temps de suppression

    return tailles, temps

# Fonction pour tracer les résultats
def tracer_graphique(tailles, temps):
    plt.plot(tailles, temps, marker='o', color='b', label='Temps de suppression')
    plt.title('Temps de suppression dans arbre cartesien de recherche')
    plt.xlabel('Nombre de nœuds')
    plt.ylabel('Temps de suppression (secondes)')
    plt.grid(True)
    plt.legend()
    plt.show()

# Charger les données et afficher le graphique
fichier_csv = "tempssuppr.csv"
tailles, temps = lire_donnees(fichier_csv)
tracer_graphique(tailles, temps)

def lire_resultats_csv(filename):
    tailles = []
    temps = []

    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Ignorer l'en-tête
        for row in reader:
            tailles.append(int(row[0]))  # Convertir la première colonne en entier
            temps.append(float(row[1]))  # Convertir la deuxième colonne en flottant
    return tailles, temps

# Tracer un graphique avec échelle logarithmique sur l'axe des Y et courbes théoriques O(log n) et O(n)
def tracer_graphe_logy_avec_logn(tailles, temps, titre, label_x, label_y):
    plt.figure(figsize=(10, 6))
    plt.plot(tailles, temps, marker='o', linestyle='-', label='Temps moyen')
    
    # Tracer la courbe théorique O(log n)
    tailles_theoriques = np.array(tailles)
    temps_theorique_logn = np.log(tailles_theoriques) * 0.001  # Ajustez cette constante selon vos données
    plt.plot(tailles_theoriques, temps_theorique_logn, linestyle='--', label="Courbe théorique O(log n)", color='r')
    
    # Tracer la courbe théorique O(n)
    temps_theorique_on = tailles_theoriques * 0.01  # Ajuste ce facteur selon tes données
    plt.plot(tailles_theoriques, temps_theorique_on, linestyle='-.', label="Courbe théorique O(n)", color='g')

    plt.yscale('log')  # Appliquer l'échelle logarithmique à l'axe des Y
    plt.title(titre)
    plt.xlabel(label_x)
    plt.ylabel(label_y)
    plt.grid(True, which="both", ls="--")  # Ajouter une grille sur les deux axes
    plt.legend()
    plt.tight_layout()
    plt.show()

# Main pour tracer les résultats
if __name__ == "__main__":
    # Arbre cartésien
    taillesCartesien, tempsCartesien = lire_resultats_csv("temps_moyen_cartesien.csv")
    tracer_graphe_logy_avec_logn(
        taillesCartesien,
        tempsCartesien,
        "Temps moyen d'insertion (Arbre Cartésien)",
        "Nombre de nœuds",
        "Temps moyen (log échelle)"
    )

    # Arbre BST
    taillesBST, tempsBST = lire_resultats_csv("temps_moyen_bst.csv")
    tracer_graphe_logy_avec_logn(
        taillesBST,
        tempsBST,
        "Temps moyen d'insertion (Arbre BST)",
        "Nombre de nœuds",
        "Temps moyen (log échelle)"
    )
