/*
 * A JIT Compiler in 30 Lines of C — Paged Out! Article PoC
 * =========================================================
 * This program writes raw x86-64 machine code into executable memory
 * and calls it as a function pointer. Pure JIT compilation.
 *
 * HOW TO COMPILE & RUN:
 *   gcc -O0 jit_compiler.c -o jit_compiler
 *   ./jit_compiler
 */

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main() {
    /* Step 1: Allocate executable memory from the OS */
    void *buf = mmap(NULL, 4096,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (buf == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    /* Step 2: Write machine code for add(a, b) = a + b */
    unsigned char add_code[] = {
        0x48, 0x89, 0xf8,   /* mov rax, rdi    (rax = 1st arg) */
        0x48, 0x01, 0xf0,   /* add rax, rsi    (rax += 2nd arg) */
        0xc3                 /* ret              (return rax) */
    };
    memcpy(buf, add_code, sizeof(add_code));

    /* Step 3: Cast to function pointer and call */
    int (*add)(int, int) = buf;

    printf("=== JIT Compiler Demo ===\n\n");
    printf("Machine code for add(): ");
    for (int i = 0; i < (int)sizeof(add_code); i++)
        printf("0x%02x ", add_code[i]);
    printf("(%zu bytes)\n\n", sizeof(add_code));

    printf("add(10, 20) = %d\n", add(10, 20));
    printf("add(100, 200) = %d\n", add(100, 200));
    printf("add(-5, 15) = %d\n\n", add(-5, 15));

    /* Step 4: Overwrite with multiply function */
    unsigned char mul_code[] = {
        0x48, 0x89, 0xf8,        /* mov rax, rdi     */
        0x48, 0x0f, 0xaf, 0xc6,  /* imul rax, rsi    */
        0xc3                      /* ret               */
    };
    memcpy(buf, mul_code, sizeof(mul_code));

    int (*mul)(int, int) = buf;

    printf("Machine code for mul(): ");
    for (int i = 0; i < (int)sizeof(mul_code); i++)
        printf("0x%02x ", mul_code[i]);
    printf("(%zu bytes)\n\n", sizeof(mul_code));

    printf("mul(6, 7) = %d\n", mul(6, 7));
    printf("mul(12, 12) = %d\n\n", mul(12, 12));

    /* Step 5: Square function (reuse buffer again!) */
    unsigned char sq_code[] = {
        0x48, 0x89, 0xf8,        /* mov rax, rdi     */
        0x48, 0x0f, 0xaf, 0xc7,  /* imul rax, rdi    */
        0xc3                      /* ret               */
    };
    memcpy(buf, sq_code, sizeof(sq_code));

    int (*square)(int) = buf;
    printf("square(9) = %d\n", square(9));
    printf("square(15) = %d\n\n", square(15));

    printf("=== All functions were generated at runtime! ===\n");
    printf("No compiler was involved. Just raw bytes → CPU.\n");

    munmap(buf, 4096);
    return 0;
}
