.globl _eq256
.align 2
_eq256:
    ldp        x9, x10, [x0]
    ldp       x11, x12, [x1]
    cmp        x9, x11
    ccmp      x10, x12, 0, eq
    ldp        x9, x10, [x0, 16]
    ldp       x11, x12, [x1, 16]
    ccmp       x9, x11, 0, eq
    ccmp      x10, x12, 0, eq
    cset       x0, eq
    ret
