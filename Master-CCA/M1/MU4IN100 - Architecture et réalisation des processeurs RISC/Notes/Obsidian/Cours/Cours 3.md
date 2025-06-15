# Pipelines

On peut exécuter n'importe quoi dans un cycle d'horloge car on définit l'unité de temps.

$Performance = \dfrac{F}{CPI}$

Est-il possible d'augmenter la fréquence de fonctionnement sans toucher le $CPI = 1$ ?

Cuando una instruccion se ejecuta en el HW, estas se propagan a través de los circuitos internos (compuertas lógicas, etc).

```
                                     ┌───┐   ┌───┐   ┌───┐
                            |     ┌──┤Ope├───┤Mem├───┤Sav├───┐
                            v     │  └───┘   └───┘   └───┘   │
                         here     │                          │
         ┌───┐   ┌───┐   ┌───┐    │                          │
=========┤Rea├===┤Dec├===┤Ope├────┤                          ├────────────
         └───┘   └───┘   └───┘    │                          │
                                  │         ┌───┐            │
                                  └─────────┤ @ ├────────────┘
                                            └───┘
```

Si tomamos una snapshot en un momento t (ejemplo calculo de operandos) podemos ver que las instrucciones anteriores ya no trabajan (o más bien, han terminado de ejecutarse) y las demás instrucciones no se están ejecutando tampoco.

Ejemplo: Una linea de produccion de autos con obreros.

En lugar de que un solo obrero trabaje con un coche, podemos crear una línea de producción para que varios obreros trabajen cada uno con una etapa diferente de la construcción del automóvil (ventanas, ruedas, motor, etc).

Para evitar que las señales interfieran con las demás instrucciones, ponemos una barrera. Esta barrera es un registro.

# Pipeline design

Aumenta la frecuencia de funcionamiento sin modificar el CPI.
Le pipeline permet d'augmenter le débit d'éxecution des instructions.

**Latence**: Temps entre la sortie et l'entrée d'une instruction
**Débit**: Temps entre la sortie 1 et la sortie 2

Imagina una carretera, en lugar de que un coche C1 vaya del punto A al punto B, termine y enseguida pase otro coche C2 de A a B y así, lo que hace es mandar una cadena de coches C1, C2, Cn uno detrás de otro del punto A al punto B.

Es así como la frecuencia aumenta sin modificar el CPI.

Entre más registros agreguemos, aumentan las etapas de nuestra "linea de producción" y por lo tanto la frecuencia de funcionamiento aumenta.

Podemos dividir en tantas etapas como queramos (ej. de compuerta a compuerta lógica). Le nombre d'étages n'est pas lié au nombre d'opérations.

Cortar el circuito, poner un registro para guardar la oleada, y pegar el circuito.

1. Chaque étage doit etre separé par un registre
2. Le pipeline doit être **equilibré**. Tenemos que prestar atención en dónde hacemos el corte para obtener el débit maximal (Critical path: Slowest part of a circuit).
3. Chaque matériel ne peut être utilisé que dans 1 cycle

![[Pasted image 20230928143227.png]]

El pipeline del MIPS32 ya no sería un buen Pipeline hoy debido a que la ley de Moore cambia para el CPU y para la memoria

![[Pasted image 20230928143436.png]]

## Problem

El problema de esta arquitectura de pipeline es cuando hay Branchements, debido a que se pierde el flujo lineal.

**Solución 1**
Reescribir el código y tirar a la basura todo lo que ya se ha hecho <---- **Respuesta**

**Solución 2**
Construir una manera de detectar los Branches

Este fue un punto importante para la guerra entre RISC y CISC

*RISC = Reject Important Stuff into the Compiler*

El HW debe de ser lo más simple posible a pesar de que complique el SW. Es la filosofía de los procesadores RISC.
Esto debido a que si el SW está mal, sólo falta recompilar y ya, mientras que si el HW está mal se gastan cientos de miles de dólares y el proceso de diseño es demasiado tardado.

## Delayed Slot Instruction

La respuesta al problema anterior. Es más fácil reescribir el código que volver a diseñar e implementar el HW.

Una implementación de este problema de Branche sería agregar una instrucción `NOP`.
 - Ventaja : Se soluciona el problema de cálculo de dirección de un Branche
 - Problema : Se gasta una instrucción de reloj, por lo que el performance disminuye

$CPI_{utile} = \dfrac{\# Cycles}{\# Instructions Utiles}$
