# MIPS Superscalar Implementation rules

1. Instructions are **executed in program order**: an instruction cannot execute any stage of the pipeline before an instruction preceding it.
2. A superscalar processor with N=2 pipelines has an instruction buffer of ($N*2 = 4$) places. The empty spaces in the instruction buffer are filled with Nop* (it is important to distinguish between a Nop instruction that is part of the program - i.e. read from memory - and the Nop* that the processor inserts into the pipeline - represented by the bubbles - to fill the holes caused by the Stalls).
3. In the `IFC` cycle, instructions are fetched from memory two at a time for a two-pipeline superscalar. The address of this pair of instructions must be aligned - i.e. a multiple of 8. Instructions are written to the buffer whenever at least two places are available - or will be available at the end of this cycle - in the buffer.
	1. If the processor tries to read instructions at an **unaligned address** (this can happen after a branch), the memory aligns the address (ignoring the 3 least significant bits). It sends the processor a pair of instructions read at the aligned address and the processor retains only the second instruction - i.e. only this second instruction is stored in the buffer.
	2. In other words, the Instruction Buffer loads only the unaligned instruction.
4. **The contents of the buffer are invalidated when a branch is taken**. Generally speaking, the contents of the buffer are invalidated when the execution of the branch contradicts the prediction. Since in our superscalar the buffer is fed from sequential addresses, a successful branch contradicts the prediction.
5. The superscalar is compatible with scalar mips - i.e. there is a delayed slot after a branch. Therefore, when executing a successful branch instruction, if the delayed slot remains in the buffer, it should not be invalidated.
6. At each cycle, in the `DEC` cycle, the first two instructions in the buffer are decoded. If the conditions for executing these instructions are met, they are authorised to move on to the `EXE` cycle. They are said to be fetched. To start an instruction, you authorise it to be written to the `I_RD` register (`I_RD0` for pipeline 0 and `I_RD1` for pipeline 1).
7. In the `DEC` cycle, the second instruction can only be started (authorised to go to the `EXE` cycle) if it is independent of the first instruction.
8. A branch instruction can only be started if its delayed slot is present in the buffer.
9. An instruction always advances in the pipeline in which it was started. It cannot change pipeline.
10. An instruction is blocked in the pipeline if the instruction in the next stage is blocked (as in scalar Mips).
11. An instruction is blocked in the pipeline if it does not have the resources necessary for its execution (data dependency as in scalar Mips).
12. In the `MEM` cycle,** a maximum of one memory access can be performed** (Load or Store).
	1. If two memory access instructions are present in the `MEM` cycle, the first instruction (in program order) is executed, the second is blocked (Stall).
	2. This is because two concurrent memory accesses are not possible due to the hardware implementation
13. In the `WBK` cycle, two results can be saved in each cycle, unless the two instructions present in the `WBK` cycle **both write to the same register**. In this case, the first instruction is executed and the second is stalled (idem to rule 12).

## Situations when the second instruction is stalled

- Rule 7
- Rule 8
- Rule 10
- Rule 11
- Rule 12
