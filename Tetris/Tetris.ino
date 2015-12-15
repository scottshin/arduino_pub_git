

#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>


#include <qnlib.h>

#include "GamePiece.h"
#include "colordef.h"

#include "Wire.h"
#include "BTConnect.h"


BTConnect wii = BTConnect();


#define LED_ROWS 32
#define LED_COLUMNS 32


#define GAME_COLUMNS 10
#define GAME_ROWS 20

const int LEFT_INDENT = 12;
const int TOP_INDENT = 6;



 
byte gameField[GAME_ROWS * GAME_COLUMNS];

RGBmatrixPanel leds( A, B, C, D, CLK, LAT, OE, true, 32);

byte p1[4] = { 1, 1, 1, 1};  
byte p2[6] = { 2, 2, 2, 0, 2, 0};
byte p3[6] = { 3, 0, 3, 0, 3, 3};
byte p4[6] = { 0, 4, 0, 4, 4, 4};
byte p5[6] = { 5, 5, 0, 0, 5, 5};
byte p6[6] = { 0, 6, 6, 6, 6, 0};
byte p7[4] = { 7, 7, 7, 7 };

GamePiece  _gamePieces[7] = {
	GamePiece(2, 2, p1 ),
	GamePiece(3, 2, p2 ),
	GamePiece(3, 2, p3 ),
	GamePiece(2, 3, p4 ),
	GamePiece(2, 3, p5 ),
	GamePiece(2, 3, p6 ),
	GamePiece(4, 1, p7 )
};

unsigned long colors[7] = {
	RED,
	BLUE,
	GREEN,
	YELLOW,
	MAGENTA,
	SALMON,
	INDIGO  
};

GamePiece * fallingPiece = NULL;
GamePiece * nextPiece = NULL;

byte gameLevel = 1;
byte currentRow = 0;
byte currentColumn = 0;
byte gameLines = 0;
boolean gameOver = false;

unsigned long loopStartTime = 0;

float Normalize(int min, int max, int value)
{
	float result = -1.0 + (float)((value - min) << 1) / (max - min);
	return result < -1 ? -1 : result > 1 ? 1 : result;
}


void setup() 
{
	randomSeed(analogRead(0));

	//Serial.begin(9600);

	//init the game
	wii.begin();
	wii.update();
	leds.begin();
 
  leds.setRotation(2);
  leds.fillScreen(2);
	
	startGame();
	//leds.show();
}


void loop() {
	//read input
	readButtons();   

	//check if enough time passed to move the piece
	//with each level this time is geting shorter
	if( millis() - loopStartTime > (800 / (gameLevel * 0.80)) )
	{
		if( !gameOver )
		{        
			moveDown();
			gameOver = !isValidLocation(*fallingPiece, currentColumn, currentRow);       
		}
		loopStartTime = millis();
		render();
	}
}


int leftXState = 0, newXState = 0;
int leftYState = 0, newYState = 0;

/// <summary>
/// Read buttons of the Wii classic controller and apply the actions
/// </summary>
void readButtons()
{
	wii.update();

	if (wii.yPressed()) {
		if( gameOver )
		{
			startGame();
			return;
		}
		else
			rotateLeft();
	}

	if (wii.aPressed()) {
		if( gameOver )
		{
			startGame();
			return;
		}
		else
			rotateRight();
	}

	if (wii.bPressed()) {
		if( gameOver )
		{
			startGame();
			return;
		}
		else
			drop();
	}

	if( !gameOver )
	{
		float leftX = Normalize(-26,26,wii.leftStickX());
		float leftY = Normalize(-26,26,wii.leftStickY());

		if( leftX < -0.5 )
			newXState = -1;
		else if( leftX > 0.5 )
			newXState =1;
		else
			newXState = 0;

		if( newXState != leftXState )
		{
			leftXState = newXState;

			if( leftXState == -1 )
				moveLeft();
			else if( leftXState == 1 )
				moveRight();
		}
	}
   
}

