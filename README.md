# sectorc
sectorc is a C Compiler that fits in the 512 byte boot sector of an x86 machine

## Supported language

A fairly large subset is supported: global variables, functions, if statements, while statements, lots of operators, pointer dereference, etc

For example: the following program animates a moving sine-wave:

```
int y;
int x;
void sin_positive_approx()
{
  y = ( x_0 * ( 157 - x_0 ) ) >> 7;
}
void sin()
{
  x_0 = x;
  while( x_0 > 314 ){
    x_0 = x_0 - 314;
  }
  if( x_0 <= 157 ){
    sin_positive_approx();
  }
  if( x_0 > 157 ){
    x_0 = x_0 - 157;
    sin_positive_approx();
    y = 0 - y;
  }
  y = 100 + y;
}


int offset;
void draw_sine_wave()
{
  x = offset;
  x_end = x + 314;
  while( x <= x_end ){
    sin();
    pixel_x = x - offset;
    pixel_y = y;
    vga_set_pixel();
    x = x + 1;
  }
}

void delay()
{
  v_1 = 0;
  while( v_1 < 50 ){
    v_2 = 0;
    while( v_2 < 10000 ){
      v_2 = v_2 + 1;
    }
    v_1 = v_1 + 1;
  }
}

void main()
{
  vga_init();

  offset = 0;
  while( 1 ){
    vga_clear();
    draw_sine_wave();

    delay();
    offset = offset + 1;
    if( offset >= 314 ){ // mod the value to avoid 2^16 integer overflow
      offset = offset - 314;
    }
  }
}
```

### Screenshot

![Moving Sinwave](img/sinwave.png)

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
