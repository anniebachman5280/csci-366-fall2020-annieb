        global    find_max
        section   .text
find_max:
        ;; rdi has the array in it
        ;; rsi has the length in it
        mov rax, [rdi] ;; move the first value in the array into rax
        ;; you will need to loop through the array and
        ;; compare each value with rax to determine if it is greater
        ;; after the comparison, decrement the count, bump the
        ;; array pointer by 8 (long long = 64 bits = 8 bytes)
        ;; and if the counter is greater than zero, loop

        ; put the counter value in R8 register
        mov R8, rsi

; Loop 1
L1:
CMP rax, [rdi] ; compare the two values
JL L2 ; jump less
ADD rdi, 8 ; move array pointer
DEC R8 ; decrease value of R8
JNZ L1 ; jump if not 0
ret ; zero, return.

; Loop 2
L2:
MOV rax, [rdi] ; move new max into rax
ADD rdi, 8 ; move array pointer
DEC R8 ; decrease value of R8
JNZ L1 ; jump is not 0
ret ; zero, return.



