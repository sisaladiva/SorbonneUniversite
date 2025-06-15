# Corrigé TD4

## Exercice 1

Écrire en assembleur Mips-32 la boucle principale de GetListLength . On suppose que R4 contient l'adresse de la liste (pt). Analyser l'exécution de la boucle à l'aide d'un schéma simplifié. On considère que R4 contient *pt et R2 la variable i.

```arm-asm
    ori     r2, r0, 0
    beq     r4, r0, endwhile
    nop
while :
    addi    r2, r2, 1
    lw      r4, 0(r4)
    bne     r4, r0, while
    nop
endwhile:
    jr      r31
    nop
```

`#cycles/iter = 6`\
`#inst/iter = 4`\
`CPI = \frac{6}{4} = 1,5 cycles/inst`\
`CPI_{utile} = \frac{6}{3} = 2 cycles/inst`

## Exercice 2

### Question a

*Modifier le programme pour qu'il soit exécutable sur le Mips-32.*

```arm-asm
loop:
    lbu     r8, 0(r4)       # lire un pixel
    addu    r8, r8, r5
    lbu     r9, 0(r8)       # lire f(pixel)
    sb      r9, 0(r4)

    addiu   r4, r4, 1
    bne     r4, r10, loop
    nop                     # on ajoute un nop (delayed slot)
```

### Question b

*Analyser, à l'aide d'un schéma simplifié, l'exécution de ce programme dans le Mips. Calculer le nombre de cycles pour effectuer une itération.*

$\#cycles/iter = 10$
$\#inst/iter = 7$
$CPI = \frac{10}{7} = 1,4 \:cycles/inst$
$CPI_{utile} = \frac{10}{6} = 1,6 \:cycles/inst$`

### Question c

*Optimiser le code en changeant l'ordre des instructions de manière à obtenir un CPI et un CPI-utile de 1.*

**Étape 1 : Trouver les dépendances de données**

```arm-asm
loop:
    (1) lbu     r8, 0(r4)       # lire un pixel
    (2) addu    r8, r8, r5
    (3) lbu     r9, 0(r8)       # lire f(pixel)
    (4) sb      r9, 0(r4)

    (5) addiu   r4, r4, 1
    (6) bne     r4, r10, loop
    (7) nop                     # on ajoute un nop (delayed slot)
```

$\rightarrow$ : "est dépendant de"

$(2) \rightarrow (1)$ [R8]

$(3) \rightarrow (2)$ [R8]

$(4) \rightarrow (3)$ [R9]

$(6) \rightarrow (5)$ [R4]

**Étape 2 : Identification des cycles perdus**

$(2) \rightarrow (1)$ : 1 cycle perdu

$(4) \rightarrow (3)$ : 1 cycle perdu

$(6) \rightarrow (5)$ : 1 cycle perdu

$(7)$ : 1 cycle perdu

**Étape 3 : Schéma des dépendances**

Attention ici la flèche n'a pas le même sens qu'à l'étape précédente ! Chaque ligne désigne une "chaîne" de dépendance. Les instructions qui ne sont pas reliées entre elles sont indépendantes, l'une de l'autre.
Il est conseillé de dessiner ce graphe à la vertical à côté du code.

$\rightarrow$ : " **dépend de**"

Le numéro par dessus la flèche désigne le nombre de cycle perdu.

$(1) \xrightarrow{1} (2) \rightarrow (3) \xrightarrow{1} (4)$

$(5) \xrightarrow{1} (6) (7)$

Ainsi (4) et (5) ne sont pas dépendantes.

**Étape 4 : Réordonnacement du code**

Le but est de combler les cycles perdus par des instructions.

Le schéma des dependances donne ainsi :
$(1) \rightarrow (5) \rightarrow (2) \rightarrow (3) \rightarrow (6) \rightarrow (4)$

```arm-asm
loop:
    (1) lbu     r8, 0(r4)       # lire un pixel
    (5) addiu   r4, r4, 1
    (2) addu    r8, r8, r5
    (3) lbu     r9, 0(r8)       # lire f(pixel)
    (6) bne     r4, r10, loop
    (4) sb      r9, -1(r4)      # r4 est incremente avant le sb d'ou -1
```

L'incrémentation de r4 (5) se fait avant qu'on y ait enregistré la valeur de r9, ainsi, lors de l'execution de (4), r4 contient la prochaine adresse d'où le `-1(r4)` et non `0(r4)`.

`#cycles/iter = 6`\
`#inst/iter = 6`\
`CPI = CPI_{utile} = \frac{6}{6} = 1 cycles/inst`

### Question d

*Optimiser le code en utilisant la technique du "pipeline logiciel".*
*Calculer le nombre de cycles par itération.*

**Optimisation par déroulage de boucle :**

À chaque itération, dans le code non-optimisé, on ne traitait qu'un seul élèment. Si l'on en traite 2 par itération, le temps d'execution de la boucle en sera subséquement réduit. De même que si l'on en traite 3 et ainsi de suite. Le nombre d'instruction se trouve quand à lui augmenter et permet plus de réordonnancement.

**Pourquoi traiter 2 élèments par itérations est plus interessant que d'en traiter 3, 4 ou plus ?**

Une des raisons est lorsque le nombre d'éléments traités par itération augmente, la proportion des overheads (tels que les instructions de contrôle de la boucle et les instructions de branchement) par rapport au calcul réel peut diminuer. Cela signifie que l'impact relatif de l'overhead de la boucle sur le temps d'exécution total devient plus faible à mesure que l'on traite plus d'éléments par itération. Par conséquent, le bénéfice tiré de la réduction de l'overhead de la boucle peut ne pas être aussi prononcé lorsque l'on passe de 3 éléments à 4 éléments par itération.

```arm-asm
loop:
    lbu     r8, 0(r4)
    addu    r8, r8, r5
    lbu     r9, 0(r8)
    sb      r9, 0(r4)

    lbu     r10, 1(r4)
    addu    r6, r6, r5
    lbu     r7, 0(r6)
    sb      r7, 1(r4)

    addiu   r4, r4, 2
    bne     r4, r10, loop
    nop
```

- Séparer le **traitement** de la gestion de la boucle
- Déterminer les dépendances de données dans le **traitement**
- Indiquer les pertes de performances (cycles de gel)
- Découper le traitement sur ces pertes (en étages qu'on nomera E1,
    E2, …, Ei
- Vérifier que les **règles du pipeline** sont respectées :
  - Chaque étape et opération doit être séparée par un registre : si
        non respecté, on ajoute des instructions de transfert ou on
        modifie le découpage
  - Chaque matériel est utiliser dans un unique étage : si non
        respectés, renommer les registres (en utiliser d'autres)
- Réorganiser sous la forme $En_i, …, E0_{n+i}$
- Effectuer le réordonnancement de la gestion de boucle
