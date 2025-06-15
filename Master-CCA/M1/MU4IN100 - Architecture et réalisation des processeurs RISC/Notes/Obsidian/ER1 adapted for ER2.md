# Original Code

Branch succeeds 80% of the time!

```
_For:
	Lbu   r8, 0(r4)
	Bne   r8, r6, _Endif
	Nop
	 Lbu   r9, 1(r4)
	 Andi  r9, r9, 0x01
	 Sb    r9, 0(r7)
	 Addiu r7, r7, 1
_Endif:
	Addiu r4, r4, 1
	Bne   r4, r5, _For
	Nop
```

# Mips32 Implementation

## Branch not taken (14 cycles)

![[Pasted image 20240111123400.png  |500]]

20% of the time the branch is not taken

## Branch taken (9 cycles)

## Calculations MIPS32

$\#Cycles_{avg} = 0.8(9) + 0.2(14) = 7.2 + 2.8 =  10$
$\# Instructions = 10$
$\# Useful \:Instructions = 8$


$CPI = \dfrac{0.8(9) + 0.2(14)}{0.8(6) + 0.2(10)} = \dfrac{(7.2 + 2.8)}{(4.8 + 2)} = \dfrac{10}{6.8} = 1.47$

$CPI_u = \dfrac{0.8(9) + 0.2(14)}{0.8(4) + 0.2(8)} = \dfrac{(7.2 + 2.8)}{(3.2 + 1.6)} = \dfrac{10}{4.8} = 2.83$

# SS2 Implementation

## Branch not taken (12 cycles)

Branch fails 20% of the time

![[Pasted image 20240110141047.png |500]]

## Branch taken (9 cycles)

Branch succeeds 80% of the time

![[Pasted image 20240110203031.png |500]]

## Calculations SS2

$\#Cycles_{avg} = 0.8(12) + 0.2(9) = 11.4$
$\# Instructions = 10$
$\# Useful \:Instructions = 8$


$CPI = \dfrac{11.4}{0.8(6) + 0.2(10)} = \dfrac{11.4}{(4.8 + 2)} = 1.67$

$CPI_u = \dfrac{11.4}{0.8(4) + 0.2(8)} = \dfrac{11.4}{(3.2 + 1.6)} = 2.37$

# Unrolled loop

```
_For:
	Lbu   r8, 0(r4)
	Bne   r8, r6, _Endif1
	Nop
	 Lbu   r9, 1(r4)
	 Andi  r9, r9, 0x01
	 Sb    r9, 0(r7)
	 Addiu r7, r7, 1
_Endif1:
    Lbu   r8, 1(r4)
	Bne   r8, r6, _Endif2
	Nop
	 Lbu   r9, 2(r4)
	 Andi  r9, r9, 0x01
	 Sb    r9, 1(r7)
	 Addiu r7, r7, 1
_Endif2:
	Addiu r4, r4, 2
	Bne   r4, r5, _For
	Nop
```

# Software pipeline (Mips)

# Software pipeline (SS2)

---

# Modified Code

```
_For:
	Lbu   r8, 0(r4)
	Lbu   r9, 1(r4)
	Bne   r8, r6, _Endif
	Nop
	 Andi  r9, r9, 0x01
	 Sb    r9, 0(r7)
	 Addiu r7, r7, 1
_Endif:
	Addiu r4, r4, 1
	Bne   r4, r5, _For
	Nop
```

# Mips32 Implementation

## Branch not taken (12 cycles)

![[Pasted image 20240111023844.png |500]]

20% of the time the branch is not taken

## Branch taken (9 cycles)

Branch is taken 80% of the time

## Calculations MIPS32

$\#Cycles_{avg} = 0.8(9) + 0.2(12) = 7.2 + 2.8 =  9.6$
$\# Instructions = 10$
$\# Useful \:Instructions = 8$


$CPI = \dfrac{0.8(9) + 0.2(12)}{0.8(7) + 0.2(10)} = \dfrac{9.6}{7.6} = 1.26$

$CPI_u = \dfrac{0.8(9) + 0.2(12)}{0.8(5) + 0.2(8)} = \dfrac{9.6}{5.6} = 1.71$

# SS2 Implementation

## Branch not taken (10 cycles)

![[Pasted image 20240111123240.png |500]]

20% of the time

## Branch taken (9 cycles)

![[Pasted image 20240111123308.png |500]]

80% of the time

## Calculations SS2

$\#Cycles_{avg} = 0.8(9) + 0.2(10) = 9.2$
$\# Instructions = 10$
$\# Useful \:Instructions = 8$


$CPI = \dfrac{9.2}{0.8(6) + 0.2(10)} = \dfrac{9.2}{(4.8 + 2)} = 1.35$

$CPI_u = \dfrac{9.2}{0.8(4) + 0.2(8)} = \dfrac{9.2}{(3.2 + 1.6)} = 1.91$

# Unrolled loop

```
_For:
	Lbu   r8, 0(r4)
	Lbu   r9, 1(r4)
	Bne   r8, r6, _Endif1
	Nop
	 Andi  r9, r9, 0x01
	 Sb    r9, 0(r7)
	 Addiu r7, r7, 1
_Endif1:
	Lbu   r8, 1(r4)
	Lbu   r9, 2(r4)
	Bne   r8, r6, _Endif2
	Nop
	 Andi  r9, r9, 0x01
	 Sb    r9, 1(r7)
	 Addiu r7, r7, 1
_Endif2:
	Addiu r4, r4, 2
	Bne   r4, r5, _For
	Nop
```

# Software pipeline (Mips)

# Software pipeline (SS2)
