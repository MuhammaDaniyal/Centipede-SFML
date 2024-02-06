#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;
int dir = 2;
int trans = 3;
int UP = 1, 	DOWN = 0,
	LEFT = 0,	RIGHT = 1;

void GridPrint();

void mushroomAddition( Sprite &playerSprite , int NUM_MUSHROOMS , int &mushrooms_added , RenderWindow& window , Sprite& mushroomSprite1 , Clock& mushroomClock , Sprite& backgroundSprite  );
void mushroomDraw( RenderWindow& window , Sprite& mushroomSprite1 , Sprite& mushroomSprite2 , Sprite& mushroomSprite3 , Sprite& mushroomSprite4);

void movePlayer( RenderWindow& window, Sprite& playerSprite , float Pspeed , Clock& playerClock   );
void moveBullet( RenderWindow& window , int maxBullets , bool bullet_exist[] , float Sbullet , Clock& bClock , Sprite bulletSprite[] , Sprite& playerSprite , int& score  , Music& bulletMusic );

void centMove( Sprite* cent , int** centinfo , bool &enterPlayerArea , int centipedeSpeed , int& numBeads , Sprite& LHeadCent , Sprite& RHeadCent , Sprite& LbodyCent , Sprite& RbodyCent , Texture &RheadTexture , Texture &LheadTexture , Texture &RbodyTexture , Texture &LbodyTexture   );
void centInitialization( int numBeads , Sprite& LHeadCent , Sprite& LbodyCent , int** centinfo , Sprite* cent );

void updateNewLevel( Sprite* &cent , int** &centinfo , bool& enterPlayerArea , int& ScentOr , int& BeadsOr , int& numBeads , int& mushrooms_added);

void spiderMove( Sprite& spiderSprite , Vector2f& spiderDir , bool& spiderExists , Clock& spiderClock );

void Coll( Sprite* cent , Sprite& spiderSprite ,  Sprite& playerSprite , int& numBeads , Music& deathMusic );
void closeWindow(sf::Event e, sf::RenderWindow& window);


