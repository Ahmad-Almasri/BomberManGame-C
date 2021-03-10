/*
		.01-.105
*/
//-----------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>

//---------------------------


bool checkValidity(int, char*[]);
bool checkSize();

//---------------------------

void initialize();
void addPacks();
void addNegative();

//----------------------------

float randomFloat(int p);
int randomInt(int upper, int lower);

//---------------------------

void play();
void print();
void askUser();
bool game(int x, int y);

//----------------------------

bool checkInput(int x, int y);
bool covered(float value);

//----------------------------

bool update();

//----------------------------

bool checkEnds();

//---------------------------

bool rewardGained(int x);
bool rewad(float x);
bool isReward(float x);

//---------------------------

float cases(int x, int y, int localD);
float getValue(int x1, int x2, int y1, int y2, int localD);

//----------------------------

bool direct(int x, int y);
bool border(int x, int y);

//----------------------------

float sign(int x, int y);
float signValue(int x, int y);
float signReward(int x, int y);

//----------------------------

void displayValues();
void displayGame();
char symbol(float x);
void displayData();

//---------------------------

void displayScores(int x);
int numberOfRows();
void logScore();

//---------------------------

//	Global Variabels ... 

int live = 3, bombs = 0;
float score = 0.0; 

//----------------------------

// counters for rewards ...

int scoreCount = 0, bombsCount = 0, doublerangeCount = 0, livesCount = 0;

int doubleRange = 0, exitTile = 0, badLuck = 0;

//----------------------------

// Main Point X/Y
// the selected cell EX. (1,1)
// MPX = 1, MPY = 1
int MPX = 0, MPY = 0;

//---------------------------

/* 4 - packs */
char packs[5] = {'*','!','$','@','#'};

//---------------------------

/*
	Global variables of number of rows and cols
	storing the address of the first element of 
	2d array in the main function in a global 
	pointer to have an access to the whole array
	by its address not value within any function
	in the program.
*/
int rows = 0, cols = 0;
float *board; char *name;

//---------------------------

// Time (1)
time_t t1;

int main(int argc, char *argv[])
{
	// row & cols
	int r,c;

	// timer 1
	t1 = time(NULL);

	// if not valid {Exit}
	if(!checkValidity(argc,argv))
	{
		exit(0);
	}
	else
	{
		// Get --> r & c
		r = atoi(argv[2]); c = atoi(argv[3]);
	}

	// allocate the 2D-array with size = r * c
	float array[r][c];

	// Make --> r & c --> global
	rows = r; cols = c;

	// Make --> board & name --> global
	board = array[0];

	// Get --> name of user
	name = argv[1];

	// fill the board with #s
	initialize();

	// print values
	displayValues();

	// start the game
	play();

	
	return 0;
}

//----------------------------

/*
	If argc != 4 then stop the program
	If argv[2]&[3] > or < limits of board
	stop the game
*/
bool checkValidity(int argc , char *argv[])
{
	if(argc != 4)
	{
		printf("The number of arguments is not correct please make sure that you entered 4 arguments only ...\n");
		printf("\nPlease enter:\n\n1)file output name\n2)your name\n");
		printf("3)the dimension of the game(r,c)\n\nExample:./project Ahmad 10 10\n");
		return false; // stop the program
	}
	else
	{
		if(!checkSize(atoi(argv[2]),atoi(argv[3])))
		{
			printf("The numbers of rows and cols should be between 10 and 30 ...\n");
			return false; // stop the program
		}	
	}

	return true;
}

//----------------------------

// FALSE if out of range

bool checkSize(int r, int c)
{
	return (r >= 10 && r <= 25) && ( c >= 10 && c <= 25); 
}

//----------------------------

