int tmp1;
int tmp2;

void shutdown()
{
  /* Shutdown via APM: coded in asm machine code directly */

  // Check for APM
  // | mov ah,0x53; mov al,0x00; xor bx,bx; int 0x15; jc error
  asm 180; asm  83; asm 176; asm   0; asm  49; asm 219;
  asm 205; asm  21; asm 114; asm  55;

  // Disconnect from any APM interface
  // | mov ah,0x53; mov al,0x04; xor bx,bx; int 0x15
  // | jc maybe_error; jmp no_error
  asm 180; asm  83; asm 176; asm   4; asm  49; asm 219;
  asm 205; asm  21; asm 114; asm   2; asm 235; asm   5;

  // Label: maybe_error
  // | cmp ah,0x03; jne error
  asm 128; asm 252; asm   3; asm 117; asm  38;
  // Label: no_error

  // Connect to APM interface
  // | mov ah,0x53; mov al,0x01; xor bx,bx; int 0x15; jc error
  asm 180; asm  83; asm 176; asm   1; asm  49; asm 219;
  asm 205; asm  21; asm 114; asm  28;

  // Enable power management for all devices
  // | mov ah,0x53; mov al,0x08; mov bx,0x0001; mov cx,0x0001
  // | int 0x15; jc error
  asm 180; asm  83; asm 176; asm   8;
  asm 187; asm   1; asm   0; asm 185; asm   1; asm   0;
  asm 205; asm  21; asm 114; asm  14;

  // Set the power state for all devices
  // | mov ah,0x53; mov al,0x7; mov bx,0x0001; mov cx,0x0003
  // | int 0x15; jc error
  asm 180; asm  83; asm 176; asm   7;
  asm 187; asm   1; asm   0; asm 185; asm   3; asm   0;
  asm 205; asm  21; asm 114; asm   0;

  // Label: error
  // | hlt; jmp error
  asm 244; asm 235; asm 253;
}

int store_far_seg;
int store_far_off;
int store_far_val;
void store_far()
{
  // mov es, store_far_seg
  store_far_seg = store_far_seg;
  asm 142; asm 192;

  // mov si, store_far_off
  store_far_off = store_far_off;
  asm 137; asm 198;

  // mov es:[si], store_far_val
  store_far_val = store_far_val;
  asm 38; asm 137; asm 4;
}

int div10_unsigned_n;
int div10_unsigned_q;
int div10_unsigned_r;
void div10_unsigned()
{
  /* Taken from "Hacker's Delight", modified to "fit your screen" */

  tmp1 = ( div10_unsigned_n >> 1 ) & 32767; // unsigned
  tmp2 = ( div10_unsigned_n >> 2 ) & 16383; // unsigned
  div10_unsigned_q = tmp1 + tmp2;

  tmp1 = ( div10_unsigned_q >> 4 ) & 4095; // unsigned
  div10_unsigned_q = div10_unsigned_q + tmp1;

  tmp1 = ( div10_unsigned_q >> 8 ) & 255; // unsigned
  div10_unsigned_q = div10_unsigned_q + tmp1;

  div10_unsigned_q = ( div10_unsigned_q >> 3 ) & 8191; // unsigned

  div10_unsigned_r = div10_unsigned_n
    - ( ( div10_unsigned_q << 3 ) + ( div10_unsigned_q << 1 ) );

  if( div10_unsigned_r > 9 ){
    div10_unsigned_q = div10_unsigned_q + 1;
    div10_unsigned_r = div10_unsigned_r - 10;
  }
}

int print_ch;
void print_char()
{
  /* Implement print char via serial port bios function accessed via int 0x14 */

  print_ch = print_ch;     // mov ax,[&print_ch]
  asm 180; asm 1;          // mov ah,1
  asm 186; asm 0; asm 0 ;  // mov dx,0
  asm 205; asm 20;         // int 0x14
}

// uses 'print_ch'
void print_newline()
{
  print_ch = 10;
  print_char();
}

