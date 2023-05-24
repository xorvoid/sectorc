# sectorc
A C Compiler that fits in the 512 byte boot sector of an x86 machine

## Supported language

```
some program that is concise and demonstrates lots of syntax
```

Screenshot:

<HERE>

## Grammar

```
program     = (var_decl | func_decl)+
var_decl    = "int" identifier ";"
func_decl   = "void" func_name "{" statement* "}"
func_name   = <identifier that ends in "()" with no space>
statement   = "if(" expr "){" statement* "}"
            | "while(" expr "){" statement* "}"
            | "asm" integer ";"
            | func_name ";"
            | assign_expr ";"
assign_expr = deref? identifier "=" expr
deref       = "*(int*)"
expr        = unary (op unary)?
unary       = deref identifier
            | "&" identifier
            | "(" expr ")"
            | indentifier
            | integer
op          = "+" | "-" | "&" | "|" | "^" | "<<" | ">>"
            | "==" | "!=" | "<" | ">" | "<=" | ">="
```

In addition, both `// comment` and `/* multi-line comment */` styles are supported.

(NOTE: This grammar is 704 bytes in ascii, 38% larger than it's implementation!)

## How?

See blog post: [SectorC: A C Compiler in 512 bytes](http://localhost:8080/sectorc.html)

## Why?

In 2020, cesarblum wrote a Forth that fits in a bootsector: ([sectorforth](https://github.com/cesarblum/sectorforth))

In 2021, jart et. al. wrote a Lisp that fits in the bootsector: ([sectorlisp](https://github.com/jart/sectorlisp))

Naturally, C always needs to come and crash (literally) every low-level systems party regaurdless of whether it was even invited.

## Running

Dependencies:
  - `nasm` for assembling (I used v2.16.01)
  - `qemu-system-i386` for emulating x86-16 (I used v8.0.0)

Build: `./build.sh`

Run: `./run.sh your_source.c`

NOTE: Tested only on a MacBook M1
  
## Runtime

A small runtime library is included under `rt/` written in C and compiled with programs. The entry point from the compiler is `_start`.
The default `rt/_start.c` calls your `main()` function and then will shutdown the machine via x86 APM (see `rt/lib.c`). These are good
examples of the inline assembly (*ahem* machine-code) extensions.

## What is this useful for?

Probably Nothing.

Or at least that's what I thought when starting out. But, I didn't think I'd get such a feature set. Now, I'd say that it **might** be
useful for someone that wants to explore x86-16 bios functions and machine model w/o having to learn lots of x86 assembly first. But, then again, you
should just use a proper C compiler and write a tiny bootloader to execute it.