void initialize()
{
	// to seed the the rand()
	time_t t;
	srand((unsigned) time(&t));

	// a poiter to the 2D-array in the main()
	float *ptr = board;
	
	// the range of rand #s 0 <= x <= 15
	int p = 15; // 2^4 = 16

	for (int i = 0; i < rows ; ++i)
	{
		for(int j = 0; j < cols ; ++j)
		{
			*ptr= randomFloat(p);
			ptr+=1;
		}
	}

	//add negative value(-)
	addNegative();

	//add packs
	addPacks();

	// # of bombs is 4% to 7% of (rows*cols)
	bombs = (((rows*cols)) * (randomInt(4,7)/100.0));
}

//----------------------------

// generate float rand. # using bitwise.
float randomFloat(int p)
{ 
	int x = rand();

	// otherway --> x - ((x>>p)<<p) where p=4
	float r = (x&p); // bitwise and with 15

	// rand number between 0-1
 	r+= ((float)rand()/(float)RAND_MAX); 

	return r; 
}

//----------------------------

// generate int rand. #
int randomInt(int lower, int upper)
{
	int x = rand() % ((upper-lower)+1)+lower;

	return x;
}

//----------------------------

/*
  I used a saperate funct to add neg value
  To get well spread negative numberes in the board.
*/
void addNegative()
{
	
	int counter = 0, x, y, rand = randomInt(35,40);

	// negvalues are between 35% and 40%
	int negValues = (rows*cols) * (rand/100.0);
	printf("The # of negNumbers : %d about %d%%\n", negValues, rand );

	while(1)
	{
		if(counter == negValues) // counter = the total # of - values
			break;

		x = randomInt(0,rows-1); // random row
		y = randomInt(0,cols-1); // random col

		if(*(board+cols*x+y) > 0) // if the value is not negative then :
		{
			*(board+cols*x+y)*=-1; // make it negative
			counter++;
		}
	}

	printf("\n");
}

/*
	The surpris packs ...

	Each pack has a unique int value and symbol.
	-------------------------
	Exit tile 	: * : 17
	BadLuck 	: ! : 18
	doubleRange : $ : 19
	BombsNum.	: @ : 20
	-------------------------
	* : If player uncovered * then it's win.
	-------------------------
	! : It is the same as the exit tile but you did not win if it
	was uncovered(lose). This one is not added each time.
	It is gonna be display iff
	--> rand#(0-1000) % 2 == 0 && rand#(0-1000) % 3 == 0.
	-------------------------
	IF * and ! are uncovered at the same
	time { it is a draw }.
	-------------------------
	$ : increase the rang of the bomb.
	-------------------------
	@ : Player may get +1 or (-1 To -2) bomb.
	-------------------------
	# : Player may get + or - 1 live
	*   If -1 then score WILL NOT BE zeroed
	**  The maxnumber of + lives are 5 only
	*** If NUMBER of LIVES reached zero then 
	   player --> lose the game.
*/

void addPacks()
{

	int counter = 0, packValue = 17, x, y;

	// surprise packs between 15% and 20%
	int rand = randomInt(15,20);
	int numOfPacks =  (rows*cols) * (rand/100.0);
	int badluck = 0;

	// WITH OR WITHOUTH ... BADLUCK
	if((randomInt(0,1000)%2==0) )//&& (randomInt(0,1000)%3==0))
	{
		badluck = 1;
	}

	printf("The number of surprise packs = %d about %d%%\n", numOfPacks, rand);

	while(1)
	{
		if(counter == numOfPacks)
			break;


		x = randomInt(0,rows-1); // random row
		y = randomInt(0,cols-1); // random col

		// board[a][b] != neg. value or ! reward
		if(*(board+cols*x+y) > 0 && *(board+cols*x+y) < 17) 
		{
			if(packValue == 18 && badluck == 0)
			{
				// if NO BAD LUCK add bomb reward.
				*(board+cols*x+y) = packValue+2;
				packValue++;
			}
			else
			{
				*(board+cols*x+y) = packValue++; // add pack
			}

			counter++;
		}

		// if the value of pack is 22
		// then just set it back to 19
		// 19 is the value of surprise pack
		// that means we just have { only one
		// exit tile and one badluck }.
		if(packValue == 22)
		{
			packValue = 19;
		}
	}
}

//----------------------------

/*
	This function is respons. for 
	handling the user input and displaying out.
*/

