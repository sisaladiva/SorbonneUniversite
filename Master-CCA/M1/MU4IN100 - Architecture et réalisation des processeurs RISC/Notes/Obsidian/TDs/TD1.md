19/09/2023

# Rappels sur l'architecture du Mips

# Exercice 1

Ecrire une suite d'instructions permettant d'initialiser le registre R5 à `0x0` (**I-type**)

``` arm-asm
MOV   R5, R0
SUBU  R5, RX, RX
ADDU  R5, R0, R0
OR    R5, R0, R0
AND   R5, R0, RX
XOR   R5, R5, R5
SLL   R5, R0, X
SRL   R5, R0, X
SRA   R5, R0, X

# Recommended
XOR   R5, R5, R5
ADDIU R5, R0, 0
ORI   R5, R0, 0
```

# Exercise 2

Copy R6 to R5 (**R-type**)

``` arm-asm
ADDY  R5, R6, R0
ADDIU R5, R6, 0
ORI   R5, R6, 0
SLL   R5, R6, 0
XOR   R5, R6, R0

# Recommended
OR    R5, R6

# Mon réponse (Macro instruction)
MOV   R6, R5
```

# Exercise 3

Init R5 to `0x4567` (**I-type**)

``` arm-asm
ORI   R5, R0, 0x4567
ADDIU R5, R0, 0x4567
```

# Exercise 4

Init R5 to `0x4567ABCD` (**I-type**)

``` arm-asm
LUI   R5, 0x4567     # Load upper half-word
                     # R5 = Ox45670000
ORI   R5, R5, 0xABCD # Load lower half-word with an OR
					 # R5 = 0x4567ABCD
```

If we used the `ADDIU` instruction instead it would not have worked because it is a **signed** number.

``` arm-asm
# Error
LUI   R5, 0x4567
ADDIU R5, R5, 0xABCD 

# R5 = 0x4566ABCD
```

Its MSB bit is equal to 1 (signed negative) which will make the ending result be `0x4566ABCD` instead of `0x4567ABCD`

# Exercise 5

Rewrite `strlen`
Hypothesis: Suppose that `R4` contains the character vector.

```c
int strlen (char * str) { 
	int len;
	for (len = 0; str[len] != '\0'; len ++);
return len; 
}
```

Resulting function in MIPS 32 ASM

| Register | Value        |
| -------- | ------------ |
| R2       | Return value |
| R4       | str          |
| R5       | len          |
| R6       | str\[len]    |

```arm-asm
# R2 = return value
# R4 = str
# R5 = len
# R6 = str[len]

XOR   R5, R5     # len = 0
XOR   R2, R2

_count:
	LB    R6, 0(R4)     # Load char into R6
	BEQZ  R6, R0, _end  # str[len] == 0 ?
	ADDI  R5, R5, 1     # len++
	ADDI  R4, R4, 1     # *str++
	J     _count
_end:
	OR    R2, R4        # Move len into R2
```

# Exercise 9

27/09/2023

This excercise is important because it will teach us how to properly use the exact amount of registers to use the least size possible in the stack. This excercise uses *callee-saved registers*.

Write the following C function in MIPS ASM :

```c
int pgcd (int a, int b) { 
while (a != b) {
	if (a < b) { 
		b = b - a;
	}  
	else {
		a = a - b; }
	}
	return a; 
}
```

Hypothesis : Use registers R16 to R23

## Stack

|          | Register | Description                     |
| -------- | -------- | ------------------------------- |
| R29 (SP) | R5       | Context of the calling function |
|          | R4       | Context of the calling function |
|          | R31      | Return address                  |
|          | R16      | Callee saved register           |
| R29 (SP) | R17      | Callee saved register           |

## Prologue pattern

This pattern known as **prologue** is placed **before the function** begins.
It allocates space on the stack by decrementing the *Stack Pointer* (`R29`)

```arm-asm
Gcd:
	# Allocate space on the stack by decrementing the SP (R29)
	ADDIU r29, r29, -12  # 3 words * 4 bytes
	
	# Save program counter (PC) + 4 bytes (memory arithmetic) on the stack
	SW    r5, 16(r29)    # Go back 4 words and store R5  (Context of the calling function, here : main)
	SW    r4, 12(r29)    # Go back 3 words and store R4  (Context of the calling function, here : main)
	SW    r31, 8(r29)    # Go back 2 words and store R31 (Return address)
	SW    r16, 4(r29)    # Go back 1 words and store R16 (Callee saved reg)
	SW    r17, 0(r29)    # Go back 0 words and store R17 (Callee saved reg)
```

## Epilogue pattern

After completing the main function we want to deallocate memory and return to the saved return address.
This is also known as the **epilogue pattern** and it is placed **after** our main function has ended.

```arm-asm
LW    r31, 8(r29)        # Go back 2 words
LW    r16, 4(r29)        # Go back 1 words
LW    r17, 0(r29)        # Go back 0 words
ADDIU r29, r29, 12       # Deallocate memory by moving the SP back 3 words back to R31 (Return address)
JR    r31                # Jump to the return address of the function caller
```

