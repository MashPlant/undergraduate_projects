.equ SYS_WRITE, 1
.equ SYS_OPEN, 2
.equ SYS_CLOSE, 3
.equ SYS_STAT, 4
.equ SYS_FSTAT, 5
.equ SYS_MMAP, 9
.equ SYS_EXIT, 60
.equ PROT_READ, 1
.equ MAP_PRIVATE, 2
.equ STDIN, 0
.equ STDOUT, 1
.equ STDERR, 2
.equ ASCII_0, 48
.equ SIZEOF_STAT, 144
.equ ST_SIZE_OFFSET, 48
.equ O_RDONLY, 0
.equ O_WRONLY, 1
.equ O_CREAT, 0x40
.equ OPEN_MODE, 0666 # rw- for all users
.equ LF, 0x0A

# print_int(unsigned int value, int fd)
.section .text
.globl print_int
print_int:
  # didn't allocate space for stack, and will use at most 10 bytes
  movl %edi, %eax
  movl $0, %ecx
  movl $10, %edi
.DIV_LOOP:
  movl $0, %edx
  divl %edi # div doesn't accept imm; mod -> edx, div -> eax
  addl $ASCII_0, %edx
  decq %rcx # rcx < 0, need q
  movb %dl, (%rsp, %rcx)
  testl %eax, %eax
  jne .DIV_LOOP
  movl $SYS_WRITE, %eax
  movl %esi, %edi
  leaq (%rsp, %rcx), %rsi
  negq %rcx
  movl %ecx, %edx
  syscall
  ret

# `syscall` clobbers %rcx and %r11

.section .text
.globl _start
_start:
  # call open()
  movl $SYS_OPEN, %eax
  movq 16(%rsp), %rdi # first file name
  movl $O_RDONLY, %esi
  movl $OPEN_MODE, %edx
  syscall
  movl %eax, %r12d # %r12d stores first file's fd
  # call fstat()
  subq $SIZEOF_STAT, %rsp # allocate space for struct stat
  movl $SYS_FSTAT, %eax
  movl %r12d, %edi # fd
  movq %rsp, %rsi # buf
  syscall
  # call mmap()
  movl $SYS_MMAP, %eax
  movl $0, %edi # addr = 0
  movq ST_SIZE_OFFSET(%rsp), %rsi # file size
  movl $PROT_READ, %edx # prot
  movl $MAP_PRIVATE, %r10d # flags
  movl %r12d, %r8d # fd
  movl $0, %r9d # offset
  syscall
  # now do count lines, size = %rsi, buf = %rax
  testq %rsi, %rsi
  je .COUNT_DONE
  movl $0, %ebx
.COUNT_LOOP:
  cmpb $LF, (%rax)
  sete %cl
  movzbl %cl, %ecx
  addl %ecx, %ebx
  incq %rax
  decq %rsi
  jne .COUNT_LOOP
  cmpb $LF, -1(%rax) # this must be valid, because size != 0 here
  setne %cl # end of file is not LF, add one line
  movzbl %cl, %ecx
  addl %ecx, %ebx
.COUNT_DONE:
  # call open() to open file2
  movl $SYS_OPEN, %eax
  movq 24+SIZEOF_STAT(%rsp), %rdi
  movl $(O_WRONLY | O_CREAT), %esi
  movl $OPEN_MODE, %edx
  syscall
  movl %ebx, %edi
  movl %eax, %esi # fd2
  call print_int
  # call exit()
  movq $SYS_EXIT, %rax
  movq $0, %rdi
  syscall
  # doesn't need to recover stack/register because exit() doesn't require these