int main()
{
	// Declaring RenderWindow.
	sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	window.setSize(sf::Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
	// window.setSize(sf::Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize(sf::Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.
	
	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(sf::Vector2i(700, 0));


	srand(time(0));
	int centipedeSpeed=1,
		ScentOr = 5;
	float Sbullet = 2;
	float Pspeed = 0.2;

	int score=0 , lives=3;					//Initialization of score and lives

	Font font;								//Initializing font
	font.loadFromFile("Fonts/Amatic-Bold.ttf");			

	Text textScore;							//Text for score
	textScore.setFont(font);
	textScore.setCharacterSize(56);
	textScore.setFillColor(Color::White);
	textScore.setString("Score: " + to_string(score));
	textScore.setPosition(2,1);

	Text textLives;							//Text for lives
	textLives.setFont(font);
	textLives.setCharacterSize(56);
	textLives.setFillColor(Color::White);
	textLives.setString("Lives: " + to_string(lives));
	textLives.setPosition(2,50);

	// Initializing Background Music.
	sf::Music bgMusic;
	bgMusic.openFromFile("Sound_Effects/field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);

	//Initializing bullet music
	Music bulletMusic;
    bulletMusic.openFromFile("Sound_Effects/fire1.wav");
    bulletMusic.setVolume(5);

	//Initializing death music
	sf::Music deathMusic;	        						       
	deathMusic.openFromFile("Sound_Effects/death.wav");
	deathMusic.setVolume(100);


	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.20)); // Reduces Opacity to 25%

	// Initializing Player 
	Clock playerClock;
	
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	playerSprite.setPosition( (gameColumns / 2) * boxPixelsX , (gameColumns * 3 / 4) * boxPixelsY );					//For setting player position

	//Initilizing spider
	sf::Texture spiderTexture;
	sf::Sprite spiderSprite;
	spiderTexture.loadFromFile("Textures/spider.png");
	spiderSprite.setTexture(spiderTexture);
	spiderSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX*2 , boxPixelsY));
	// spiderSprite.setPosition( 300 , 300 );					//For setting player position

	Clock spiderClock;
	Vector2f spiderDir;
	bool spiderExists=0;

	// Initializing Bullet
	int maxBullets = 10;
	sf::Clock bClock;
	bool bullet_exist[maxBullets]={0};	
	Sprite bulletSprite[maxBullets];
	
	sf::Texture bulletTexture;
	bulletTexture.loadFromFile("Textures/bullet.png");

	for( int i=0 ; i<maxBullets ; i++ )
	{   
		bulletSprite[i].setTexture(bulletTexture);
		bulletSprite[i].setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
		bulletSprite[i].setPosition(900,900);
	}   	

	//Initilizing mushroom 
	int NUM_MUSHROOMS=60, mushrooms_added=0;
	bool enterPlayerArea=false;
	Clock mushroomClock;

	Texture mushroomTexture;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	
	Sprite mushroomSprite1;
	mushroomSprite1.setTexture(mushroomTexture);						//Initailting full sprite mushroom
	mushroomSprite1.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	Sprite mushroomSprite2;
	mushroomSprite2.setTexture(mushroomTexture);						//Initailting broken sprite mushroom
	mushroomSprite2.setTextureRect(sf::IntRect(boxPixelsX*2, 0, boxPixelsX, boxPixelsY));
	
	Sprite mushroomSprite3;
	mushroomSprite3.setTexture(mushroomTexture);						//Initailting poison sprite mushroom
	mushroomSprite3.setTextureRect(sf::IntRect( 0 , boxPixelsX , boxPixelsX, boxPixelsY));

	Sprite mushroomSprite4;
	mushroomSprite4.setTexture(mushroomTexture);						//Initailting broken poison sprite mushroom
	mushroomSprite4.setTextureRect(sf::IntRect(boxPixelsX*2, boxPixelsX , boxPixelsX, boxPixelsY));

	//Addition of mushrooms
	mushroomAddition( playerSprite , NUM_MUSHROOMS , mushrooms_added  , window , mushroomSprite1 , mushroomClock  , backgroundSprite );

	//Initializing centipede
	int numBeads=12;
	int BeadsOr=12;
	Sprite* cent = new Sprite[numBeads];
	Clock centClock;
	int **centinfo = new int*[numBeads];

	centinfo = new int*[numBeads];
	for( int i=0 ; i<numBeads ; i++ )  		centinfo[i] = new int[4];  
			


	Texture LheadTexture;
	LheadTexture.loadFromFile("Textures/c_head_left_walk.png");
	Sprite LHeadCent;
	LHeadCent.setTexture(LheadTexture);
	LHeadCent.setTextureRect(IntRect( 0 , 0 , boxPixelsX , boxPixelsY ));

	Texture RheadTexture;
	RheadTexture.loadFromFile("Textures/c_head_right_walk.png");
	Sprite RHeadCent;
	RHeadCent.setTexture(RheadTexture);
	RHeadCent.setTextureRect(IntRect( 0 , 0 , boxPixelsX , boxPixelsY ));

	Texture LbodyTexture;
	LbodyTexture.loadFromFile("Textures/c_body_left_walk.png");
	Sprite LbodyCent;
	LbodyCent.setTexture(LbodyTexture);
	LbodyCent.setTextureRect(IntRect( 0 , 0 , boxPixelsX , boxPixelsY ));
	
	Texture RbodyTexture;
	RbodyTexture.loadFromFile("Textures/c_body_right_walk.png");
	Sprite RbodyCent;
	RbodyCent.setTexture(RbodyTexture);
	RbodyCent.setTextureRect(IntRect( 0 , 0 , boxPixelsX , boxPixelsY ));

	//For initializing cenetipede for the first time
    centInitialization( numBeads , LHeadCent , LbodyCent , centinfo , cent );

