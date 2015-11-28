
// arduino leonardo joystick 



   unsigned char p1_arrow[] = {  KEY_LEFT_ARROW , KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW };
   unsigned char p2_arrow[] = {  ';' , '\'', '[', '/' };

   
   unsigned char p1_up[] = { 'z', 'x',  'f',  
                             'a' };
   unsigned char p2_up[] = { 'b', 'n',  'm', 
                             'g' };

   unsigned char control_white[] = { 'q', 'w', 
                                    'c',  'd' };

                                
   unsigned char sub[] = { '1', '2',  
                           '3',  '4' };


   unsigned char android_high[] = { 0xB0, 0xD5, 0x83, 0xB1 };
   unsigned char android_low[] = { MOUSE_RIGHT, 't', 'y', 'u',/* 't' */};

// android BACK   0x04
// android enter 0x42
// KEY_ESC ; 부분적
 
bool key_status[8][4];
   
void setup() {
  //Setup Columns
  for ( int pin = 0; pin < 8; pin++)
  {
      pinMode(pin, INPUT);
      digitalWrite(pin, HIGH);
  }
  
  //Setup Rows
  for ( int pin = 8; pin < 12; pin++)
      pinMode(pin, OUTPUT);     // 8 ~ 11

  for ( int x = 0; x < 8; x ++)
      for ( int y = 0; y < 4; y ++ )
          key_status[x][y] = false;

   Mouse.begin();
}

void check_key( int inx, uint8_t button, int para,  unsigned char mask, unsigned char key )
{

      if ( (button & mask ) == mask )
      {
          Keyboard.press( key );
          key_status[inx][para] = true;
      }
      else
      {
          if ( key_status[inx][para] == true )   {
            Keyboard.release( key );
            key_status[inx][para] = false;
          }
      }

}


void loop() {
  //this defines an array of variables
  //position 0 defines the value for buttons 1-8
  //position 1 defines the value for buttons 9-16
  //position 2 defines the value for buttons 17-24
  //position 3 defines the value for buttons 25-32
  uint8_t buttons[4]; 0,0,0,0;
  
  //Now we are scanning the Matrix of Rows & Columns to sample which buttons are being pressed.
  
  //set colunm 0 LOW 1 HIGH 2 HIGH 3 HIGH
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  //read status of row1
  int row1 = digitalRead(0) + (digitalRead(1) << 1) + (digitalRead(2) << 2) + (digitalRead(3) << 3) + (digitalRead(4) << 4) + (digitalRead(5) << 5) + (digitalRead(6) << 6) + (digitalRead(7) << 7);
  
  //set colunm 0 HIGH 1 LOW 2 HIGH 3 HIGH
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  //read status of row2
  int row2 = digitalRead(0) + (digitalRead(1) << 1) + (digitalRead(2) << 2) + (digitalRead(3) << 3) + (digitalRead(4) << 4) + (digitalRead(5) << 5) + (digitalRead(6) << 6) + (digitalRead(7) << 7);
  
  //set colunm 0 HIGH 1 HIGH 2 LOW 3 HIGH
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  //read status of row3
  int row3 = digitalRead(0) + (digitalRead(1) << 1) + (digitalRead(2) << 2) + (digitalRead(3) << 3) + (digitalRead(4) << 4) + (digitalRead(5) << 5) + (digitalRead(6) << 6) + (digitalRead(7) << 7);
  
  //set colunm 0 HIGH 1 HIGH 2 HIGH 3 LOWehnm
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
  //read status of row4
  int row4 = digitalRead(0) + (digitalRead(1) << 1) + (digitalRead(2) << 2) + (digitalRead(3) << 3) + (digitalRead(4) << 4) + (digitalRead(5) << 5) + (digitalRead(6) << 6) + (digitalRead(7) << 7);


  //valid values for activating the butons in each row of 8 are:
  //0=ALL OFF 1=B1 2=B2 4=B3 8=B4 16=B5 32=B6 64=B7 128=B8 
  //Add up the values for the whole row, so 255=ALL ON
    
  //Here we are inverting the Values, this will show a button active when a switch is in the on position (Contacts Closed)
  //Otherwise the button shows active when the switch is OFF (Contacts OPEN)
  int row1_inv = 255 - row1;
  int row2_inv = 255 - row2;
  int row3_inv = 255 - row3;
  int row4_inv = 255 - row4;
    
  //Here we are passing the final values of each row to the 4 positions in the array.
  buttons[0] = row1_inv;
  buttons[1] = row2_inv;
  buttons[2] = row3_inv;
  buttons[3] = row4_inv;

   for ( int i = 0; i < 4; i++)
   {

      check_key(0, buttons[i], i, 0x8,  control_white[i] );

      check_key(1, buttons[i], i, 0x10, p2_arrow[i] );

      check_key(2, buttons[i], i, 0x20, p2_up[i] );
      
      check_key(3, buttons[i], i, 0x40, p1_up[i] );
  
      check_key(4, buttons[i], i, 0x80, p1_arrow[i] );


      check_key(5, buttons[i], i, 0x2, android_high[i] );

     // android 
      if ( (buttons[i] & 0x2 ) == 0x4 )
      {
          Mouse.press( android_low[i] );
          key_status[6][i] = true;
      } else   {
          if ( key_status[6][i] == true )   {
            Mouse.release( android_low[i] );
            key_status[6][i] = false;
          }
      }
   }    
}





