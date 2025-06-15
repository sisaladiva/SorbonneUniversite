import matplotlib.pyplot as plt
import csv

#-------------------------------------------------------------------------------
#                              LIRE LES DONNEES CSV - ROTATIONS D'UN ARBRE CARTESIEN
#-------------------------------------------------------------------------------

# Fonction pour lire les données du fichier CSV généré par ton programme C
"""def lire_donnees_csv(fichier):
    nb_noeuds = []
    rotations_totales = []

    with open(fichier, 'r') as file:
        next(file)  # Passer la première ligne (en-tête)
        for line in file:
            values = line.strip().split(',')
            if len(values) == 2 and values[0].isdigit() and values[1].isdigit():
                nb_noeuds.append(int(values[0]))  # Ajouter le nombre de nœuds
                rotations_totales.append(int(values[1]))  # Ajouter le nombre de rotations totales
        
    return nb_noeuds, rotations_totales

# Lire les données depuis le fichier CSV
nb_noeuds, rotations_totales = lire_donnees_csv('resultats_rotations.csv')

#-------------------------------------------------------------------------------
#                              GRAPHIQUE : NOMBRE DE NŒUDS vs ROTATIONS TOTALES (ARBRE CARTESIEN)
#-------------------------------------------------------------------------------

# Créer un graphique pour visualiser les résultats
plt.figure(figsize=(10, 6))
plt.plot(nb_noeuds, rotations_totales, marker='o', linestyle='-', color='b', label="Rotations Totales")

# Ajouter des titres et des labels
plt.title('Relation entre le nombre de nœuds et les rotations totales dans un arbre cartésien')
plt.xlabel('Nombre de nœuds')
plt.ylabel('Rotations Totales')

# Afficher une légende
plt.legend()

# Ajouter une grille pour mieux visualiser
plt.grid(True)

# Ajuster le graphique pour éviter les chevauchements
plt.tight_layout()

# Afficher le graphique
plt.show()"""

#-------------------------------------------------------------------------------
#                              LIRE LES RESULTATS CSV - TEMPS D'INSERTION (ARBRE CARTESIEN)
#-------------------------------------------------------------------------------

# Fonction pour lire le fichier CSV et extraire les données
"""def lire_resultats_csv(filename):
    tailles = []
    temps = []
    
    with open(filename, 'r') as f:
        next(f)  # Ignorer l'en-tête
        
        for line in f:
            n, t = line.strip().split(',')
            tailles.append(int(n))
            temps.append(float(t))
    
    return tailles, temps

# Fonction pour tracer le graphique avec un scatter plot
def tracer_graphe(tailles, temps, titre, label_x, label_y):
    plt.figure(figsize=(10, 6))
    plt.scatter(tailles, temps, color='b', label='Temps moyen d\'insertion', marker='o')  # Scatter plot
    
    plt.title(titre)
    plt.xlabel(label_x)
    plt.ylabel(label_y)
    
    # Ajouter une grille pour faciliter la lecture
    plt.grid(True)
    
    # Ajuster les limites de l'axe des X (nombre de nœuds)
    plt.xlim(min(tailles), max(tailles))
    
    # Ajuster les limites de l'axe des Y (temps), zoomer un peu
    plt.ylim(0, max(temps) * 1.1)
    
    # Ajouter une légende
    plt.legend()
    
    # Afficher le graphique
    plt.show()"""

#-------------------------------------------------------------------------------
#                              TEMPS D'INSERTION - ARBRE CARTESIEN
#-------------------------------------------------------------------------------

# Lire les données depuis le fichier CSV pour l'arbre cartésien
"""filename = 'temps.csv'  # Le nom de votre fichier CSV
tailles, temps = lire_resultats_csv(filename)  # Lire les données du fichier CSV

# Tracer le graphique pour l'arbre cartésien
tracer_graphe(tailles, temps, 'Temps d\'insertion en fonction du nombre de nœuds (Arbre Cartésien)', 'Nombre de nœuds', 'Temps d\'insertion (secondes)')"""
"""taillesCartesien, tempsCartesien = lire_resultats_csv("temps_moyen_cartesien.csv")
    tracer_graphe(taillesCartesien, tempsCartesien,
                  "Temps moyen d'insertion (Arbre Cartésien)",
                  "Nombre de nœuds", "Temps moyen (secondes)")"""