/////////////////////////////////////////////////////////////////////////////////////////////////////////

	while(window.isOpen()) {


		if( numBeads == 0 )
		{	
			//This function updated the requirements for new level
			// updateNewLevel( cent , centinfo , enterPlayerArea , ScentOr , BeadsOr , numBeads , mushrooms_added);
			
			delete[] cent;		cent = nullptr;
			for( int i=0; i<numBeads; i++ )  		delete [] centinfo[i]; 
			delete []centinfo;						centinfo = nullptr;

			//This part is making necessary changes for further iniialization
			ScentOr += 1;			enterPlayerArea=false;	
			BeadsOr += 3;				if( numBeads == 0 )
		{	
			//This function updated the requirements for new level
			// updateNewLevel( cent , centinfo , enterPlayerArea , ScentOr , BeadsOr , numBeads , mushrooms_added);
			
			delete[] cent;		cent = nullptr;
			for( int i=0; i<numBeads; i++ )  		delete [] centinfo[i]; 
			delete []centinfo;						centinfo = nullptr;

			//This part is making necessary changes for further iniialization
			ScentOr += 1;			enterPlayerArea=false;	
			BeadsOr += 3;			numBeads=BeadsOr;

			mushrooms_added=0;
			//This part is initializing both arrays
			centinfo = new int*[numBeads];
			for( int i=0 ; i<numBeads ; i++ )  		centinfo[i] = new int[4];  			
			cent = new Sprite[numBeads];	
			
			//It adds more mushrooms
			mushroomAddition( playerSprite , NUM_MUSHROOMS-20 , mushrooms_added  , window , mushroomSprite1 , mushroomClock , backgroundSprite );
			//It initilizes a new centipede for updated level
			// centInitialization( numBeads , LHeadCent , LbodyCent , centinfo , cent );
	
	
	
			for( int i=0 ; i<numBeads ; i++ )
			{   
				if( i==0 )	cent[i] = LHeadCent;  
				else       cent[i] = LbodyCent;

				gameGrid[10/boxPixelsX][(300 + i*boxPixelsX)/boxPixelsX]=9;  
				centinfo[i][y]=10; centinfo[i][x]=300 + i*boxPixelsX;
				cent[i].setPosition( centinfo[i][x], centinfo[i][y] );

				centinfo[i][dir] = 0;
				centinfo[i][trans] = 0;
			} 
		}

		window.draw(backgroundSprite);
		mushroomDraw( window , mushroomSprite1 , mushroomSprite2 , mushroomSprite3 , mushroomSprite4 );

		spiderMove( spiderSprite , spiderDir , spiderExists , spiderClock );

		sf::Event e;
		while (window.pollEvent(e)) 
		{
			if( Keyboard::isKeyPressed(Keyboard::Escape) )
			{
				delete[] cent;		cent = nullptr;
				for( int i=0; i<numBeads; i++ )  		delete [] centinfo[i]; 
					delete []centinfo;	
				centinfo = nullptr;
				
				closeWindow( e,  window );
			}			
		}

		if( Keyboard::isKeyPressed(Keyboard::S) )	centipedeSpeed=ScentOr*2;
		else 										centipedeSpeed=ScentOr;	

		if( centClock.getElapsedTime().asMilliseconds() > 5 )
		{			
			centMove( cent , centinfo , enterPlayerArea , centipedeSpeed , numBeads , LHeadCent , RHeadCent , LbodyCent , RbodyCent , RheadTexture , LheadTexture , RbodyTexture , LbodyTexture  );
			centClock.restart();
		}

		for( int i=0 ; i<numBeads ; i++ )				window.draw(cent[i]);

		moveBullet( window , maxBullets , bullet_exist , Sbullet , bClock , bulletSprite , playerSprite  , score  , bulletMusic );
	numBeads=BeadsOr;

			mushrooms_added=0;
			//This part is initializing both arrays
			centinfo = new int*[numBeads];
			for( int i=0 ; i<numBeads ; i++ )  		centinfo[i] = new int[4];  			
			cent = new Sprite[numBeads];	
			
			//It adds more mushrooms
			mushroomAddition( playerSprite , NUM_MUSHROOMS-20 , mushrooms_added  , window , mushroomSprite1 , mushroomClock , backgroundSprite );
			//It initilizes a new centipede for updated level
			// centInitialization( numBeads , LHeadCent , LbodyCent , centinfo , cent );
	
	
	
			for( int i=0 ; i<numBeads ; i++ )
			{   
				if( i==0 )	cent[i] = LHeadCent;  
				else       cent[i] = LbodyCent;

				gameGrid[10/boxPixelsX][(300 + i*boxPixelsX)/boxPixelsX]=9;  
				centinfo[i][y]=10; centinfo[i][x]=300 + i*boxPixelsX;
				cent[i].setPosition( centinfo[i][x], centinfo[i][y] );

				centinfo[i][dir] = 0;
				centinfo[i][trans] = 0;
			} 
		}

		window.draw(backgroundSprite);
		mushroomDraw( window , mushroomSprite1 , mushroomSprite2 , mushroomSprite3 , mushroomSprite4 );

		spiderMove( spiderSprite , spiderDir , spiderExists , spiderClock );

		sf::Event e;
		while (window.pollEvent(e)) 
		{
			if( Keyboard::isKeyPressed(Keyboard::Escape) )
			{
				delete[] cent;		cent = nullptr;
				for( int i=0; i<numBeads; i++ )  		delete [] centinfo[i]; 
					delete []centinfo;	
				centinfo = nullptr;
				
				closeWindow( e,  window );
			}			
		}

		if( Keyboard::isKeyPressed(Keyboard::S) )	centipedeSpeed=ScentOr*2;
		else 										centipedeSpeed=ScentOr;	

		if( centClock.getElapsedTime().asMilliseconds() > 5 )
		{			
			centMove( cent , centinfo , enterPlayerArea , centipedeSpeed , numBeads , LHeadCent , RHeadCent , LbodyCent , RbodyCent , RheadTexture , LheadTexture , RbodyTexture , LbodyTexture  );
			centClock.restart();
		}

		for( int i=0 ; i<numBeads ; i++ )				window.draw(cent[i]);

		moveBullet( window , maxBullets , bullet_exist , Sbullet , bClock , bulletSprite , playerSprite  , score  , bulletMusic );

	
	for( int i=0 ; i<maxBullets ; ++i )
	if( bullet_exist[i] == true )
	{	
		int X=bulletSprite[i].getPosition().x/boxPixelsX , Y=bulletSprite[i].getPosition().y/boxPixelsY;		
		
		if( bulletSprite[i].getGlobalBounds().intersects(spiderSprite.getGlobalBounds()) )
			spiderExists=0;

		if( gameGrid[Y][X] == 9 )
		{							//For collision detection of bullet and centipede
			
			numBeads--;					//Numbeads decrement
			bullet_exist[i]=false;				//Bullet disappears

			for(int i=0; i<numBeads; i++)
			{
			   	int tempx=centinfo[i][0]/boxPixelsX,	//column
			   		tempy=centinfo[i][1]/boxPixelsY;	//Rows
			   	if(tempx==X && tempy==Y)		//If centepede and bullet are collidng
			   	{

					if( cent[i].getTexture() == &LheadTexture )		score += 30;
					else											score += 10;

					//Making another head for the centipede which i now broken					
					cent[i+1].setTexture(LheadTexture);
					cent[i+1].setTextureRect(IntRect( 0 , 0 , boxPixelsX , boxPixelsY ));

				    for(int j=i; j<numBeads; j++)		//kills on bead and shift all to behind
			    	{
						cent[j] = cent[j+1];
						centinfo[j][0]=centinfo[j+1][0];
						centinfo[j][1]=centinfo[j+1][1];							
			      	}
					break;
			    }
			}
			if( enterPlayerArea )	   gameGrid[Y][X] = 3;			//Poisnous Mushroom is added at that position
			else					   gameGrid[Y][X] = 1;			//Normal Mushroom is added at that position
		}	
	}

	Coll( cent , spiderSprite , playerSprite , numBeads , deathMusic );

		textScore.setString("Score: " + to_string(score));			//Update score text
		textLives.setString("Lives: " + to_string(lives));			//Update lives text
		window.draw(textScore);						//draw text score
		window.draw(textLives);						//draw text lives
		if( spiderExists )		window.draw( spiderSprite );
	
		window.draw(playerSprite);		
		movePlayer( window, playerSprite , Pspeed , playerClock  );	
		window.display();
		window.clear();
	}
}

