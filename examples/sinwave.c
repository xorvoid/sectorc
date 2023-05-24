
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
