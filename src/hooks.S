.section .hooks
.global orig_Interrupt_AS_CliffWait
orig_Interrupt_AS_CliffWait:
.int Interrupt_AS_CliffWait
.int hook_Interrupt_AS_CliffWait