void centMove( Sprite* cent , int** centinfo , bool &enterPlayerArea , int centipedeSpeed , int& numBeads , Sprite& LHeadCent , Sprite& RHeadCent , Sprite& LbodyCent , Sprite& RbodyCent , Texture &RheadTexture , Texture &LheadTexture , Texture &RbodyTexture , Texture &LbodyTexture )
{
		for (int i = 0; i < numBeads; i++ )
		{
			
			int py=centinfo[i][y],		px=centinfo[i][x];
			int RGrid = gameGrid[py/boxPixelsY][(px+boxPixelsX)/boxPixelsX],	//For one step ahead of grid to left
				LGrid = gameGrid[py/boxPixelsY][(px-boxPixelsX/6)/boxPixelsX],  //For one step ahead of grid to right
				presentGrid = gameGrid[py/boxPixelsY][px/boxPixelsX];			//For at that present spot


			if( cent[i].getPosition().y > 650 )  enterPlayerArea = true;		//centipede entered player area
			//Moves the centipede UP when colliding the wall OR mushrooms							This one is for direction
			if( (cent[i].getPosition().x >= 900 || RGrid==1 || RGrid==2 || RGrid==3 || RGrid==4 ) && centinfo[i][dir]==RIGHT  && centinfo[i][trans]==UP )
			{
				//Moving one step down , changing direction to left ,& updating grid
				centinfo[i][dir] = LEFT;		centinfo[i][y]-=boxPixelsX;		cent[i].move( 0 , -boxPixelsY );	

				//Updating grid according to head or body
				gameGrid[(py/boxPixelsY)][px/boxPixelsX]=0;				gameGrid[(py-boxPixelsY)/boxPixelsY][px/boxPixelsX]=9;	
			
				if( cent[i].getPosition().y < 650 )		centinfo[i][trans] = 0;

			}	//             Checks if going out of the screen or any kind of mushroom			   horizaontal direction     vertical direction
			else if( (cent[i].getPosition().x <= 10 || LGrid==1 || LGrid==2 || LGrid==3 || LGrid==4 ) && centinfo[i][dir]==LEFT && centinfo[i][trans]==UP  )
			{
				centinfo[i][dir] = RIGHT;		centinfo[i][y]-=boxPixelsX;		cent[i].move( 0 , -boxPixelsX );

				//Updating grid according to head or body				
				gameGrid[(py/boxPixelsY)][px/boxPixelsX]=0;				gameGrid[(py-boxPixelsY)/boxPixelsY][px/boxPixelsX]=9;	
			
				if( cent[i].getPosition().y < 650 )		centinfo[i][trans] = 0;			

			}

			//Moves the centipede DOWN when colliding the wall OR mushrooms							This one is for direction
			else if( (cent[i].getPosition().x >= 900 || RGrid==1 || RGrid==2 || RGrid==3 || RGrid==4 ) && centinfo[i][dir]==RIGHT  && centinfo[i][trans]==DOWN )
			{
				//Moving one step down , changing direction to left ,& updating grid
				centinfo[i][dir] = LEFT;		centinfo[i][y]+=boxPixelsX;		cent[i].move( 0 , boxPixelsY );	
				// cent[i].rotate(180.0f);
				
				//Updating grid according to head or body
				gameGrid[(py/boxPixelsY)][px/boxPixelsX]=0;				gameGrid[(py+32)/boxPixelsY][px/boxPixelsX]=9;	
				
				if( cent[i].getPosition().y >= 850 )	centinfo[i][trans] = 1;
			}				
			else if( (cent[i].getPosition().x <= 10 || LGrid==1 || LGrid==2 || LGrid==3 || LGrid==4 ) && centinfo[i][dir]==LEFT && centinfo[i][trans]==DOWN  )
			{//  Checks if going out of the screen or any kind of mushroom								Checks the direction			
			
				centinfo[i][dir] = RIGHT;		centinfo[i][y]+=boxPixelsX;		cent[i].move( 0 , boxPixelsX );

				//Updating grid according to head or body	
				gameGrid[(py/boxPixelsY)][px/boxPixelsX]=0;				gameGrid[(py+32)/boxPixelsY][px/boxPixelsX]=9;	
			
				if( cent[i].getPosition().y >= 850 )	centinfo[i][trans] = UP;

			}
			
			//This part moves the cenetipede left and right
			else if( centinfo[i][dir] == RIGHT )						
			{	//This is updaitng position in the array  while here it is moving towards right
				centinfo[i][x]+=centipedeSpeed;				    cent[i].move( centipedeSpeed , 0);
				//Updating grid according to head or body
	
				gameGrid[py/boxPixelsY][px/boxPixelsX]=0;  gameGrid[py/boxPixelsY][(px+centipedeSpeed)/boxPixelsX]=9; 
			}			
			else if( centinfo[i][dir] == LEFT )			
			{	//This is updaitng position in the array  while here it is moving towards left
				centinfo[i][x]-=centipedeSpeed;	            	cent[i].move(-centipedeSpeed , 0);
				//Updating grid according to head or body
				gameGrid[py/boxPixelsY][px/boxPixelsX]=0;  gameGrid[py/boxPixelsY][(px-centipedeSpeed)/boxPixelsX]=9; 
			}
		}
		
}