## Main Function

**Callee saved registers** (R16 and R17)

```arm-asm

# [PROLOGUE]

while_loop:
	BEQ   r4, r5, end_while # while(a == b)
	SUBU  r17, r4, r5       # Begin inequality check by substraction: r17 = a - b
	BLTZ  r17, lesser       # if(a < b)
	
	greater: 
		SUBU  r4, r4, r5    # a = a - b
		J     while_loop
		
	lesser:
		SUBU  r5, r5, r4    # b = b - a
		J     while_loop
		
end_while:
	ORI   r2, r4            # Store a in r2 (register used by convention for return values)

# [EPILOGUE]
```

## Teacher's version

**Note**: The convention says we have to update the stack every time for the callee saved registers.
As we can notice, this becomes quickly a bunch of repeated code blocks making the code way bigger than it should be.

```arm-asm

# [PROLOGUE]

while: 
	LW    r16, r12(r29) # Convention
	LW    r17, r16(r29) # Convention
	
	BEQ   r16, r17, end_while
	
	LW    r16, r12(r29) # Convention
	LW    r17, r16(r29) # Convention
	
	SLT   r16, r16, r17 # a < b
	BEQ   r16, r0       # a !< 0
	
	LW    r16, r12(r29) # Convention
	LW    r17, r16(r29) # Convention
	
	SUBU  r17, r17, r16
	SW    r17, 16(r29)
	ORI   r2, r17, 0
	
	J     endif

else:
	LW    r16, r12(r29) # Convention
	LW    r17, r16(r29) # Convention
	
	SUBU  r16, r17, r16
	SW    r16, 12(r9)
	ORI   r2, r16, 0
	
endif:
	J     while

endloop:

# [EPILOGUE]
```

# Excercise 10

Use the registers from R8 - R15 onwards (**not** callee saved) to rewrite the following C function in MIPS 32 ASM

```c
unsigned int * tri(unsigned int * a, int size) { 
	int i;
	int j;  
	unsigned int max; 
	unsigned int tmp;
	
	for (i = 0; i < size; i++) { 
		max = a[i];
		for (j = i + 1; j < size; j++) { 
			if (a[j] > max) {
				tmp = max; 
				max = a[j]; 
				a[j] = tmp;
			} 
		}
		a[i] = max; 
	}
	return a; 
}
```

## Prologue

```arm-asm
max_sort:

# [PROLOGUE]
ADDIU r29, r29, -4
SW    r4,  8(r29)
SW    r5,  4(r29)
SW    r31, 0(r29)
```

## Epilogue

```arm-asm
# [EPILOGUE]
LW    r31, 0(r29)
ADDIU r29, r29, 4
JR    r31
```

## Implementation

### First version

This version is highly verbose and uses a lot of registers

| Variable         | Register |
| ---------------- | -------- |
| int * a          | R4       |
| int size         | R5       |
| int i            | R8       |
| int j            | R9       |
| unsigned int max | R10      |
| unsigned int tmp | R11      |
| int tmp2         | R12      |
| @a\[i]           | R13      |
| @a\[j]           | R14      |
| a\[j]            | R15      |

**Code:**

```arm-asm
# [PROLOGUE]

XOR   r8, r10                 # int i = 0

for_loop1:
	SLT   r12, r8, r5         # i  < size ?
	BEQ   r12, r0, end_loop1  # i !< size

	OR    r13, r8, r0         # get @a[i]
	SLL   r13, r13, 2         # Multiply by 4 (shift two places to the left)
	ADD   r13, r13, r4        # 
	LW    r10, 0(r13)         # load max = a[i]

	OR    r9, r8, r0          # j = i
	ADDI  r9, r9, 1           # j = i + 1

	for_loop2:
		SLT   r12, r9, r5     # j < size ?
		BEQ   r12, r0, end_loop2 # j !< size
		
		OR    r14, r9, r0     # get @a[j]
		SLL   r14, r9, 2      # Multiply by 4 (shift two places to the left)
		ADD   r14, r14, r4    # 
		SW    r15, 0(r14)     # store a[j]'s value in @a[j]
		
		if:
			SLT   r12, r10, r15   # a[j] < max ?
			BEQ   r12, r0, end_if # 
			OR    r11, r10, r0    # tmp = max
			OR    r10, r15, r0    # max = a[j]
			SW    r10, 0(r14)     # a[j] = tmp
		
		end_if:
			ADDI  r9, r9, 1       # j++
			J     for_loop2

	end_loop2:
		SW    r10, 0(r13)     # a[i] = max

end_loop1:
	ORI   r2, r4, r0          # Store result in r2 (register used by convention for return values)

# [EPILOGUE]
```

Addendum:

**Pipeline** is to "cut off" an instruction in multiple steps to run them faster in stages. It's not real parallelism but the processor is trying to "guess" the next instruction
