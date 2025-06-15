12/10/23

Il faut éviter les stall cycles

Compilateur => Optimiseur du code
Le compilateur ne doit pas forcement savoir que sont les "stall cycles", etc etc

# Techniques d'optimisations

1. Déroulement de boucle: No se puede deshacer cualquier bucle, simplemente se puede para los bucles **for** debido a que sabemos el número de iteraciones

# Caracteristicas procesador RISC

1. Toutes les instructions ont la meme taille (codées sur 4 bytes)
2. Toutes les instructions suivent le même schéma d'éxecution
	1. -> ${CPI} = 1$

Antes se hacía el procesador en función del compilador, los procesadores RISC rompieron con este esquema.
Ahora los compiladores (el software) se hacen en función del procesador (hardware).