#-------------------------------------------------------------------------------
#                              TEMPS D'INSERTION - ARBRE BST
#-------------------------------------------------------------------------------

# Lire les données depuis le fichier CSV pour l'arbre BST
"""filenamebst = 'tempsbst.csv'  # Le nom de votre fichier CSV pour l'arbre BST
taillesbst, tempsbst = lire_resultats_csv(filenamebst)  # Lire les données du fichier CSV

# Tracer le graphique pour l'arbre BST
tracer_graphe(taillesbst, tempsbst, 'Temps d\'insertion en fonction du nombre de nœuds (Arbre BST)', 'Nombre de nœuds', 'Temps d\'insertion (secondes)')"""
"""taillesBST, tempsBST = lire_resultats_csv("temps_moyen_bst.csv")
    tracer_graphe(taillesBST, tempsBST,
                  "Temps moyen d'insertion (Arbre BST)",
                  "Nombre de nœuds", "Temps moyen (secondes)")"""
#-------------------------------------------------------------------------------
#                              LIRE LES DONNEES CSV - HAUTEUR DES ARBRES CARTESIENS
#-------------------------------------------------------------------------------

# Lire les données de hauteur pour les arbres cartésiens
"""filename = "hauteurs_arbre.csv"  # Remplacez par le nom de votre fichier CSV
nb_noeuds, hauteurs = [], []

with open(filename, mode='r') as file:
    reader = csv.reader(file)
    next(reader)  # Sauter l'en-tête
    for row in reader:
        nb_noeuds.append(int(row[0]))  # Ajouter le nombre de nœuds
        hauteurs.append(int(row[1]))   # Ajouter la hauteur de l'arbre

#-------------------------------------------------------------------------------
#                              GRAPHIQUE : NOMBRE DE NŒUDS vs HAUTEUR DE L'ARBRE CARTESIEN
#-------------------------------------------------------------------------------

# Créer un graphique pour afficher la relation entre le nombre de nœuds et la hauteur
plt.figure(figsize=(10, 6))
plt.plot(nb_noeuds, hauteurs, marker='o', color='b', label='Hauteur de l\'arbre')

# Ajouter des titres et des labels
plt.title('Hauteur des arbres cartésiens en fonction du nombre de nœuds')
plt.xlabel('Nombre de nœuds')
plt.ylabel('Hauteur de l\'arbre')
plt.grid(True)

# Afficher la légende
plt.legend()

# Afficher le graphique
plt.show()

#-------------------------------------------------------------------------------
#                              LIRE LES DONNEES CSV - HAUTEUR DES ARBRES BST
#-------------------------------------------------------------------------------

# Lire les données de hauteur pour les arbres BST
filename = "hauteurs_arbrebst.csv"  # Remplacez par le nom de votre fichier CSV
nombre_noeuds_bst, hauteurs_bst = [], []

with open(filename, mode='r') as file:
    reader = csv.reader(file)
    next(reader)  # Sauter l'en-tête
    for row in reader:
        nombre_noeuds_bst.append(int(row[0]))  # Ajouter le nombre de nœuds de l'arbre BST
        hauteurs_bst.append(int(row[1]))      # Ajouter la hauteur de l'arbre BST

#-------------------------------------------------------------------------------
#                              GRAPHIQUE : NOMBRE DE NŒUDS vs HAUTEUR DE L'ARBRE BST
#-------------------------------------------------------------------------------

# Créer un graphique pour afficher la relation entre le nombre de nœuds et la hauteur dans un BST
plt.figure(figsize=(10, 6))
plt.plot(nombre_noeuds_bst, hauteurs_bst, marker='o', color='g', label='Hauteur de l\'arbre BST')

# Ajouter des titres et des labels
plt.title('Hauteur des arbres binaires de recherche (BST) en fonction du nombre de nœuds')
plt.xlabel('Nombre de nœuds')
plt.ylabel('Hauteur de l\'arbre')
plt.grid(True)

# Afficher la légende
plt.legend()

# Afficher le graphique
plt.show()

def tracer_graphique_temps_recherche(fichier_csv, titre_graphique, nom_image):
    tailles = []
    temps = []

    # Lecture des données depuis le fichier CSV
    with open(fichier_csv, 'r') as fichier:
        lecteur = csv.reader(fichier)
        next(lecteur)  # Ignorer l'en-tête
        for ligne in lecteur:
            tailles.append(int(ligne[0]))  # Colonne 1 : Taille de l'arbre
            temps.append(float(ligne[1]))  # Colonne 2 : Temps de recherche

    # Tracer le graphique
    plt.figure(figsize=(10, 6))
    plt.plot(tailles, temps, marker='o', linestyle='-', color='r', label='Temps de recherche')

    # Ajouter des labels et un titre
    plt.xlabel('Taille de l\'arbre (nombre de nœuds)', fontsize=12)
    plt.ylabel('Temps de recherche (secondes)', fontsize=12)
    plt.title(titre_graphique, fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend(fontsize=12)

    # Sauvegarder et afficher le graphique
    plt.savefig(nom_image)
    plt.show()

# Tracer le graphique pour l'arbre cartésien
tracer_graphique_temps_recherche(
    fichier_csv="temps_recherche_cartesien.csv",
    titre_graphique="Temps de recherche dans un arbre cartésien",
    nom_image="temps_recherche_cartesien.png"
)
# Tracer le graphique pour l'arbre BST
tracer_graphique_temps_recherche(
    fichier_csv="temps_recherche_bst.csv",
    titre_graphique="Temps de recherche dans un arbre BST",
    nom_image="temps_recherche_bst.png"
)

def tracer_graphique_temps_suppression(fichier_csv, titre_graphique, nom_image):
    tailles = []
    temps = []

    # Lecture des données depuis le fichier CSV
    with open(fichier_csv, 'r') as fichier:
        lecteur = csv.reader(fichier)
        next(lecteur)  # Ignorer l'en-tête
        for ligne in lecteur:
            tailles.append(int(ligne[0]))  # Colonne 1 : Taille de l'arbre
            temps.append(float(ligne[1]))  # Colonne 2 : Temps de suppression

    # Tracer le graphique
    plt.figure(figsize=(10, 6))
    plt.plot(tailles, temps, marker='o', linestyle='-', color='purple', label='Temps de suppression')

    # Ajouter des labels et un titre
    plt.xlabel('Taille de l\'arbre (nombre de nœuds)', fontsize=12)
    plt.ylabel('Temps de suppression (secondes)', fontsize=12)
    plt.title(titre_graphique, fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend(fontsize=12)

    # Sauvegarder et afficher le graphique
    plt.savefig(nom_image)
    plt.show()

# Tracer le graphique pour l'arbre cartésien
tracer_graphique_temps_suppression(
    fichier_csv="temps_suppression_cartesien.csv",
    titre_graphique="Temps de suppression dans un arbre cartésien",
    nom_image="temps_suppression_cartesien.png"
)
# Tracer le graphique pour l'arbre BST
tracer_graphique_temps_suppression(
    fichier_csv="temps_suppression_bst.csv",
    titre_graphique="Temps de suppression dans un arbre BST",
    nom_image="temps_suppression_bst.png"
)"""