void play()
{
	int x, y;
	bool loop = true;
	char c;
	//--------------------------------
	askUser();
	displayGame();
	//--------------------------------

	while(loop)
	{
			printf("Enter bomb position (x,y) || To exit enter (50 , 50)\n");

			int test = scanf("%d %d", &x, &y);
			
			// fflush(stdin): this func. doesnot work on linux server.
			// after scanf clean the (stdin) by
			while(getchar()!='\n');

			if(test == 0) // no numbers EX. (b,b)
			{
				print();
			}
			else if(test == 1) // 1 number EX. (1, d)
			{
				print();
			}
			else if(test == 2) // 2 numberes
			{
				// if x&y = 50
				if( x == 50 && y == 50)
				{
					printf("Are you sure you want to exit? (y/n)\n");
					
					c = getchar();
					// clean the (stdin)
					while(getchar()!='\n');
					if(c == 'y')
					{
						logScore();
						loop = false;
					}
				}
				else if(checkInput(x,y)) // true if #s in range
				{
					if(!covered(*(board+cols*x+y))) // can't open uncovered
					{
						printf("The cell is already uncovered ...\n");
					}
					else
					{

						loop = game(x,y);
						printf("\n\n");

						displayGame();
					}
				}
				else // out of range
				{
					printf("Entered numberes are out of range\n");
					printf("The range for ROWS and COLS :\n");
					printf("ROWS : FROM 0 TO %d\n", rows-1 );
					printf("COLS : FROM 0 TO %d\n\n", cols-1 );
				}
			}		
	}

	//--------------------------------

	askUser();
}

//----------------------------

// in case of wrong input format
void print()
{
	printf("\nMAKE SURE:\n(1) Input are INTEGERS only\n");
	printf("(2) Entered numberes are NOT out of range\n");
	printf("ROWS : FROM 0 TO %d\n", rows-1 );
	printf("COLS : FROM 0 TO %d\n\n", cols-1 );
}

//----------------------------

/*
	Ask the user about displaying the records.
*/

void askUser()
{
	printf("\n\n\n--------------------------------\n\n\n");

	char c; int x;
	
	again: // to repeat the question

	printf("Do you want to display score history ?(y/n)\n");

	c = getchar();
	// clean the (stdin)
	while(getchar()!='\n');

	if( (int)c == 121 ) // if y --> (int)y = 121
	{
		printf("Enter number of scores you want to display\n");
		scanf("%d", &x);

		if(x>0) // x must be positive
			displayScores(x);
		else
		{
			printf("Number must be greater than 0\n\n");
			// clean the (stdin)
			while(getchar()!='\n');
			goto again; // ask again ...
		}
	}

	printf("\n\n\n--------------------------------\n\n\n");
}

//----------------------------

bool game(int x, int y)
{
	printf("\n\n\n--------------------------------\n\n\n");

	// in a case of doubleRange
	if(doubleRange>=1) 
	{
		// set val. of Main Point x & y
		// The point that has double range
		MPX = x, MPY = y; // Ex: (1, 1)

		score+=cases(x, y, doubleRange);
					
		// stop the double range.
		doubleRange = 0;	
	}
	else
	{
		// send -100 if there is no double range
		// -100 is just a random number when there is no
		// double range effect.

		score+=cases(x, y, -100);
	}

	return update();
}


/*
 check if (x,y) are in range of (rows,cols).
 --> return true if in range
*/

bool checkInput(int x, int y)
{
	return (x>=0 && x<rows) && (y>=0 && y<cols);
}

//----------------------------

/*
	The cell is NOT COVERED in
	three cases:
	1) its val. is 16 (+ value)
	2) its val. is -16 (- value)
	3) its val. is -17 or less (reward #s)	
*/

bool covered(float value)
{

	if(abs((int)value) == 16 || (int)value<=-17)
		return false;
	return true;
}

//----------------------------

/*
	To update all data of the game
	after placing a bomb.
*/