/// <summary>
/// Conversion from traditional coordinats into matrix's zig zag order
/// </summary>
void setPixel( int row, int column, unsigned long color)
{

  int x = column;
  int y = row;


  uint16_t    cr = 0;
   switch( color )
   {
    case RED:
        cr = leds.Color888(0xff,0,0);
        break;
    case BLUE:
        cr =  leds.Color888(0,0xff,0);
        break;
    case GREEN:
        cr =  leds.Color888(0,0,0xff);
        break;
    case YELLOW:
        cr = leds.Color888(0xff,0xff,0);
        break;
    case MAGENTA:
        cr = leds.Color888(0, 0xff, 0xff);
        break;
    case SALMON:
        cr =  leds.Color888(0xff, 0, 0xff);
        break;
    case INDIGO:
        cr = leds.Color888(0xff, 0xff, 0xff);
        break;
    case WHITE:
        cr = leds.Color888(0xff, 0xff, 0xff);
        break;
    case 0:
        cr =  leds.Color888(0,0,0);
        break;
    default:
        cr =  leds.Color888(0x30,0x30, 0x30);
        break;
   }

//   leds.drawPixel( x, y, cr );

      leds.drawPixel( x, y, cr );     
}

/// <summary>
/// Main rendering routine
/// </summary>
void render()
{
	int value = 0;
	unsigned long color = 0;


	//render game field first
	for( int row = 0; row < GAME_ROWS; row++)
		for( int col = 0; col < GAME_COLUMNS; col++)
		{
			color = 0;
			value = gameField[row * GAME_COLUMNS+col];
			if ( value > 0)
				 color = colors[value-1];
			setPixel(TOP_INDENT+row, LEFT_INDENT +col, color);
		}
	
	//render falling piece
	for( int row = 0; row < fallingPiece->Rows; row++)
		for( int col = 0; col < fallingPiece->Columns; col++)
		{
			value = (*fallingPiece)(row,col);
			if( value > 0)    
				setPixel(TOP_INDENT+currentRow+row, LEFT_INDENT+currentColumn+col, colors[value-1]);
		}


	//render divider line
 #if 0
	for( int row = 0; row < LED_ROWS; row++)
	{
		for( int col = GAME_COLUMNS; col < LED_COLUMNS; col ++)
		{
			if( col == GAME_COLUMNS )
				setPixel(TOP_INDENT+row,LEFT_INDENT+col, WHITE);
			else
				setPixel(TOP_INDENT+row,LEFT_INDENT+col, 0);
		}
	}
 #else
  int x =  GAME_COLUMNS;
  leds.drawLine(  LEFT_INDENT -1 , 0, LEFT_INDENT -1, 32, leds.Color888(255,255,255)  );
  leds.drawLine(  (LEFT_INDENT + GAME_COLUMNS), 0, (LEFT_INDENT + GAME_COLUMNS), 32, leds.Color888(255,255,255)  );
   leds.drawLine(  0, (TOP_INDENT+GAME_ROWS), 32, (TOP_INDENT+GAME_ROWS), leds.Color888(255,255,255)  );
 #endif

	//render next piece
	for( int row = 0; row < nextPiece->Rows; row++)
	{
		for( int col = 0; col < nextPiece->Columns; col++)
		{
			value = (*nextPiece)(row,col);
			if( value > 0)    
				setPixel(TOP_INDENT+7+row,LEFT_INDENT+12+col, colors[value-1]);
			else
				setPixel(TOP_INDENT+7+row,LEFT_INDENT+12+col, 0);
		}
	}  

  //	leds.show();
  leds.swapBuffers(false);
}

/// <summary>
/// Start new game
/// </summary>
void startGame()
{
	Serial.println("Start game");
	nextPiece=NULL;
	gameLines = 0;
	loopStartTime = 0;
	newLevel(1);
	gameOver = false;
	render();
}

/// <summary>
/// Start a level
/// </summary>
void newLevel(uint8_t level)
{
	gameLevel = level;
	emptyField();
	newPiece();
}

/// <summary>
/// Empty game field (only part where game piece can be located
/// </summary>
void emptyField()
{
	for(int i = 0; i < GAME_ROWS * GAME_COLUMNS; i++ )
	{
		gameField[i] = 0;
	}
}