void movePlayer( RenderWindow& window , Sprite& playerSprite, float Pspeed , Clock& playerClock )
{
	int X=playerSprite.getPosition().x,
		Y=playerSprite.getPosition().y;
	int	YY = Y/boxPixelsY,
		XX = X/boxPixelsX;

	if (playerClock.getElapsedTime().asMilliseconds() < 100)			//gives the player rest else it travels very FAST
 		return;
	playerClock.restart();


	//For fast movements
	if( Keyboard::isKeyPressed(Keyboard::Up) && Keyboard::isKeyPressed(Keyboard::LShift) && (Y-32 >= 0) && (Y-64 >= 0) && !(gameGrid[YY-1][XX]==1 || gameGrid[YY-2][XX]==1 )  && !(gameGrid[YY-1][XX]==2 || gameGrid[YY-2][XX]==2 ) && !(gameGrid[YY-1][XX]==3 || gameGrid[YY-2][XX]==3 ) && !(gameGrid[YY-1][XX]==4 || gameGrid[YY-2][XX]==4 ) )
        playerSprite.move( 0, -boxPixelsX*2 );
    else if( Keyboard::isKeyPressed(Keyboard::Down) && Keyboard::isKeyPressed(Keyboard::LShift) && (Y+32 <= 925) && (Y+64 <= 925)  && !(gameGrid[YY+1][XX]==1 || gameGrid[YY+2][XX]==1 )  && !(gameGrid[YY+1][XX]==2 || gameGrid[YY+2][XX]==2 ) && !(gameGrid[YY+1][XX]==3 || gameGrid[YY+2][XX]==3 ) && !(gameGrid[YY+1][XX]==4 || gameGrid[YY+2][XX]==4 ) )
        playerSprite.move( 0 , boxPixelsX*2 );
    else if( Keyboard::isKeyPressed(Keyboard::Left) && Keyboard::isKeyPressed(Keyboard::LShift) && (X-32 >= 0) && (X-64 >= 0)  && !(gameGrid[YY][XX-1]==1 || gameGrid[YY][XX-1]==1 )  && !(gameGrid[YY][XX-1]==2 || gameGrid[YY][XX-1] ) && !(gameGrid[YY][XX-1]==3 || gameGrid[YY][XX-1]==3 ) && !(gameGrid[YY][XX-1]==4 || gameGrid[YY][XX-1]==4 ) )
        playerSprite.move( -boxPixelsX*2 , 0 );
    else if( Keyboard::isKeyPressed(Keyboard::Right) && Keyboard::isKeyPressed(Keyboard::LShift) && (X+32 <= 928) && (X+64 <= 928)  && !(gameGrid[YY][XX+1]==1 || gameGrid[YY][XX+2]==1 )  && !(gameGrid[YY][XX+1]==2 || gameGrid[YY][XX+2]==2 ) && !(gameGrid[YY][XX+2]==2==3 || gameGrid[YY][XX+2]==2==3 ) && !(gameGrid[YY][XX+2]==2==4 || gameGrid[YY][XX+2]==2==4 ) )
        playerSprite.move( boxPixelsX*2 , 0 );

	//Super Power No collision
	else if( Keyboard::isKeyPressed(Keyboard::Up) && Keyboard::isKeyPressed(Keyboard::RShift) && (Y-1 >= 0) )
        playerSprite.move( 0, -boxPixelsX*2 );
    else if( Keyboard::isKeyPressed(Keyboard::Down) && Keyboard::isKeyPressed(Keyboard::RShift) && (Y+1 <= 925) )
        playerSprite.move( 0 , boxPixelsX*2 );
    else if( Keyboard::isKeyPressed(Keyboard::Left) && Keyboard::isKeyPressed(Keyboard::RShift) && (X-1 >= 0) )
        playerSprite.move( -boxPixelsX*2 , 0 );
    else if( Keyboard::isKeyPressed(Keyboard::Right) && Keyboard::isKeyPressed(Keyboard::RShift) && (X+1 <= 930) )
        playerSprite.move( boxPixelsX*2 , 0 );

	//For simple one sided movements
	else if( Keyboard::isKeyPressed(Keyboard::Up) && (Y-1 >= 0) && !(gameGrid[YY-1][XX]==1 ) && !(gameGrid[YY-1][XX]==2 ) && !(gameGrid[YY-1][XX]==3 ) && !(gameGrid[YY-1][XX]==4 ) )
    {
		playerSprite.move( 0, -boxPixelsX );
		gameGrid[YY-1][XX]=5;					
	    gameGrid[YY][XX]=0;							
	}
    else if( Keyboard::isKeyPressed(Keyboard::Down) && (Y+1 <= 925) && !(gameGrid[YY+1][XX]==1 ) && !(gameGrid[YY+1][XX]==2 ) && !(gameGrid[YY+1][XX]==3 ) && !(gameGrid[YY+1][XX]==4 ) )
    {
	    playerSprite.move( 0 , boxPixelsX );
	    gameGrid[YY+1][XX]=5;						//Updating Grid
	    gameGrid[YY][XX]=0;
	}
    else if( Keyboard::isKeyPressed(Keyboard::Left) && (X-1 >= 0) && !(gameGrid[YY][XX-1]==1 ) && !(gameGrid[YY][XX-1]==2 ) && !(gameGrid[YY][XX-1]==3 ) && !(gameGrid[YY][XX-1]==4 ) )
    {
	    playerSprite.move( -boxPixelsX , 0 );
		gameGrid[YY][XX-1]=5;					
	    gameGrid[YY][XX]=0;							
	}
	else if( Keyboard::isKeyPressed(Keyboard::Right) && (X+1 <= 928) && !(gameGrid[YY][XX+1]==1 ) && !(gameGrid[YY][XX+1]==2 ) && !(gameGrid[YY][XX+1]==3 ) && !(gameGrid[YY][XX+1]==4 ) )
    { 
		playerSprite.move( boxPixelsX , 0 );
		gameGrid[YY][XX+1]=5;					
	    gameGrid[YY][XX]=0;							
	}



}