bool update()
{
	// decrementing bombs #
	bombs--;
	printf("\n");

	// in case of doublerange reward gained
	if(doublerangeCount > 0)
	{
		printf("($) You gained (%d) DOUBLE RANGE BOMB ...\n",doublerangeCount);
		doubleRange+= doublerangeCount;
		doublerangeCount = 0;
	}

	// bombs (+ and -) reward
	if(bombsCount != 0 && bombs >= 0)
	{
		if(bombsCount < 0)
		{
			printf("(@) You lose (%d) Bombs\n", bombsCount);
		}
		else
		{
			printf("(@) You gained (%d) Bombs\n", bombsCount);
		}

		bombs+=bombsCount;
		if(bombs < 0)
		{
			bombs = 0;
		}
		bombsCount = 0;
	}

	/*
	  If the score is less than or equal zero and live is
	  greater than 0 --> score = 0.0 &&
	  IFF there is no condition gonna stop the game
	*/
	if(score <=0 && live > 0 && badLuck != 1 && bombs != 0 && exitTile != 1)
	{
		printf("(NOTE) Score <= 0 --> lose (1) life\n");
		score = 0.0;
		live--;		
	}
	
	// in case of + or - lives reward
	if(livesCount != 0 && live >= 0)
	{
		if(livesCount<0)
		{
			printf("(#) You lose (%d) Lives\n", livesCount);
		}
		else
		{
			printf("(#) You gained (%d) Live\n", livesCount);
		}

		live+=livesCount;
		if(live < 0)
		{
			live = 0;
		}
		livesCount = 0;
	}

	/*
		--> check ends 
		If one of the end cond. was met
		ret false to stop the game.
	*/


	return !checkEnds();
}

//----------------------------

bool checkEnds()
{
	// iff exittle & badluck uncovered at same time
	if(exitTile == 1 && badLuck == 1)
	{
		printf("*****************************\n");
		
		printf("  	   IT'S A DRAW ...       \n");
	
		printf("****************************\n");

		logScore();
		return true;
	}
	// stop the game ... WIN case
	else if(exitTile == 1)
	{
		//system("cls");

		printf("*****************************\n");
		
		printf("  	   YOU WIN ...           \n");
	
		printf("****************************\n");

		logScore();
		return true; // to stop 
	}
	// stop game LOSE case
	else if(badLuck == 1)
	{
		printf("***********************************\n");

		printf("          YOU LOSE!         	   \n");
		printf("  Better luck next time ...      \n");

		printf("***********************************\n");

		logScore();
		return true; // to stop 
	}
	// stop game LOSE case
	else if(score <= 0 && live <= 0 )
	{
		printf("***********************************\n");

		printf("           YOU LOSE!             \n");
		printf("   score <= 0 && lives = 0       \n");

		printf("***********************************\n");

		logScore();
		return true; // to stop 
	}
	// stop game LOSE case
	else if (bombs <= 0)
	{
		printf("***********************************\n");

		printf("          YOU LOSE!             \n");
		printf("  	  	  BOMBS = 0             \n");

		printf("***********************************\n");

		logScore();
		return true; // to stop 
	}
	// stop game LOSE case
	else if(live <= 0)
	{
		/*
		 Because of lives reward the user may reach 0 lives 
		 (got a negative life rewards more than once) 
		 ... therefore he lose.
		*/
		printf("***********************************\n");

		printf("          YOU LOSE!             \n");
		printf("  	  	  Lives = 0             \n");

		printf("***********************************\n");

		logScore();
		return true; // to stop
	}
	
	return false; // To continue
}


//----------------------------

/*
	The following func.
	are respons. for adding 
	a reward effect.
*/

