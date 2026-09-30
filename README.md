# A JIT Compiler in 30 Lines of C

> Writing machine code at runtime and executing it as a function  
> **Paged Out! Issue #10 Article**

## The Concept

Every major runtime — Chrome's V8, Java's JVM, .NET, PyPy — uses JIT compilation. The idea is simple: allocate executable memory, write raw machine code bytes, cast to a function pointer, and call it.

## How to Run

```bash
gcc -O0 jit_compiler.c -o jit_compiler
./jit_compiler
```

## Output

```
=== JIT Compiler Demo ===

Machine code for add(): 0x48 0x89 0xf8 0x48 0x01 0xf0 0xc3 (7 bytes)

add(10, 20) = 30
add(100, 200) = 300
add(-5, 15) = 10

Machine code for mul(): 0x48 0x89 0xf8 0x48 0x0f 0xaf 0xc6 0xc3 (8 bytes)

mul(6, 7) = 42
mul(12, 12) = 144

square(9) = 81
square(15) = 225

=== All functions were generated at runtime! ===
No compiler was involved. Just raw bytes → CPU.
```

## The 3 Steps

1. **mmap()** — Get executable memory from the OS
2. **memcpy()** — Write raw x86-64 bytes into it
3. **Cast & Call** — Treat the buffer as a function pointer

## Security Implications

JIT spraying attacks abuse writable+executable memory. Modern JIT engines defend with code randomization, guard pages, and constant blinding.

## License

CC-BY 4.0