void moveBullet( RenderWindow& window , int maxBullets , bool bullet_exist[] , float Sbullet , Clock& bClock , Sprite bulletSprite[] , Sprite& playerSprite , int& score  , Music& bulletMusic )
{
	if (Keyboard::isKeyPressed(Keyboard::Space) && bClock.getElapsedTime().asMilliseconds() > 100) 
	{ 
		bClock.restart();
	    for (int i = 0; i < maxBullets; i++)
	        if (!bullet_exist[i])
			{
			    bulletMusic.stop();
	    		bulletMusic.play();

	            bullet_exist[i] = true;
	            bulletSprite[i].setPosition( playerSprite.getPosition() );
				break;
	        }
	}

	for (int i = 0; i < maxBullets; i++)
	    if (bullet_exist[i] == true)
		{
			int X=bulletSprite[i].getPosition().x , Y=bulletSprite[i].getPosition().y;

	        bulletSprite[i].move(0, -Sbullet);
	        window.draw(bulletSprite[i]);

			int gridPos = gameGrid[Y/boxPixelsY][X/boxPixelsX];

	        if ( ( Y<-32)  || gridPos==1 || gridPos==2 || gridPos==3 || gridPos==4 )
			{
				if( gridPos==1 )			gameGrid[Y/boxPixelsY][X/boxPixelsX]=2;				
				else if( gridPos==2 )		gameGrid[Y/boxPixelsY][X/boxPixelsX]=0;
				else if( gridPos==3 )		gameGrid[Y/boxPixelsY][X/boxPixelsX]=4;
				else if( gridPos==4 )		gameGrid[Y/boxPixelsY][X/boxPixelsX]=0;				
				
				bullet_exist[i] = false;	
				
				if( !(Y < -32) && !(gridPos==1) && !(gridPos==3)  )	score += 1;						
			
			}
			
		
		}
}


