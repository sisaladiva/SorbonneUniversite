21/09/2023

**Emulation d'instructions** : Cuando podemos realizar una tarea sin que el hardware lo realice, es decir "emulamos" la instrucción de otras maneras.
Multi tâche : Multiplexage témporaire

Un CPU d'un seul coeur peut exécuter qu'une tâche séquentiellement, si il y en a plus de tâches le processeur alterne entre les différentes tâches (*commutation, quantum*) et sauvegarde le contexte du processus dans la pile. Il prend un *snapshot*, commute à une autre tache et une fois fini il reprend le contexte du *snapshot* afin de continuer avec la tâche.

Une instruction `SUBI` n'est pas nécessaire car il existe déjà une instruction qui peut éffectuer une substraction, c'est `ADDI`. Il faut utiliser cet instruction là avec un nombre négatif. Puisque l'espace est limité, il faut réduire la quantité d'*opcodes* disponibles.

Pas de Nori car pas assez d'opcodes (coute trop cher en espace)
Nor existe car on utilise `func`
Si instructino pas bcp utile, on trouve d'autres moyens -> Loi d'amdhal

**Loi d'Amdhal**
$gain_{reel} = gain_{theorique} * taux\ d'utilisation$

Les adresses mémoire sont alignées à 4 bits, il faut donc multiplier les addresses par 4.
@cible = @B^t + 4 + I + 4
Note: @ est l'adresse mémoire (& en C)

# Réalisation

Dans le monde de la réalisation on peut toujours faire le choix, entre performance et coût. On doit trouver un équilibre.

## RISC vs CISC

### IBM 370 (CISC)

Les processeurs CISC pendant les annés 70s, la machine **IBM 370** (1976) ont été crées pour faire principalement des transactions bancaires. C'était donc une machine **transactionelle**.

Exemple, l'instruction de C `strcmp` est réalisé pendant 1 cycle d'horloge pour ce type de machines. On peut voir donc la complexité des processeurs CISC.

### Vax 800 (CISC)

Le processeur **Vax 800** (1978) sont utilisés pour des calculs **scientifiques**.

Exemple, le Vax 800 a une instruction `ADD` assez complexe en utilisant des adresses mémoire au lieu des registres ou nombres immediats

``` asm
ADD @a, @b, @c
```

La philosophie des processeurs RISC est de rester simple.

Exemple : Différence entre les machines Vax et MIPS

```c
// Instruction en C d'une simple somme
a = b + c;
```

| Vax 800          | Mips32           |
| ---------------- | ---------------- |
| `Add @a, @b, @c` | `LW  r4, @b`     |
|                  | `LW  r5, @c`     |
|                  | `ADD r6, r4, r5` |
|                  | `SW  r6, @a`     |

Example : Languages

Imaginemos que apenas llevo aprendiendo un idioma, a comparación de alguien que ya lleva mucho tiempo aprendiéndolo (o incluso es nativo)
Para expresar una idea la otra persona tiene a su repertorio un lenguage bastante complejo, por lo que necesita pocas palabras.
En cambio, la persona que es nueva, necesita expresar la idea con varias palabras simples to get the point across.

Vax vs MIPS, cuál es mejor?

Tout d'abord, avant de répondre à cette question il faut définir le **critère de comparaison**. Une foit définit cela on peut commencer à juger.

Temps d'éxecution ? Taille ?

## Evaluations de performance

1. Consommation électrique (Efficiency)
2. Temps d'éxecution
3. Taille du circuit

$Performance = \dfrac{F}{CPI}$

*CPI = Cycle Per Instruction*

RISC

$CPI_{min} = {1}/{instr}$

| D'  | D   | HW                    | ADD | LW  | SW  | JR  |
| --- | --- | --------------------- | --- | --- | --- | --- |
|     | D   | Get Instruction       | x   | x   | x   | x   |
|     | D   | Decode                | x   | x   | x   | x   |
| D   | D   | Preparer les operands | x   | x   | x   | x   |
|     | D   | Operation             | x   | x   | x   |     |
|     | D   | Accès mémoire         |     | x   | x   |     |
|     | D   | Store the result      | x   | x   |     |     |
| D   |     | @Instruction Suivante | x   | x   | x   | x   |

**Operands**
Add (Addition) : Rd, Rs, Rt
LW (Load Word) : Rd, J(Rs)
SW (Store Word) : Rs, Rt, I
JR : Rd

Note: D -> *Dépendence*
