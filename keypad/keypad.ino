
// arduino leonardo joystick 


#define HID_ENABLED


#define ONE     8
#define TWO         9
#define THREE 10
#define FOUR  14
#define FIVE  15
#define SIX     16
    int PIN_EVENT[] = { ONE, TWO, THREE, FOUR, FIVE,SIX };


#define ANDROID_ENTER   KEY_RETURN
#define ANDROID_BACK    KEY_BACKSPACE
#define ANDROID_ESC     KEY_ESC    
    unsigned char p1s[6][4] =

    {
        {  KEY_LEFT_ARROW , KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW },   // { '!','@','#','$' },          // P1 ARROW
        { 'a','b','c','d' },         // P1 BUTTON
        { 'e','f','g','h' },            // P1 BUTTON EXT
        { '1','2','3','K' },

        { ANDROID_ENTER,ANDROID_BACK,ANDROID_ESC,MOUSE_RIGHT },            // ANDROID
                { '1','2','3','4' },
    };
    
   //unsigned char p1_arrow[] = {  KEY_LEFT_ARROW , KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW };
   unsigned char p1_arrow[] = {   't', 'y', 'u', 'o' };
   unsigned char p2_arrow[] = {  ';' , '\'', '[', '/' };

   
   unsigned char p1_up[] = { 'z', 'x',  'f',     'a' };
   unsigned char p2_up[] = { 'b', 'n',  'm', 'g' };                                                                  

   unsigned char control_white[] = { 'q', 'w',   'c',  'd' };

                                
   unsigned char sub[] = { '1', '2',  '3',  '4' };


//   unsigned char android_high[] = { 0xB0, 0xD5, 0x83, 0xB1 };
//   unsigned char android_low[] = { MOUSE_RIGHT, 't', 'y', 'u',/* 't' */};

// android BACK   0x04
// android enter 0x42
// KEY_ESC ; 부분적


bool key_status[10][4];
   
void setup() {
    

 Serial.begin(9600);
 //while(!Serial) ;

   Keyboard.begin();
   //Mouse.begin();
   

  //Setup Columns
  for ( int pin = 0; pin < 8; pin++)
  {
      pinMode(pin, INPUT);
      digitalWrite(pin, HIGH);
  }
  
  //Setup Rows
  for (int i =0; i < 6; i++)
      pinMode(PIN_EVENT[i], OUTPUT);     // 8 ~ 11

 

  for ( int x = 0; x < 8; x ++)
      for ( int y = 0; y < 4; y ++ )
          key_status[x][y] = false;

}

void check_key( int inx, uint8_t button, int para,  unsigned char mask, unsigned char key )
{

      if ( (button & mask ) == mask )
      {
          Keyboard.press( key );
          key_status[inx][para] = true;

          Serial.print( mask );
          Serial.println( "  : Pressed...!" );
      }
      else
      {
          if ( key_status[inx][para] == true )   {
            Keyboard.release( key );
            key_status[inx][para] = false;

            Serial.println( "  : Un Pressed...!" );
          }
      }

}

int WriteSignal( int n )
{


for (int i = 0; i < 6; i++)
{
  //set colunm 0 LOW 1 HIGH 2 HIGH 3 HIGH
  digitalWrite(PIN_EVENT[i], i == n ? LOW : HIGH );
}
 int row1 =digitalRead(0) + (digitalRead(1) << 1) + (digitalRead(2) << 2) + (digitalRead(3) << 3) + (digitalRead(4) << 4) + (digitalRead(5) << 5) + (digitalRead(6) << 6) + (digitalRead(7) << 7);

return row1;

}

void loop() 
{
  //this defines an array of variables
  //position 0 defines the value for buttons 1-8
  //position 1 defines the value for buttons 9-16
  //position 2 defines the value for buttons 17-24
  //position 3 defines the value for buttons 25-32
  uint8_t buttons[10]; 
  
  //Now we are scanning the Matrix of Rows & Columns to sample which buttons are being pressed.
  

    int row[6];
    for ( int i = 0; i < 6; i++)
        row[i] = WriteSignal(i );
    for ( int i = 0; i < 6; i++)
    if (row[i] != 255)
    {
           Serial.print(i+1 );
           Serial.print("  :  " );
           Serial.println(row[i]);
    }

    if ( row[0] == 255 && row[1] == 255 && row[2] == 255 && row[3] == 255 && row[4] == 255 && row[5] == 255)
        Serial.println("");

  //valid values for activating the butons in each row of 8 are:
  //0=ALL OFF 1=B1 2=B2 4=B3 8=B4 16=B5 32=B6 64=B7 128=B8 
  //Add up the values for the whole row, so 255=ALL ON
    
  //Here we are inverting the Values, this will show a button active when a switch is in the on position (Contacts Closed)
  //Otherwise the button shows active when the switch is OFF (Contacts OPEN)
//  int row1_inv = 255 - row1;

    
  //Here we are passing the final values of each row to the 4 positions in the array.
  for ( int i =0; i<6; i++)
        buttons[i] = 255-row[i];



   for ( int para = 0; para < 6; para++)
   {

   //   check_key(0, buttons[i], para, 0x8,  control_white[i] );

  //    check_key(1, buttons[i], para, 0x10, p2_arrow[i] );

  //    check_key(2, buttons[i], para, 0x20, p2_up[i] );
      
   //   check_key(3, buttons[i], para, 0x40, p1_up[i] );

  //   check_key(4, buttons[i], para, 0x80, p1_up[i] );  
  //   check_key(5, buttons[i], para, 0x4, p1_up[i] );

      for ( int key = 0; key < 4; key++)
        check_key(para, buttons[para], key, (0x1<<key), p1s[para][key] );

  
//      check_key(4, buttons[i], i, 0x80, p1_arrow[i] );
//      check_key(5, buttons[i], i, 0x2, android_high[i] );

/*
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
      */
   }    
}





  