void mushroomAddition( Sprite &playerSprite , int NUM_MUSHROOMS , int &mushrooms_added , RenderWindow& window , Sprite& mushroomSprite1 , Clock& mushroomClock , Sprite& backgroundSprite )
{
	while (mushrooms_added < NUM_MUSHROOMS)			//We are making mushrooms till NUM_MUSHROOMS
    {


			mushroomClock.restart();

	        int row = rand() % gameRows;				//Random numbers for mushroom
	        int col = rand() % gameColumns;

	        if ( gameGrid[row][col] != 1 && row!=0  && (row>2 && col>2))
		    {								//no repetition, no mushroom on player area
	            gameGrid[row][col] = 1;	 			//Fixing mushroom as 1
				mushrooms_added++;	 	 			//mushroom incremented	
		    }

	}
}

void mushroomDraw( RenderWindow& window , Sprite& mushroomSprite1 , Sprite& mushroomSprite2 , Sprite& mushroomSprite3 , Sprite& mushroomSprite4 )
{
	for (int i = 0; i < gameRows; ++i)
	{
   		for (int j = 0; j < gameColumns; ++j)
		{
	        if (gameGrid[i][j] == 1)				//For Drawing full normal mushroom				
		    {
           	    mushroomSprite1.setPosition( j*boxPixelsX, i*boxPixelsY );
           	    window.draw(mushroomSprite1);
            }
		    else if(gameGrid[i][j] == 2)			//For drawing half normal mushroom
		    {
           	    mushroomSprite2.setPosition( j*boxPixelsX , i*boxPixelsY );
           	    window.draw(mushroomSprite2);		
		    }
		    else if(gameGrid[i][j] == 3)			//For drawing full poisnous mushroom
		    {
           	    mushroomSprite3.setPosition( j*boxPixelsX , i*boxPixelsY );
           	    window.draw(mushroomSprite3);		
		    }
		    else if(gameGrid[i][j] == 4)			//For drawing half poisnous mushroom
		    {
           	    mushroomSprite4.setPosition( j*boxPixelsX , i*boxPixelsY );
           	    window.draw(mushroomSprite4);		
		    }
			// else if( gameGrid[i][j] == 8 )
			// {

			// }

   		}
	}
}