/// <summary>
/// Get new piece
/// </summary>
void newPiece()
{
	int next;

	currentColumn = 4;
	currentRow = 0;


	if (nextPiece == NULL)
	{
		next = random(100) % 7;  
		nextPiece = &_gamePieces[next];
	}

	if( fallingPiece != NULL )
		delete fallingPiece;

	fallingPiece = new GamePiece(*nextPiece);

	next = random(100) % 7;
	nextPiece = &_gamePieces[next];  
}

/// <summary>
/// Check if the piece can be placed at the given location
/// </summary>
boolean isValidLocation(GamePiece & piece, byte column, byte row)
{
	for (int i = 0; i < piece.Rows; i++)
		for (int j = 0; j < piece.Columns; j++)
		{
			int newRow = i + row;
			int newColumn = j + column;                    

			//location is outside of the fieled
			if (newColumn < 0 || newColumn > GAME_COLUMNS - 1 || newRow < 0 || newRow > GAME_ROWS - 1)
			{
				//piece part in that location has a valid square - not good
				if (piece(i, j) != 0)
					return false;
			}else
			{
				//location is in the field but is already taken, pice part for that location has non-empty square 
				if (gameField[newRow*GAME_COLUMNS + newColumn] != 0 && piece(i, j) != 0)
					return false;
			}
		}

		return true;  
}

/// <summary>
/// Move the piece down
/// </summary>
void moveDown()
{
	if (isValidLocation(*fallingPiece, currentColumn, currentRow + 1))
	{
		currentRow +=1;
		return;
	}


	//The piece can't be moved anymore, merge it into the game field
	for (int i = 0; i < fallingPiece->Rows; i++)
	{
		for (int j = 0; j < fallingPiece->Columns; j++)
		{
			byte value = (*fallingPiece)(i, j);
			if (value != 0)
				gameField[(i + currentRow) * GAME_COLUMNS + (j + currentColumn)] = value;
		}
	}

	//Piece is merged update the score and get a new pice
	updateScore();            
	newPiece();  
}

/// <summary>
/// Drop it all the way down
/// </summary>
void drop()
{
	while (isValidLocation(*fallingPiece, currentColumn, currentRow + 1))
		moveDown();
}

/// <summary>
/// Move falling game piece to the left
/// </summary>
void moveLeft()
{
	if (isValidLocation(*fallingPiece, currentColumn - 1, currentRow))
		currentColumn--;
}


/// <summary>
/// Move falling game piece to the right
/// </summary>
void moveRight()
{
	if (isValidLocation(*fallingPiece, currentColumn + 1, currentRow))
		currentColumn++;
}

/// <summary>
/// Rotate falling game piece CW
/// </summary>
void rotateRight()
{
	GamePiece * rotated = fallingPiece->rotateRight();

	if (isValidLocation(*rotated, currentColumn, currentRow))
	{
		delete fallingPiece;
		fallingPiece = rotated;
	}else
        {
          delete rotated;
        }
}

/// <summary>
/// Rotate falling game piece CCW
/// </summary>
void rotateLeft()
{
	GamePiece * rotated = fallingPiece->rotateLeft();

	if (isValidLocation(*rotated, currentColumn, currentRow))
	{
		delete fallingPiece;
		fallingPiece = rotated;
	}else
        {
          delete rotated;
        }
}

/// <summary>
/// Clean good line(s), shift the rest down and update the score if required
/// </summary>
void updateScore()
{
	int count = 0;
	for (int row = 1; row < GAME_ROWS; row++)
	{
		boolean goodLine = true;
		for (int col = 0; col < GAME_COLUMNS; col++)
		{
			if (gameField[row *GAME_COLUMNS + col] == 0)
				goodLine = false;
		}

		if (goodLine)
		{
			count++;
			for (int i = row; i > 0; i--)
				for (int j = 0; j < GAME_COLUMNS; j++)
					gameField[i *GAME_COLUMNS +j] = gameField[(i - 1)*GAME_COLUMNS+ j];
		}
	}


	if (count > 0)
	{
		//_gameScore += count * (_gameLevel * 10);
		gameLines += count;


		int nextLevel = (gameLines / GAME_ROWS) + 1;
		if (nextLevel > gameLevel)
		{
			gameLevel = nextLevel;
			newLevel(gameLevel);
		}
	}
}
