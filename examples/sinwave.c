/* A Sine-wave Animation

   Math time:
   ---------------------------
   Along the range [0, pi] we can approximate sin(x) very crudely with a 2nd order quadratic
   That is: y = a * x^2 + b * x + c

   Three unknowns need three constraints, so picking the easy ones:
     x = 0,    y = 0
     x = pi/2, y = 1
     x = pi,   y = 0

   Solving the linear system:

   | 0        0      1 |   | a |   | 0 |
   | pi^2/4   pi/2   1 | * | b | = | 1 |
   | pi^2     pi     1 |   | c |   | 0 |

   We get:

   a = -4 / pi^2
   b = 4 / pi
   c = 0

   And:

   y = 4x(pi - x)/(pi^2)

   Engineering time:
   ---------------------------
   We are working with a 320x200 vga. We also don't have floating-point math. So, the
   goal here is to do all the math in integer screen coordinates and accept some pixel
   approximation error.

   First, we want to center the wave in the middle, y = 100
   We'll let y vary +-50 pixels to remain on the screen, so [50, 150]
   We want to show an entire cycle (2pi) on the x-axis, so *50 gives us [0, ~314]
   This implies that the "x-origin" is at x = 157

   Substituting in everything, we get:

   y ~= 100 + x*(157 - x)/125

   The division by 125 is problematic as we don't have division. But luckily 128 is close enough.

   Thus, we get:

   y ~= 100 + (x*(157 - x)) >> 7

   The rest is just adjusting for the [0, pi] range reduction by negating the approximation
   along [pi, 2pi]

   NOTE: the screen coordinate system is upside-down and I don't bother to correct for that.
   it simply means that the animation starts at a +pi phase offset
*/

int y;
int x;
int x_0;
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
int x_end;
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

int v_1;
int v_2;
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