bool rewardGained(int x)
{
	/*
		**NOTE:

		board[x][y] - 17 = 0 --> Double Range
		board[x][y] - 18 = 1 --> Exit tile
		etc...
	*/

	if(x == 0) // x - 17 = 0
	{
		exitTile = 1;	
	}
	else if(x == 1) //x - 17 = 1
	{
		badLuck = 1;
	}
	else if(x == 2) //x - 17 = 2
	{
		doublerangeCount++;	
	}
	else if(x == 3)//x - 17 = 3 
	{
		int y = randomInt(1,3);

		if (y == 1 || y == 3)
		{
			y = randomInt(1,3);
			

			if( y == 2 && (rows >= 15 || cols >=15) )
			{
				printf("2X:(@,-)  ");
				bombsCount-=2;
			}
			else
			{
				printf("1X:(@,-)  ");
				bombsCount--;
			}
			
		}
		else
		{
			printf("1X:(@,+)  ");
			bombsCount++;
		}	
	}
	else if(x == 4) //x - 17 = 4
	{
		if (randomInt(1,3) ==1)
		{
			printf("(#,-)  ");
			livesCount--;	
		}
		else
		{
			// # of lives less than or equal 5 only
			if(live+livesCount+1 <= 5)
			{
				printf("(#,+)  ");
				livesCount++;
			}
			else
			{
				printf("(#,-)  ");
				livesCount--;
			}		
		}	
	}

	return true;
}

//----------------------------
/*
	return true iff
	the value >= 17

	*NOTE: rewards are between
	17 - 20
*/
bool isReward(float value)
{
	if(value >= 17)
		return true;
	return false;
}

//----------------------------

/*
	cases func. is a recursive function ...
	-------------------------------------
	The purpose of the two loop is to implenent
	the following patteren of function calls
	-------------------------------------
	(x,y) :

	(1) (x-1,y-1)+(x-1,y)+(x-1,y+1)
	(2) (x,y-1)+(x,y)+(x,y+1)
	(3) (x+1,y-1)+(x+1,y)+(x+1,y+1)

	Score : (1) + (2) + (3)
	--------------------------------------

	** DR : Double range, MP: Main point

	**************************************
	Inside the second loop. there are 4 CASES:
	**************************************

	(1) Out of range.
	--------------------------------------
	(2) Uncovered then nglected.
	--------------------------------------
	(***)

		localD means local Double range
		each cell has its case (local case)

		IF localD is > 0 || == -5
		then we have DR

	-------------------------------------
	(3) If it is a reward (TRUE)

		(A)--> localD > 0 || == -5
			call getValue()
	
		(B)--> NO DR

	--------------------------------------	
	(4/ELSE) localD > 0 ==> there is DR
		
		(A)--> localD > 0 || == -5
			call getValue()
	
		(B)--> NO DR
	--------------------------------------
	(***)

		localD = -5 it is just a random number.
	    If the point is a border point then
	    it sends the value -5 AND this cell
	    is not going to make any farther call
	    agian. it's gonna calc. its surrounding.

	    BORDER POINT: THE CELL THAT IS LOCATED ON
	    THE END OF THE DOUBLE RANGE	EFFECT.

	    EX1:
	    IF doublerange = 1 and the selected cell
	    is(MAIN POINT) (0,0) --> the border pointes are:
	    (1,0), (1,1), (0,1)
	    Those point should calc. their surrounding cells value.

	    EX2:
	    DR = 2 and main point is (0,0)
	    (2,0), (2,1), (2,2), (0,2), (1,2)   	
*/

float cases(int x, int y, int localD)
{

	float i = 0.0; // the returned value to score

	// px and py is just to make the series of recursive calls
	// as mentioned above
	int px = x, py = y, counter1 = -1, counter2 = -1;
	
	while(counter1 < 2) // loop 1
	{
		px=x+counter1;
		counter2 = -1;
		py = y;

		while(counter2 < 2) // loop 2
		{	

			py=y+counter2;		

			//----------------------------

			// The 4 CASES

			if(!checkInput(px,py)) // out of range (1)
			{
				i+=0;
			}
			//----------------------------
			else if(!covered(*(board+cols*px+py))) // uncovered tile (2)
			{
				i+=0;
			}
			//----------------------------
			else if(isReward(*(board+cols*px+py))) // A reward (3)
			{	
				if(localD > 0 || localD == -5)// Double range
				{
						i+=getValue(x, px, y, py, localD);
				}
				else // no double range
				{
						i+=signReward(px,py);
				}			
			}
			//----------------------------
			else // covered tile (4)
			{
				 if (localD > 0 || localD == -5) // Double range
				{	
					i+=getValue(x, px, y, py, localD);
				}
				else // No double range
				{
					i+=signValue(px, py);
				}
			}
			//----------------------------

			counter2++;
		}

		counter1++;
	}

	return i;
}

