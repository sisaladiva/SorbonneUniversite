# Architecture des Processeurs

Toutes les informations dont on va disposer pour utiliser quelque chose
L'architecture définit comme un utilisateur voit cet objet.

Répond à la question : Qu'est-ce qu'il fait (**What**)

# Réalisation

Réalisation est le point de vue de conception, de la personne qui fabrique l'objet. Il faut définir l'architecture avant la conception.

Répond à la question : Comment il fait (**How**)

# Differences entre Architecture et Réalisation

Exemple : le fait qu'un processeur est de 1GHz ou plus n'importe pas au niveau de l'utilisateur (architecture) car le code s'éxecute également.

Exemple : Le manuel d'utilisation d'une voiture est au niveau de l'utilisateur (architecture) car le type de moteur, le nombre de pistons, etc sont plutôt au niveau du concepteur (réalistation)

# Architecture des Processeurs

1. **Registres** : Visibles (pas les registres physiques)
2. **Jeux d'instructions** : définit l'ensemble d'instructions pour écire un programme.
3. **Mémoire** : L'abstraction du processeur avec l'interface de la mémoire, le vue du processeur de la mémoire.
4. **Excéptions / Interruptions / Reset (M2 SESI)**

Note: 1-3 prends 20% de temps pour le concepteur, le point 4 prends 80% de temps pour le concepteur -> plus compliqué

Exemple : Voiture

- Interruption : Car crash -> Airbag
- Reset : Démarrage

# Histoire

1980 - Processeur MIPS_32

University of Standford : [MIPS (Microprocessor without Interlocked Pipelined Stages)](https://en.wikipedia.org/wiki/MIPS_architecture) by John Henessy
University of Berkeley : [RISC I (Reduced Instruction Set Computer)](https://en.wikipedia.org/wiki/Berkeley_RISC) by David Patterson

After the "war" between these two universities, J. Henessy and D. Patterson teamed up and wrote a book centered on MIPS processors [Computer Architecture : A quantitative approach](<file://Users/giorgio/Library/CloudStorage/OneDrive-Personal/Sorbonne Université/2023/Semestre 1/ARCHI/Computer Architecture A Quantitative Approach (5th edition).pdf>)

The latest RISC architecture is RISC V, which is basically a copy of the MIPS architecture

# MIPS 32-bit

## Registers

### Registres Numerotés

Registres de 32 bits qui contient des numéros éntiers

R0 - R31

R0 : Poubelle (Trash register) ne peut pas sauvegarder des numéros, il contient que 0s

R31 : Lien (Link) : Sauvegarde l'adresse de retour quand on éxecute une fonction, c'est tjrs le derner adresse

### Named Registers

Hi : 32-bit
Lo : 32-bit

Used for multiplications and divisions since the numbers won't fit a normal 32-bit register

## Instruction Set

[MIPS Reference Sheet](https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_help.html)

Instructions are written in two languages:
- Assembly
- Machine (ASM code translated to bits)

Instructions are one of the three following types:
- R : Register
- I : Immediate (Constant)
- J : Jump

``` arm-asm
ADD R3, R4, R5
```

Shift ammount

64 opcode 2^6 = 64

## R-Type Instructions

| 6     | 5   | 5   | 5   | 5     | 6    |
| ----- | --- | --- | --- | ----- | ---- |
| Opcod | Rs  | Rt  | Rd  | ShAmt | Func |

R-Type instructions use the Func bits to code further "opcodes" to save space

## I-Type Instructions

## J-Type Instructions

| 6     | 5   | 5   | 16  |
| ----- | --- | --- | --- |
| Opcod | Rs  | Rd  | Imd |

| 6     | 26  |
| ----- | --- |
| Opcod | x   |

1. C
2. Accès Mémoire
3. Contrôle
4. **Systême**

``` arm-asm
ADD Rd, Rs, Rt // (Risk of overflow exception)
```

Rd <- Rs + Rt
32 + 32 bits = Overflow!

``` arm-asm
ADDU Rd, Rs, Rt // Unsigned (No overflow exception)
SUBU Rd, Rs, Rt // Unsigned (No overflow exception)
```
