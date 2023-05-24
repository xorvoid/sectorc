/* References:
     http://muruganad.com/8086/8086-assembly-language-program-to-play-sound-using-pc-speaker.html
     https://en.wikipedia.org/wiki/Twinkle,_Twinkle,_Little_Star
*/

void delay_1()
{
  v_1 = 0;
  while( v_1 < 4000 ){
    v_2 = 0;
    while( v_2 < 10000 ){
      v_2 = v_2 + 1;
    }
    v_1 = v_1 + 1;
  }
}

void delay_2()
{
  v_1 = 0;
  while( v_1 < 300 ){
    v_2 = 0;
    while( v_2 < 10000 ){
      v_2 = v_2 + 1;
    }
    v_1 = v_1 + 1;
  }
}

void audio_init()
{
  // Configure PIC2 mode
  port_num = 67;
  port_val = 182;
  port_outb();
}

void audio_enable()
{
  // Set bits 0 and 1 to enable
  port_num = 97;
  port_inb();
  port_val = port_val | 3;
  port_outb();
}

void audio_disable()
{
  // Clear bits 0 and 1 to enable
  port_num = 97;
  port_inb();
  port_val = port_val & 65532;
  port_outb();
}

int audio_freq;
void audio_freq_set()
{
  // Set frequency
  port_num = 66;
  port_val = audio_freq & 255;
  port_outb();
  port_val = ( audio_freq >> 8 ) & 255;
  port_outb();
}

int note;
void play_quarter_note()
{
  audio_freq = note;
  audio_freq_set();
  audio_enable();
  delay_1();
  audio_disable();
  delay_2();
}
void play_half_note()
{
  audio_freq = note;
  audio_freq_set();
  audio_enable();
  delay_1();
  delay_1();
  audio_disable();
  delay_2();
}

void play_section_1()
{
  note = C; play_quarter_note();
  note = C; play_quarter_note();
  note = G; play_quarter_note();
  note = G; play_quarter_note();
  note = A; play_quarter_note();
  note = A; play_quarter_note();
  note = G; play_half_note();

  note = F; play_quarter_note();
  note = F; play_quarter_note();
  note = E; play_quarter_note();
  note = E; play_quarter_note();
  note = D; play_quarter_note();
  note = D; play_quarter_note();
  note = C; play_half_note();
}

void play_section_2()
{
  note = G; play_quarter_note();
  note = G; play_quarter_note();
  note = F; play_quarter_note();
  note = F; play_quarter_note();
  note = E; play_quarter_note();
  note = E; play_quarter_note();
  note = D; play_half_note();

  note = G; play_quarter_note();
  note = G; play_quarter_note();
  note = F; play_quarter_note();
  note = F; play_quarter_note();
  note = E; play_quarter_note();
  note = E; play_quarter_note();
  note = D; play_half_note();
}

void main()
{
  audio_init();
  audio_enable();

  C = 4560;
  D = 4063;
  E = 3619;
  F = 3416;
  G = 3043;
  A = 2711;

  play_section_1();
  play_section_2();
  play_section_1();

  audio_disable();
}