//----------------------------

/*
	return float value if there is no
	further calls are needed.
	This function is going to be called
	IFF there is a double range.
*/
float getValue(int x1, int x2, int y1, int y2, int localD)
{
	// (1)
	if(localD == -5)// LAST CALL (x1,y1) are border point
	{
		// IF (x2,y2) adjecent to border cell (x1,y1)
		// && not direct to main point in case of DR
		if(!direct(x2, y2)) 
		{
			// get its value (make it uncovered)
			return sign(x2, y2);		
		}
		else
		{
			// take its value and call cases()
			return sign(x2,y2)+cases(x2,y2,-5);
		}
	}
	// (2)
	else if(x1 == x2 && y1 == y2) // it is main point
	{
		// get its value
		return sign(x1,y1);
	}
	// (3)
	else if(border(x2, y2)) // it is border point
	{
		// get its value and make LAST CALL
		// to calc the surrounding cells
		return sign(x2,y2)+cases(x2,y2,-5);
	}
	// (4)
	else // recursive call
	{
		// make a new call to cases func.
		return cases(x2,y2,localD);
	}

	return 0.0;
}

//----------------------------	

/*
	This func. just call other
	two func. & return float value
*/

float sign(int x, int y)
{
	// if value is >= 17
	if( isReward(*(board+cols*x+y)) )
	{
		return signReward(x,y);
	}
	// else
	return signValue(x,y);
}

//----------------------------

/*
	This func. changes a value of 
	the cell (x,y) to +16 or -16
	----------------------------
	+ and - 16 means that the cell
	is already uncovered and should be
	neglected next time.
	-----------------------------
	This func. ret. the value of cell
	before setting + or - 16.
*/

float signValue(int x, int y)
{
	float temp = *(board+cols*x+y);

	// less than 0 => -16
	if(temp < 0)
		*(board+cols*x+y) = -16;
	else
		*(board+cols*x+y) = 16;
	
	return temp;
}

//----------------------------

/*
	make the cell uncovered by
	mult. its value by -1.

	call rewardgained() to get 
	reward effect.

	return float 0.0
*/

float signReward(int x, int y)
{
	float temp = *(board+cols*x+y);

	rewardGained((int)temp-17);

	// make it uncovered
	*(board+cols*x+y)*=-1;
	
	return 0.0;
}

//----------------------------

/*
	The cell is direct to main point(selected cell to place the bomb)
	if the difference between them is zero or <= 
	the effect of double range.

	IF the (x,y) are direct they must not be uncovered
	directly, they do their calls to the surrounding cells
	and later we get their value.

	EX:
	    DR = 2 and main point is (0,0)
	    Border point : (2,0), (2,1), (2,2), (0,2), (1,2)

	    All border points are direct to main point (0,0)
*/

bool direct(int x, int y)
{
	int r1 = abs(MPX - x);
	int r2 = abs(MPY - y);

	// if true then direct ...
	return (r1 == 0 || r1 <= doubleRange) && (r2 == 0 || r2 <= doubleRange);
}

//----------------------------

/*
	This func chek whether the point is border or not

	EX:
	    DR = 2 and main point is (0,0)
	    Border point : (2,0), (2,1), (2,2), (0,2), (1,2)
		Then these points calc. its surrounding cells' values

	** RETURN TRUE 
*/
bool border(int x, int y)
{
	if(x == MPX-doubleRange || x == doubleRange+MPX)
	{
		return true;
	}
	else if(y == MPY-doubleRange || y == doubleRange+MPY)
	{
		return true;
	}

	return false;
}

//----------------------------

