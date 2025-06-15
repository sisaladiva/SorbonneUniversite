# Cache types

1. Direct Mapping
2. Set Associative
3. Fully Associative

![[slide_8.jpg]]

# "Un cache est un reservoir"

Imaginemos que estamos en el cine y tienes ganas de palomitas, te levantas de tu asiento para ir por una palomita, la tomas, regresas y te la comes. Si tienes ganas de otra palomita, tienes que volver a levantarte, ir por una palomita, y regresar.

Esto toma bastante tiempo.

Un cache soluciona este problema al evitar tomar palomita por palomita. En cambio, tomas una bolsa (*cache*) y la llenas de varias palomitas (*block* or *line*).

Así, la próxima vez si tienes ganas de más palomitas, no es necesario levantarse e ir por palomitas, ya que tienes un pequeño *cache* de palomitas en la bolsa. Esto nos ayuda a explotar la **localidad espacial** y **temporal**.

## Bus sytème

El cache del CPU, la RAM, y otros periféricos están conectados a un bus de datos común. Puesto a que este bus está muy concurrido, hay que pedir acceso para poder utilisarlo, lo que toma tiempo (ciclos de reloj).

Retomando el ejemplo de las palomitas, el bus sisteme podría ser visto como una fila para el área de comida del cine. Si uno quiere ir por más palomitas, se tiene esperar a que la gente avance y libere la file (*data bus*) para poder pedir palomitas (*block* or *line*) y transferirlas a la bolsa (*cache*).

## Miss types: The Three C’s

- **Cold (compulsory) Miss**: Never seen this address before, hence it is not in the cache.
- **Conflict Miss**: Cache associativity is too low. A new value overwrites an old value.
- **Capacity Miss**: Cache is too small, the data that needs to be used does not fit the cache.

# Write Strategies

## Write-through cache

The RAM will always have the updated version of the data because the processor writes through the cache (skips it) and writes directly to the RAM.

Avec un **BEP** (Buffer d'Écriture Posté), si il existe un débit d'entrée d'instructions supérieur au débit de sortie d'instructions, on aura des cycles de gel.

Expliquer les pertes de performance sur les caches: Débit d'écriture < Débit de consommation

**Write-through cache does not have Write Misses!** (since we don't write to the cache).

## Write-back cache

Écrire dans le cache directement, cela veut dire que la version dans la RAM ne sera pas la version plus à jour

Ensemble des lignes de cache:

Degré d'associativité: Nb d'ensembles par ensemble

No puede haber una lectura después de una escritura de manera inmediata. Se debe esperar al menos 1 ciclo.

Hay siempre dos casos para el miss:
- **Clean** Miss:
- **Dirty** Miss:

BEP = boite aux lettres. Si tenemos un BEP de un place, un cartero viene y deja una carta. El hecho de ir a recogerla para leerla toma x cycles.

Si nuestro BEP es de dos lugares, podemos esperar a que haya 2 cartas para después ir a buscarlas, tomando x*2 cycles.

Miss rate = # Misses / Iterations

Autrement dit:

Miss rate (lecture) = $\dfrac{(Nb \:of \:loads)*(\dfrac{iterations}{frequency\: of\: miss})}{Nb \: of \: memory \: accesses}$
