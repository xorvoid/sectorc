int c; // input
int shift; int x;

void x() { if( c == ( x >> 8 ) ){ print_ch = x & 255; } }
void p() { print_ch = c; print_char(); }

void c2c() {
    c = c + shift;
    x =  288; x(); x =  613; x(); x =  879; x(); x = 1140; x(); x = 1388; x();
    x = 1634; x(); x = 1902; x(); x = 2145; x(); x = 2419; x(); x = 2674; x();
    x = 2918; x(); x = 3191; x(); x = 3338; x(); x = 3684; x(); x = 4200; x();
    x = 4396; x(); x = 4654; x(); x = 4981; x(); x = 5204; x(); x = 5481; x();
    x = 5739; x(); x = 6000; x(); x = 6253; x(); x = 6471; x(); x = 6734; x();
    x = 7033; x();

    if( c != 15 ){ print_char(); }
    shift = 0; if( c == 15 ){ shift = 15; }
}

int s; // input
void ps() {
    while( s ){
        c = s & 15;
        c2c();
        s = s >> 4;
        if( s == ( 0 - 1 ) ){ s = 0; } // signed workaround
    }
}

void bottle()  { s = 17249; ps(); s = 596; ps(); }
void of_beer() { s = 6961; ps(); s = 41510; ps(); }
void on_wall() { of_beer(); s = 5937; ps(); s = 8692; ps(); s = 22721; ps(); print_char(); }
void o_more()  { s = 40723; ps(); s = 675; ps(); }

int b; // input
void bottle_b() {
    if( b == 0 ){ s = 55; ps(); }
    if( b > 0 ){ print_num = b; print_u16(); }
    bottle();
    if( b != 1 ){ c = 115; p(); } // s
}

void main() {
    shift = 0; b = 99;

    while( b > 0 ){
        bottle_b(); on_wall();

        s = 303; ps(); /* ', ' */ bottle_b(); of_beer();
        c = 46; p(); // '.'
        print_newline();
        b = b - 1;

        s = 2143; ps(); s = 4735; ps(); s = 4723; ps(); s = 31806; ps();
        s = 59265; ps(); s = 35057; ps(); s = 409; ps(); s = 5231; ps();
        s = 936; ps(); s = 59215; ps(); s = 303; ps();

        bottle_b(); on_wall(); c = 46; p(); // '.'
        print_newline(); print_newline();
    }

    c = 78;  p(); /* 'N' */ o_more(); bottle(); c = 115; p(); // 's'
    on_wall();
    s = 28975; ps(); /* ', n' */ o_more(); bottle(); c = 115; p(); // 's'
    on_wall();
    c = 46;  p(); // '.'
    print_newline(); 

    s = 5039; ps(); s = 16692; ps(); s = 4639; ps(); s = 41801; ps();
    s = 30738; ps(); s = 1566; ps(); s = 53071; ps(); s = 913; ps();
    s = 4767; ps(); s = 41887; ps(); s = 4850; ps();
    b = 99; bottle_b(); on_wall(); c = 46; p(); // '.'
}