/*

The following 4 func.

To display:
	->values of 2D-array
	->Game patteren
	-> Symbol like: X, *, ..etc
	->Data:live, score, bombs

*/

void displayValues()
{
	// A pointer to 2d-array in main()
	float *copy = board;

	for (int i = 0; i < rows ; ++i)
	{

		for(int j = 0; j < cols; ++j)
		{
			
			if( *copy >= 17)
				printf("%c\t", packs[(int)(*copy) -17] );
			else
				printf("%.2f\t", *copy );
			copy+=1;
		}	

		printf("\n");
	}

}

//----------------------------

void displayGame()
{
	// A pointer to 2d-array in main()
	float *copy = board;

	for (int i = 0; i < rows ; ++i)
	{	

		for(int j = 0; j < cols; ++j)
		{
			printf(" %c ", symbol(*copy)); // 3 spaces
			copy+=1;
		}

		printf("\n");
	}

	displayData();
}

//----------------------------

char symbol(float x)
{

	if(covered(x)) // covered X
		return 'X';
	else if(x<=-17) // reward ! @ $ *
		return packs[(int)fabsf(x)-17];
	else if(x == -16) // uncovered < 0 (-)
		return '-';
	else // covered > 0 (+)
		return '+';
}

//----------------------------

void displayData()
{
	printf("\nLives : %d\n", live);
	printf("Score : %.2f\n", score);
	printf("Bombs : %d\n", bombs);
}

//----------------------------

/*
	Storing the name, score, time
	into a text file ...
*/

void logScore()
{
	FILE *f;
	// Time (2)
	time_t t2 = time(NULL);

	f = fopen("score.txt", "a+"); // append and write mode

	// Difference between t1 and t2
	//float result = ((float) (t2-t1))/CLOCKS_PER_SEC;
	float result = t2 - t1;
	// the file format
	fprintf(f, "%s\t%.2f\t%.2f\n",name,score,result);

	fclose(f);
}

//----------------------------
/*
	Display the requested number of ordered records
*/
void displayScores(int x)
{
	printf("\n\n\n--------------------------------\n\n\n");

	FILE *f; 
	
	int size = numberOfRows();

	if(size == 0)
	{
		printf("There is no record to be displayed ... \n");
	}
	else
	{
		char Fname[size][10];

		float Fscore[size], Ftime[size];

		int counter = 0;

		f = fopen("score.txt", "r");

		// read data from the file
		while(fscanf(f, "%s\t%f\t%f\t", Fname[counter], &Fscore[counter], &Ftime[counter] ) != EOF)
		{
			counter++;
		}

		fclose(f);

		float temp;

		char tempName[10];

		if(counter < x)
		{
			printf("The maximun number of records (%d) < %d\n\n\n", counter, x );
			x = counter;
		}

		// sorting the records 
		for(int i = 0; i < counter; i++)
		{
			for(int j = i+1; j < counter; j++)
			{
				if(Fscore[j] > Fscore[i])
				{
					//------------------
					temp = Fscore[i];
					Fscore[i] = Fscore[j];
					Fscore[j] = temp;
					//--------------------
					strcpy(tempName, Fname[i]);
					strcpy(Fname[i],Fname[j]);
					strcpy(Fname[j],tempName);
					//-------------------
					temp = Ftime[i];
					Ftime[i] = Ftime[j];
					Ftime[j] = temp;
					//------------------ 
				}
			}
		}

		// display

		for( int i = 0; i < x; i++)
		{
			printf("NAME: %s\tSCORE: %.2f\tTime: %.2f\n", Fname[i], Fscore[i], Ftime[i] );
		}
	}
}

//----------------------------

/*
	return int # -> number of records in file
*/

int numberOfRows()
{
	int counter = 0;
	char ch;
	FILE *f;

	if( (f = fopen("score.txt","r")) != NULL)
	{
		while((ch=fgetc(f))!=EOF)
        	if (ch == '\n') // Incre. counter 
            	counter++;

        fclose(f);
        return counter;
    }  

    return 0;    	 
}


//----------------------------