"""# Lire les données depuis le fichier CSV
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

# Tracer un graphique
def tracer_graphe(tailles, temps, titre, label_x, label_y, couleur=None, label_legende=None):
    plt.figure(figsize=(10, 6))
    plt.plot(tailles, temps, marker='o', linestyle='-', color=couleur, label=label_legende)
    plt.title(titre)
    plt.xlabel(label_x)
    plt.ylabel(label_y)
    plt.grid(True)
    if label_legende:
        plt.legend()
    plt.tight_layout()
    plt.show()

# Main pour tracer les résultats
if __name__ == "__main__":
    # Arbre cartésien - Insertion
    taillesCartesien, tempsCartesien = lire_resultats_csv("temps_moyen_cartesien.csv")
    tracer_graphe(
        taillesCartesien,
        tempsCartesien,
        "Temps moyen d'insertion (Arbre Cartésien)",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "blue",
        "Arbre Cartésien"
    )

    # Arbre BST - Insertion
    taillesBST, tempsBST = lire_resultats_csv("temps_moyen_bst.csv")
    tracer_graphe(
        taillesBST,
        tempsBST,
        "Temps moyen d'insertion (Arbre BST)",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "blue",
        "Arbre BST"
    )

    # Arbre cartésien - Recherche
    taillesCartesienRecherche, tempsCartesienRecherche = lire_resultats_csv("temps_moyen_recherche_cartesien.csv")
    tracer_graphe(
        taillesCartesienRecherche,
        tempsCartesienRecherche,
        "Temps moyen de recherche (Arbre Cartésien)",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "green",
        "Arbre Cartésien"
    )

    # Arbre BST - Recherche
    taillesBSTRecherche, tempsBSTRecherche = lire_resultats_csv("temps_moyen_recherche_bst.csv")
    tracer_graphe(
        taillesBSTRecherche,
        tempsBSTRecherche,
        "Temps moyen de recherche (Arbre BST)",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "green",
        "Arbre BST"
    )
# Arbre cartésien - Suppression
    taillesCartesienSuppression, tempsCartesienSuppression = lire_resultats_csv("temps_moyen_suppression_cartesien.csv")
    tracer_graphe(
        taillesCartesienSuppression,
        tempsCartesienSuppression,
        "Temps moyen de suppression (Arbre Cartésien)",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "orange",
        "Arbre Cartésien"
    )

    # Arbre BST - Suppression
    taillesBSTSuppression, tempsBSTSuppression = lire_resultats_csv("temps_moyen_suppression_bst.csv")
    tracer_graphe(
        taillesBSTSuppression,
        tempsBSTSuppression,
        "Temps moyen de suppression (Arbre BST)",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "orange",
        "Arbre BST"
    )"""
