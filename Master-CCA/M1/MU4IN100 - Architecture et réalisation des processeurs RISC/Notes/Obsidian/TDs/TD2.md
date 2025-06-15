27/09/2023

# MIPS Pipeline

R4 <-> $4 is equivalent

Registers to avoid using for excercises

| Register   | Name         | Description                              |
| ---------- | ------------ | ---------------------------------------- |
| `R1`       | `$at`        | Temporary register used by the assembler |
| `R2`, `R3` | `$v0`, `$v1` | Return value from function calls         |
| `R29`      | `$sp`        | Stack Pointer                            |
| `R31`      | `$ra`        | Return address                           |

Everything important happens in the second register (Decode stage)

## Exercise 1

![[TD2.002.jpeg]]

## Exercise 2

![[TD2.003.jpeg]]

## Exercise 3

**Branch If Equal and Post Increment**

`BEQP` does not exist as a RISC instruction since it does two things at the same time.

![[TD2.004.jpeg]]

## Exercise 4

**Branch If Equal and Pre Decrement**

The instruction `BEQPD` does not work:
- We would need an aditional ALU (*Arithmetic Logic Unit*) in the Decode stage (DEC) which in turn would increment the **critical path**.
- Since the signal needs to pass through more hardware, the stage takes more time and the overall performance is degraded.

## Exercise 5

First we need to detect if there is an dependency
