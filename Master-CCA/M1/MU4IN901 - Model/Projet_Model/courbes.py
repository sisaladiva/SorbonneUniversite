import matplotlib.pyplot as plt

# Lire les données du fichier
with open('times.txt', 'r') as f:
    lines = f.readlines()

# Séparer les temps d'exécution de multiply_polynomials et multiply_polynomials_fft
times_multiply = [float(line.split(',')[0]) for line in lines]
times_fft = [float(line.split(',')[1]) for line in lines]

# Créer un range pour l'axe des x
x = range(1, len(times_multiply) + 1)

# Tracer les données
plt.figure(figsize=(10, 6))
plt.plot(x, times_multiply, label='multiply_polynomials')
plt.plot(x, times_fft, label='multiply_polynomials_fft')
plt.xlabel('Exécution')
plt.ylabel('Temps (secondes)')
plt.title('Temps d\'exécution de multiply_polynomials et multiply_polynomials_fft')
plt.legend()
plt.show()

