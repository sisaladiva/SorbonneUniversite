



import matplotlib.pyplot as plt

# Données pour Écotype
ecotype_data = {
    "n": [10, 11, 12, 13, 20, 25, 28, 30, 31,32,34],
    "processes": [2, 2, 2, 2, 2, 4, 4, 4, 4, 32],
    "execution_time": [0.00073, 0.00099, 0.00151, 0.00294, 0.32343, 3.72592, 31.62059, 126.69334, 256.49270, 427.5367,3634.68540]
}

# Données pour Gros
gros_data = {
    "n": [10, 11, 12, 13, 20, 25, 28, 30, 31,32,34],
    "processes": [2, 2, 2, 2, 2, 4, 4, 4, 4],
    "execution_time": [0.00041, 0.000605, 0.001015, 0.002075, 0.247085, 3.2322025, 26.11319, 104.43656, 212.36820, 380.5684,2906.3754 ]
}

# Création du graphe
plt.figure(figsize=(10, 6))

# Tracer les données pour Écotype
plt.plot(ecotype_data["n"], ecotype_data["execution_time"], marker="o", label="Écotype", color="blue")

# Tracer les données pour Gros
plt.plot(gros_data["n"], gros_data["execution_time"], marker="s", label="Gros", color="green")

# Configuration des axes et du titre
plt.xscale("linear")
plt.yscale("log")  # Échelle logarithmique pour mieux visualiser les grandes valeurs
plt.xlabel("Valeur de n", fontsize=12)
plt.ylabel("Temps d'exécution (s)", fontsize=12)
plt.title("Comparaison des temps d'exécution entre Écotype et Gros", fontsize=14)
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)

# Affichage
plt.tight_layout()
plt.show()