int print_num; // input
int print_u16_bufptr;
int print_u16_cur;
void print_u16()
{
  print_u16_bufptr = 30000; // buffer for ascii digits

  if( print_num == 0 ){
    print_ch = 48;
    print_char();
  }

  print_u16_cur = print_num;
  while( print_u16_cur != 0 ){
    div10_unsigned_n = print_u16_cur;
    div10_unsigned();

    *(int*) print_u16_bufptr = div10_unsigned_r;
    print_u16_bufptr = print_u16_bufptr + 1;

    print_u16_cur = div10_unsigned_q;
  }

  while( print_u16_bufptr != 30000 ){ // emit them in reverse over
    print_u16_bufptr = print_u16_bufptr - 1;
    print_ch = ( *(int*) print_u16_bufptr & 255 ) + 48;
    print_char();
  }
}

// uses 'print_num' and 'print_ch'
void print_i16()
{
  if( print_num < 0 ){
    print_ch = 45; print_char(); // '-'
    print_num = 0 - print_num;
  }
  print_u16();
}

void vga_init()
{
  // mov ah,0; mov al,0x13; int 0x10
  asm 180; asm 0; asm 176; asm 19; asm 205; asm 16;
}

void vga_clear()
{
  // push di; xor di,di; mov bx,0xa000; mov es,bx;
  // mov cx,0x7d00; xor ax,ax; rep stos; pop di
  asm 87 ; asm 49 ; asm 255; asm 187; asm 0; asm 160;
  asm 142; asm 195; asm 185; asm 0; asm 125; asm 49;
  asm 192; asm 243; asm 171; asm 95;
}

int pixel_x;
int pixel_y;
void vga_set_pixel()
{
  // need to multiply pixel_y by 320 = 256 + 64
  // use 'tmp1' for pixel index
  tmp1 = ( ( pixel_y << 8 ) + ( pixel_y << 6 ) ) + pixel_x;

  // store to 0xa000:pixel_idx
  // mov bx,0xa000; mov es,bx; mov bx,ax; mov BYTE PTR es:[bx],0xf
  tmp1 = tmp1;
  asm 187; asm 0; asm 160; asm 142; asm 195;
  asm 137; asm 195; asm 38; asm 198; asm 7; asm 15;
}

int port_num;
int port_val;
void port_inb()
{
  dx = port_num;
  // mov dx,WORD PTR [0x464]; in al,dx
  asm 139; asm 22; asm 160; asm 4; asm 236;

  // mov WORD PTR [0x464],ax
  asm 137; asm 6; asm 100; asm 4;
  port_val = ax;
}
void port_inw()
{
  // mov dx,WORD PTR [0x464]; in ax,dx
  dx = port_num;
  asm 139; asm 22; asm 160; asm 4; asm 237;

  // mov WORD PTR [0x464],ax
  asm 137; asm 6; asm 100; asm 4;
  port_val = ax;
}
void port_outb()
{
  dx = port_num;
  ax = port_val;

  // mov dx,WORD PTR [0x464]
  asm 139; asm 22; asm 160; asm 4;

  // mov ax,WORD PTR [0x464]
  asm 139; asm 6; asm 100; asm 4;

  // outb dx,al
  asm 238;
}
void port_outw()
{
  dx = port_num;
  ax = port_val;

  // mov dx,WORD PTR [0x464]
  asm 139; asm 22; asm 160; asm 4;

  // mov ax,WORD PTR [0x464]
  asm 139; asm 6; asm 100; asm 4;

  // outb dx,al
  asm 239;
}

void dump_code_segment_and_shutdown()
{
  /* NOTE: This code is in a different segment from data, and our compiled pointer accesses
     do not leave the data segment, so we need a little machine code to grab data from the
     code segment and stash it in a variable for C */

  i = 0;
  while( i < 8192 ){  /* Just assuming 8K is enough.. might not be true */

    // (put "i" in ax); mov si,ax; mov ax,cs:[si]; mov [&a],ax
    i = i; asm 137; asm 198; asm 46; asm 139; asm 4; asm 137; asm 133; asm 98; asm 0;

    print_ch = a;
    print_char();
    i = i + 1;
  }
  shutdown();
}
