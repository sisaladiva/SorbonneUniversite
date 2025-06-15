5/10/23

# Pipeline

## Dépendance de données

### RAW : Read After Write

Utilisation d'un résultat

### WAW : Write After Write

Réutilisation d'un registre

### WAR : Write After Read

Réutilisation d'un registre


Its always better to try not to use `NOP` instructions

**Aléas dans le pipeline = Pipeline Hazard**
**cycle de gel = stall cycle (bubble)**

Registers can be written three different ways:

``` arm-asm
ADD R3, R4, R5
ADD r3, r4, r5
Add $3, $4, $5
```

Le bypass MEM - MEM n'existe (on pourrait le faire) pas car l'accès à la mémoire est partie du chemin critique, si on ajoute du matériel dans cet étage le performance va se degrader pour la latence.

## Exemple de code

```arm-asm
# Code ASM compilé
XOR   R8, R8, R8
BEQ   R6, R0, suite
SLL   R9, R6, 2
ADD   R9, R9, R5

loop:
LW    R4, 0(R5)
SLL   R7, R4, 1
ADDIU R5, R5, 4
BNE   R9, R5, loop
```

1-Insérer les NOPs

```arm-asm
# Code ASM pour MIPS32
XOR   R8, R8, R8
BEQ   R6, R0, suite
NOP
SLL   R9, R6, 2
ADD   R9, R9, R5

loop:
LW    R4, 0(R5)
SLL   R7, R4, 1
SW    R7, 0(R5)
ADDIU R5, R5, 4
BNE   R9, R5, loop
NOP
```

2-Identifier les dépendences

```arm-asm
# Code ASM pour MIPS32
 1. XOR   R8, R8, R8
 2. BEQ   R6, R0, suite
 3. NOP
 4. SLL   *R9*, R6, 2
 5. ADD   R9, *R9*, R5

loop:
 6. LW    +R4+, 0(R5)
 7. SLL   *R7*, +R4+, 1
 8. SW    *R7*, 0(R5)
 9. ADDIU *R5*, R5, 4
10. BNE   R9, *R5*, loop
11. NOP
```

3-Dépendance Schéma

| #   | Cycle   | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  |
| --- | ------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1   | `XOR`   | I   | D   | E   | M   | W   |     |     |     |     |     |     |     |     |     |     |     |     |
| 2   | `BEQ`   |     | I   | D   | E   | M   | W   |     |     |     |     |     |     |     |     |     |     |     |
| 3   | `NOP`   |     |     | I   | D   | E   | M   | W   |     |     |     |     |     |     |     |     |     |     |
| 4   | `SLL`   |     |     |     | I   | D   | E   | \|M | W   |     |     |     |     |     |     |     |     |     |
| 5   | `ADD`   |     |     |     |     | I   | D   | \|E | M   | W   | ▽   |     |     |     |     |     |     |     |
| 6   | `LW`    |     |     |     |     |     | I   | D   | E   | M   | \|W | ▽   | ▽   |     |     |     |     |     |
| 7   | `SLL`   |     |     |     |     |     |     | I   | D   | O   | \|E | M   | W   | ▽   |     |     |     |     |
| 8   | `SW`    |     |     |     |     |     |     |     | I   | O   | D   | E   | M   | W   | ▽   |     |     |     |
| 9   | `ADDIU` |     |     |     |     |     |     |     |     |     | I   | D   | E   | \|M | W   | ▽   | ▽   |     |
| 10  | `BNE`   |     |     |     |     |     |     |     |     |     |     | I   | O   | \|D | E   | M   | W   | ▽   |
| 11  | `NOP`   |     |     |     |     |     |     |     |     |     |     |     |     | I   | D   | E   | M   | W   |

Cycles pour itération de boucle:

- $8\: {cycles}/{iteration}$
- $6\: instructions$
- $CPI=\dfrac{8}{6}$
- $CPI_{utile}=\dfrac{8}{5}$

# Réordonnancement

1-Dérouler

```
loop:
LW    R4, 0(R5) # iteration 1
SLL   R7, R4, 1
SW    R7, 0(R5)
ADDIU R5, R5, 4
BNE   R9, R5, loop
NOP
#------------------------------
LW    R4, 0(R5) # iteration 2
SLL   R7, R4, 1
SW    R7, 0(R5)
ADDIU R5, R5, 4
BNE   R9, R5, loop
NOP
```

2-

```
loop:
LW    R4, 0(R5) # iteration 3
SLL   R7, R4, 1
SW    R7, 0(R5)

LW    R14, 4(R5)
SLL   R17, R14, 1
SW    R17, 4(R5)

ADDIU R5, R5, 8
BNE   R9, R5, loop
NOP
```

3-

```
loop:
LW    R4, 0(R5)
LW    R14, 4(R5)
SLL   R17, R4, 1
SLL   R17, R14, 1

ADDIU R5, R5, 8

SW    R7, -8(R5)
BNE   R5, R9, loop
SW    R17, -4(R5)
```
