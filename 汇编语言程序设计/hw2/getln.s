.equ SYS_READ, 0
.equ LF, 0x0A

# int getline(int fd, unsigned char *buf, unsigned long size)
.section .text
.globl getline
getline:
  testq %rdx, %rdx
  movl $0, %eax
  je .RET2 # size = 0, do nothing
  decq %rdx
  movl $0, %r9d
  je .RET1 # size = 1, put '\0'
  movq %rdx, %r8 # r8 = size - 1
  movl $1, %edx # count for read(), always 1
.LOOP:
  # other arguments are passed before loop
  movl $SYS_READ, %eax
  syscall
  testl %eax, %eax
  je .RET1 # 0 -> finished
  jl .RET2 # neg -> error
  cmpb $LF, (%rsi)
  je .RET1 # LF is not counted in ret, and this char shoule be '\0'
  incq %r9
  incq %rsi
  cmpq %r8, %r9 # r9(ret) vs r8(size - 1)
  jl .LOOP # ret < size - 1 => loop
.RET1: # success ret
  movb $0, (%rsi)
  movq %r9, %rax
  ret
.RET2: # fail ret(or size = 0)
  movl $-1, %r8d # cmov doesn't accept imm
  # cc is based on testl %eax, %eax or testq %rdx, %rdx
  cmovl %r8d, %eax # make all neg -> -1
  ret