import numpy as np
import matplotlib.pyplot as plt
import csv

# Lire les données depuis le fichier CSV
def lire_resultats_csv(filename):
    tailles = []
    temps = []

    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Ignorer l'en-tête
        for row in reader:
            tailles.append(int(row[0]))
            temps.append(float(row[1]))
    return tailles, temps

# Tracer un graphique avec courbes théoriques
def tracer_graphe(tailles, temps, titre, label_x, label_y, couleur, label_legende):
    plt.figure(figsize=(10, 6))
    
    # Tracer les résultats expérimentaux
    plt.plot(tailles, temps, marker='o', linestyle='-', color=couleur, label=label_legende)
    
    # Ajouter une courbe théorique O(log n)
    c_log = max(temps) / np.log(max(tailles) + 1)  # Constante pour ajuster O(log n) aux données
    valeurs_log = [c_log * np.log(taille + 1) for taille in tailles]
    plt.plot(tailles, valeurs_log, linestyle='--', color='red', label='$O(\log n)$ (théorique)')
    
    # Ajouter une courbe théorique O(n)
    c_n = max(temps) / max(tailles) # Constante pour ajuster O(n) aux données
    valeurs_n = [c_n * taille for taille in tailles]
    plt.plot(tailles, valeurs_n, linestyle='--', color='purple', label='$O(n)$ (théorique)')

    # Ajouter des détails au graphique
    plt.title(titre)
    plt.xlabel(label_x)
    plt.ylabel(label_y)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()

