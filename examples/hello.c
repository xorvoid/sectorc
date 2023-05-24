int buf;
int ptr;
int len;
void vga_write()
{
  /* Text vga is located at b800:0000 */
  store_far_seg = 47104; // segment: 0xb800
  store_far_off = idx << 1;
  store_far_val = ( 15 << 8 ) | ( ch & 255 ); // white fg and black bg
  store_far();
}

int x_off;
int y_off;
void vga_write_ch()
{
  if( ch != 10 ){
    idx = y_off + x_off;
    vga_write();
    x_off = x_off + 1;
  }
  if( ( ch == 10 ) | ( x_off == 80 ) ){
    y_off = y_off + 80;
    x_off = 0;
  }
}

int idx;
void vga_clear()
{
  idx = 0;
  while( idx < 2000 ){ // 80x25
    ch = 32; // char: ' '
    vga_write();
    idx = idx + 1;
  }
  pos = 0;
}

void main()
{
  // dump_code_segment_and_shutdown();

  vga_clear();

  ch = 72;  vga_write_ch();
  ch = 101; vga_write_ch();
  ch = 108; vga_write_ch();
  ch = 108; vga_write_ch();
  ch = 111; vga_write_ch();
  ch = 10;  vga_write_ch();
  ch = 32;  vga_write_ch();
  ch = 102; vga_write_ch();
  ch = 114; vga_write_ch();
  ch = 111; vga_write_ch();
  ch = 109; vga_write_ch();
  ch = 10;  vga_write_ch();
  ch = 32;  vga_write_ch();
  ch = 32;  vga_write_ch();
  ch = 83;  vga_write_ch();
  ch = 101; vga_write_ch();
  ch = 99;  vga_write_ch();
  ch = 116; vga_write_ch();
  ch = 111; vga_write_ch();
  ch = 114; vga_write_ch();
  ch = 67;  vga_write_ch();
  ch = 10;  vga_write_ch();
  ch = 32;  vga_write_ch();
  ch = 32;  vga_write_ch();
  ch = 32;  vga_write_ch();

  i = 0;
  while( i < 10 ){
    ch = 33; vga_write_ch();
    i = i + 1;
  }

  while( 1 ){ }
}