void closeWindow(sf::Event e, sf::RenderWindow& window)
{
	if (e.type == sf::Event::Closed)
		window.close();			
	else if( Keyboard::isKeyPressed(Keyboard::Escape) )
		window.close();
	else if( Keyboard::isKeyPressed(Keyboard::G) )	
		GridPrint();	
		
}
void GridPrint()
{
	cout << "\n     ";
	
	for( int i=0 ; i<3 ; ++i)
		for ( int j=0 ; j<10 ; ++j )
			cout << j << " ";
	cout << endl;	
	cout << "   --------------------------------------------------------------\n";

	for(int i=0 ; i < gameRows ; ++i)	 			//Printing Grid
	{
		if( i<10 )		cout << i << "  |";
		else 			cout << i << " |";

		for(int j=0 ; j < gameColumns ; ++j)
			if(gameGrid[i][j]!=0)	cout << gameGrid[i][j] << " ";
			else 					cout << "  ";
		cout << '|' << endl;
	}	
	

	cout << "-------------------------------------------------------------\n";

}


void centInitialization( int numBeads , Sprite& LHeadCent , Sprite& LbodyCent , int** centinfo , Sprite* cent )
{
	

	for( int i=0 ; i<numBeads ; i++ )
	{   
		if( i==0 )	cent[i] = LHeadCent;  
		else       cent[i] = LbodyCent;
		
		gameGrid[10/boxPixelsX][(300 + i*boxPixelsX)/boxPixelsX]=9;  
		centinfo[i][y]=10; centinfo[i][x]=300 + i*boxPixelsX;
		cent[i].setPosition( centinfo[i][x], centinfo[i][y] );

		centinfo[i][dir] = 0;
		centinfo[i][trans] = 0;
	}   
}	

void updateNewLevel( Sprite *&cent , int** &centinfo , bool& enterPlayerArea , int& ScentOr , int& BeadsOr , int& numBeads , int& mushrooms_added)
{
	//This part is deleting both arrays and asigning NULL to them
	delete[] cent;		cent = nullptr;
	for( int i=0; i<numBeads; i++ )  		delete [] centinfo[i]; 
	delete []centinfo;						centinfo = nullptr;
	
	//This part is making necessary changes for further iniialization
	ScentOr += 1;			enterPlayerArea=false;	
	BeadsOr += 3;			numBeads=BeadsOr;
	
	mushrooms_added=0;
	//This part is initializing both arrays
	centinfo = new int*[numBeads];
	for( int i=0 ; i<numBeads ; i++ )  		centinfo[i] = new int[4];  			
	cent = new Sprite[numBeads];	
}


void spiderMove( Sprite& spiderSprite , Vector2f& spiderDir , bool& spiderExists , Clock& spiderClock )
{
	if( spiderClock.getElapsedTime().asSeconds() < 0.01 )
		return;
	
	spiderClock.restart();
	static int X , Y;

		if( !spiderExists )
		{
			int deploy;
			deploy = rand() % 200;
			if( !deploy )
			{
				spiderDir.x = rand()%950;  spiderDir.y = 900-(rand()%300);
				spiderSprite.setPosition( spiderDir );
				spiderExists = 1;
				X=(rand()%3)-1 , Y=(rand()%3)-1;
			}
		}
		else
		{
			int posX=spiderSprite.getPosition().x + X*2  ,  posY=spiderSprite.getPosition().y + Y*2;

			if( posX>960 || posX<0 )		spiderExists=0;
			else											 //if( posY<900 && posY>600  )
			{
				int num=rand();
				if( posY>900 || posY<600  )		rand()%2 ? ( Y==0 ? Y=1 : Y=-Y ):( X==0 ? X=1 : X=-X );
									
				spiderSprite.move( X*2 , Y*2 );
				
				if( gameGrid[posY/boxPixelsY][posX/boxPixelsX]==1 )
				{	
					rand()%2 ? ( Y==0 ? Y=1 : Y=-Y ):( X==0 ? X=1 : X=-X );
					gameGrid[posY/boxPixelsY][posX/boxPixelsX]=0;
				}
			}
		}
}

void Coll( Sprite* cent , Sprite& spiderSprite ,  Sprite& playerSprite , int& numBeads , Music& deathMusic )
{

	if( playerSprite.getGlobalBounds().intersects(spiderSprite.getGlobalBounds()) )
	{   deathMusic.stop()    ,deathMusic.play();          }
	
	for( int i=0 ; i<numBeads ; i++ )
	{   
		if( playerSprite.getGlobalBounds().intersects(cent[i].getGlobalBounds()) )
		{   deathMusic.stop()    ,deathMusic.play();      }
	}   

}