# Main pour tracer les résultats avec courbes théoriques
if __name__ == "__main__":
    # 1. Insertion
    # Arbre Cartésien - Insertion
    taillesCartesienInsertion, tempsCartesienInsertion = lire_resultats_csv("temps_moyen_cartesien.csv")
    tracer_graphe(
        taillesCartesienInsertion,
        tempsCartesienInsertion,
        "Temps moyen d'insertion (Arbre Cartésien) avec courbes théoriques",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "blue",
        "Insertion - Arbre Cartésien"
    )

    # Arbre BST - Insertion
    taillesBSTInsertion, tempsBSTInsertion = lire_resultats_csv("temps_moyen_bst.csv")
    tracer_graphe(
        taillesBSTInsertion,
        tempsBSTInsertion,
        "Temps moyen d'insertion (Arbre BST) avec courbes théoriques",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "blue",
        "Insertion - Arbre BST"
    )

    # 2. Recherche
    # Arbre Cartésien - Recherche
    taillesCartesienRecherche, tempsCartesienRecherche = lire_resultats_csv("temps_moyen_recherche_cartesien.csv")
    tracer_graphe(
        taillesCartesienRecherche,
        tempsCartesienRecherche,
        "Temps moyen de recherche (Arbre Cartésien) avec courbes théoriques",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "green",
        "Recherche - Arbre Cartésien"
    )

    # Arbre BST - Recherche
    taillesBSTRecherche, tempsBSTRecherche = lire_resultats_csv("temps_moyen_recherche_bst.csv")
    tracer_graphe(
        taillesBSTRecherche,
        tempsBSTRecherche,
        "Temps moyen de recherche (Arbre BST) avec courbes théoriques",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "green",
        "Recherche - Arbre BST"
    )

    # 3. Suppression
    # Arbre Cartésien - Suppression
    taillesCartesienSuppression, tempsCartesienSuppression = lire_resultats_csv("temps_moyen_suppression_cartesien.csv")
    tracer_graphe(
        taillesCartesienSuppression,
        tempsCartesienSuppression,
        "Temps moyen de suppression (Arbre Cartésien) avec courbes théoriques",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "orange",
        "Suppression - Arbre Cartésien"
    )

    # Arbre BST - Suppression
    taillesBSTSuppression, tempsBSTSuppression = lire_resultats_csv("temps_moyen_suppression_bst.csv")
    tracer_graphe(
        taillesBSTSuppression,
        tempsBSTSuppression,
        "Temps moyen de suppression (Arbre BST) avec courbes théoriques",
        "Nombre de nœuds",
        "Temps moyen (secondes)",
        "orange",
        "Suppression - Arbre BST"
    )
# Hauteur moyenne - Arbre Cartésien
    taillesCartesienHauteur, hauteursCartesien = lire_resultats_csv("hauteur_moyenne_cartesien.csv")
    tracer_graphe(
        taillesCartesienHauteur,
        hauteursCartesien,
        "Hauteur moyenne (Arbre Cartésien) avec courbes théoriques",
        "Nombre de nœuds",
        "Hauteur moyenne",
        "violet",
        "Hauteur moyenne - Arbre Cartésien"
    )

    # Hauteur moyenne - Arbre BST
    taillesBSTHauteur, hauteursBST = lire_resultats_csv("hauteur_moyenne_bst.csv")
    tracer_graphe(
        taillesBSTHauteur,
        hauteursBST,
        "Hauteur moyenne (Arbre BST) avec courbes théoriques",
        "Nombre de nœuds",
        "Hauteur moyenne",
        "violet",
        "Hauteur moyenne - Arbre BST"
    )
    # Arbre Cartésien
    taillesCartesien, balancesCartesien = lire_resultats_csv("balance_moyenne_cartesien.csv")
    tracer_graphe(
        taillesCartesien,
        balancesCartesien,
        "Balance moyenne des sous-arbres (Arbre Cartésien)",
        "Nombre de nœuds",
        "Balance moyenne",
        "grey",
        "Balance - Arbre Cartésien"
    )

    # Arbre BST
    taillesBST, balancesBST = lire_resultats_csv("balance_moyenne_bst.csv")
    tracer_graphe(
        taillesBST,
        balancesBST,
        "Balance moyenne des sous-arbres (Arbre BST)",
        "Nombre de nœuds",
        "Balance moyenne",
        "grey",
        "Balance - Arbre BST"
    )




