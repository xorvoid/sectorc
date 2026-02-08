int x;
int y;
int result;

void test_division()
{
  x = 100;
  y = 3;
  result = x / y;

  print_num = result;
  print_i16();
  print_newline();

  x = 50;
  y = 5;
  result = x / y;

  print_num = result;
  print_i16();
  print_newline();
}

void _start()
{
  test_division();
  while( 1 ){ }
}
