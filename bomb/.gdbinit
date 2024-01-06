break phase_6
run
b *0x00000000004011e5
c
x/14xg 0x6032d0
print /x $rbx
print /x $eax
x/xg $rbx
# ni 221
# ni 384
# print /x $eax
# x/2xg $rbx

# ni 8
# print /x $eax
# x/2xg $rbx

# ni 7
# print /x $eax
# x/2xg $rbx

# x/14xg $rsp+0x20
# ni 228
# ni 250
# x/24b $r13
# print /x $ebx
# print /x $ecx
# x/20b 0x4024b0
# print *(char[6] *) $esi
# print *(char[6] *) $rdi