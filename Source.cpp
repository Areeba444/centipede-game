#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;

const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX;
const int gameColumns = resolutionY / boxPixelsY;

int gameGrid[gameRows][gameColumns] = {};
int score = 0;
const int x = 0;
const int y = 1;
const int exists = 2;
const int head = 3;
const int move_left = 4;
const int down = 5;

bool gameOver = false;

void bullet_firing(float bullet[], float player[], int boxPixelsY);
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite);
void moveBullet(float bullet[], sf::Clock& bulletClock);
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite);
void bullet_sound(sf::Music bulletMusic);
void movePlayer(float player[], sf::Clock& PlayerClock);
void drawmushroom(sf::RenderWindow& window, float mushroom[][3], sf::Sprite& mushroomSprite, sf::Sprite& halfmushroomSprite, int mushroom_count, float bullet[]);
void drawcentipede(sf::RenderWindow& window, float centipede1[12][6], sf::Sprite& centipede1Sprite, sf::Sprite& centipede2Sprite);
void checking_collision(float mushroom[][3], int mushroom_count, float bullet[]);
void movecentipede(float centipede1[12][6], sf::Clock& CentipedeClock);
bool playertermination(float centipede1[12][6], float player[], int gameGrid[][gameColumns]);
bool centipede_headshot(float centipede1[12][6], float bullet[], int gameGrid[][gameColumns]);
void centimush(float centipede1[12][6], float mushroom[][3], int gameGrid[][gameColumns], int mushroom_count);

int main()
{

	srand(time(0));

	sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);
	window.setSize(sf::Vector2u(640, 640));
	window.setPosition(sf::Vector2i(500, 100));

	sf::Music bgMusic;
	bgMusic.openFromFile("Music/field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(100);

	sf::SoundBuffer buffer;
	buffer.loadFromFile("Sound Effects/fire1.wav");
	sf::Sound sound;
	sound.setBuffer(buffer);

	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.20));

	float player[3] = {};
	player[x] = (gameColumns / 2) * boxPixelsX;
	player[y] = (gameColumns * 3 / 4) * boxPixelsY;
	player[exists] = true;
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	sf::Clock PlayerClock;

	float bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY;
	bullet[exists] = false;
	sf::Clock bulletClock;
	sf::Texture bulletTexture;
	sf::Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	bool fire_bullet = false;
	bool press_space = false;


	float mushroom[30][3] = { 0 };
	int mushroom_count = rand() % 11 + 20;

	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	int randx;
	int randy;

	for (int i = 0; i < mushroom_count; i++)
	{
		randx = (rand() % gameColumns) * boxPixelsX;
		randy = (rand() % (gameRows * 3 / 4)) * boxPixelsY;
		mushroom[i][x] = randx;
		mushroom[i][y] = randy;
		mushroom[i][exists] = true;
		gameGrid[randx / boxPixelsX][randy / boxPixelsY] = 7;
	}



	sf::Texture halfmushroomTexture;
	sf::Sprite halfmushroomSprite;
	halfmushroomTexture.loadFromFile("Textures/mushroom.png");
	halfmushroomSprite.setTexture(halfmushroomTexture);
	halfmushroomSprite.setTextureRect(sf::IntRect(64, 0, boxPixelsX, boxPixelsY));

	int bullet_count = 2;

	float centipede1[12][6] = {};

	sf::Texture centipede1Texture;
	sf::Sprite centipede1Sprite;
	centipede1Texture.loadFromFile("Textures/c_body_left_walk.png");
	centipede1Sprite.setTexture(centipede1Texture);
	centipede1Sprite.setTextureRect(sf::IntRect(0, 0, 28, boxPixelsY));

	int start_centix = rand() % (resolutionX - 12 * boxPixelsX);
	int start_centiy = rand() % (resolutionY - boxPixelsY);

	for (int i = 0; i < 12; i++)
	{
		centipede1[i][x] = start_centix + ((i)*boxPixelsX);
		centipede1[i][y] = start_centiy;
		centipede1[0][head] = true;
		centipede1[i][exists] = true;
		centipede1[i][move_left] = false;
		centipede1[i][down] = false;

	}

	sf::Texture centipede2Texture;
	sf::Sprite centipede2Sprite;
	centipede2Texture.loadFromFile("Textures/c_head_left_walk.png");
	centipede2Sprite.setTexture(centipede2Texture);
	centipede2Sprite.setTextureRect(sf::IntRect(0, 0, 28, boxPixelsY));
	sf::Clock CentipedeClock;

	sf::Font font;
	if (!font.loadFromFile("Roboto-Bold.ttf")) {
		cout << "Error" << endl;
	}

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(10, 10);

	while (window.isOpen())
	{
		if (gameOver)
		{
			sf::Text gameOverText;

			scoreText.setString("Score: " + std::to_string(score));

			gameOverText.setFont(font);
			gameOverText.setCharacterSize(60);
			gameOverText.setFillColor(sf::Color::Red);
			gameOverText.setString("GAME OVER");
			gameOverText.setPosition(resolutionX / 2 - 160, resolutionY / 2 - 30);
			scoreText.setCharacterSize(40);
			scoreText.setFillColor(sf::Color::White);
			scoreText.setPosition(resolutionX / 2 - 80, resolutionY / 2 +30 );  // Below GAME OVER

			window.draw(gameOverText);
			window.draw(scoreText);
			window.display();

			sf::sleep(sf::seconds(3));
			window.close();
		}
		window.draw(backgroundSprite);


		movePlayer(player, PlayerClock);
		drawPlayer(window, player, playerSprite);
		drawmushroom(window, mushroom, mushroomSprite, halfmushroomSprite, mushroom_count, bullet);
		movecentipede(centipede1, CentipedeClock);
		//centimush(centipede1,mushroom,gameGrid, mushroom_count);
		drawcentipede(window, centipede1, centipede1Sprite, centipede2Sprite);
		if (playertermination(centipede1, player, gameGrid))
		{
			drawPlayer(window, player, playerSprite);
		};

		scoreText.setString("Score: " + std::to_string(score));
		window.draw(scoreText);



		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			{
				return 0;
			}

			if (e.type == sf::Event::KeyPressed)
			{
				if (e.key.code == sf::Keyboard::Space)
				{
					bullet_firing(bullet, player, boxPixelsY);
					sound.play();
				}
			}
		}

		if (bullet[exists])
		{
			moveBullet(bullet, bulletClock);
			drawBullet(window, bullet, bulletSprite);
			checking_collision(mushroom, mushroom_count, bullet);
			if (centipede_headshot(centipede1, bullet, gameGrid))
			{
				drawcentipede(window, centipede1, centipede1Sprite, centipede2Sprite);
			};
		}

		



		window.display();
		window.clear();
	}

	return 0;
}
//bullet firing
void bullet_firing(float bullet[], float player[], int boxPixelsY)
{
	if (!bullet[exists])
	{
		bullet[exists] = true;
		bullet[x] = player[x];
		bullet[y] = player[y] - boxPixelsY;

	}
}
//drawing centipede
void drawcentipede(sf::RenderWindow& window, float centipede1[12][6], sf::Sprite& centipede1Sprite, sf::Sprite& centipede2Sprite)
{
	for (int i = 0; i < 12; i++)
	{
		if (centipede1[0][exists] == true)
		{
			if (i == 0)
			{
				centipede2Sprite.setPosition(centipede1[0][x], centipede1[0][y]);
				window.draw(centipede2Sprite);
			}
			else {
				centipede1Sprite.setPosition(centipede1[i][x], centipede1[i][y]);
				window.draw(centipede1Sprite);
			}
		}
	}
}
//drawing of mushrooms
void drawmushroom(sf::RenderWindow& window, float mushroom[][3], sf::Sprite& mushroomSprite, sf::Sprite& halfmushroomSprite, int mushroom_count, float bullet[])
{
	for (int i = 0; i < mushroom_count; i++)
	{


		if (mushroom[i][exists]) {

			if (gameGrid[int(mushroom[i][x] / boxPixelsX)][int(mushroom[i][y] / boxPixelsY)] == 1)
			{       //prints half mushroom by detecting in game grid
				halfmushroomSprite.setPosition(mushroom[i][x], mushroom[i][y]);
				window.draw(halfmushroomSprite);
			}

			if (gameGrid[int(mushroom[i][x] / boxPixelsX)][int(mushroom[i][y] / boxPixelsY)] == 7)
			{      //print full mushroom by detetcting in game grid
				mushroomSprite.setPosition(mushroom[i][x], mushroom[i][y]);
				window.draw(mushroomSprite);
			}
		}
	}
}
//checking collisions of mushroom and bullet
void checking_collision(float mushroom[][3], int mushroom_count, float bullet[])
{
	for (int i = 0; i < mushroom_count; ++i)
	{

		if (mushroom[i][exists] && mushroom[i][x] / 32 == bullet[x] / 32 && mushroom[i][y] / 32 == bullet[y] / 32 && bullet[exists])
		{
			bullet[exists] = false;  //so bullet disappears whenever it touches a mushroom

			if (gameGrid[int(mushroom[i][x] / boxPixelsX)][int(mushroom[i][y] / boxPixelsY)] == 7)
			{
				//whenever bullet touches a full mushroom it halves it
				gameGrid[int(mushroom[i][x] / boxPixelsX)][int(mushroom[i][y] / boxPixelsY)] = 1;
			}

			else if (gameGrid[int(mushroom[i][x] / boxPixelsX)][int(mushroom[i][y] / boxPixelsY)] == 1)
			{
				//whenever bullet touches a half mushroom, mushroom disappears
				mushroom[i][exists] = false;
				score += 1;
				gameGrid[int(mushroom[i][x] / boxPixelsX)][int(mushroom[i][y] / boxPixelsY)] = 0;
			}

			break;
		}
	}
}

//Drawing of Player
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite)
{
	if (player[exists] == true)
	{
		playerSprite.setPosition(player[x], player[y]);
		window.draw(playerSprite);
	}
}

//Movement of Bullet
void moveBullet(float bullet[], sf::Clock& bulletClock)
{
	if (bulletClock.getElapsedTime().asMilliseconds() < 10)
		return;

	bulletClock.restart();
	bullet[y] -= 32;

	// Check if the bullet is out of bounds or collides with mushrooms
	if (bullet[y] / 32 < 0)
	{
		bullet[exists] = false;
	}
}
//Whenever a bullet collides with centipede head it is killed
bool centipede_headshot(float centipede1[12][6], float bullet[], int gameGrid[][gameColumns])
{
	int bulletgridX = (bullet[x] / boxPixelsX);
	int bulletgridY = (bullet[y] / boxPixelsY);

	for (int i = 0; i < 12; i++)
	{
		int cgridX = (centipede1[0][x] / boxPixelsX);
		int cgridY = (centipede1[0][y] / boxPixelsY);


		if (centipede1[0][exists] == true && (cgridX == bulletgridX) && (cgridY == bulletgridY))
		{

			centipede1[0][exists] = false;
			score += 20;
			bullet[exists] = false;
			gameOver = true;

			return true;
		}
		gameGrid[cgridX][cgridY] = 0;
		
	}
	return false;
}




//moving of centipede within the boundaries of window
void movecentipede(float centipede1[12][6] ,sf::Clock& CentipedeClock)
{
 if (CentipedeClock.getElapsedTime().asMilliseconds() < 80)
		return;

		CentipedeClock.restart();
		bool turnDown = false;

		// Check if head hits a mushroom or wall
		int headX = centipede1[0][x] / boxPixelsX;
		int headY = centipede1[0][y] / boxPixelsY;


		// Check for mushroom
		if (gameGrid[headX][headY] == 1 || gameGrid[headX][headY] == 7)
			turnDown = true;

		for(int i=0;i<12;i++)
	 {

	 if(centipede1[i][move_left])
	 {
	  centipede1[i][x]+=32;
	  }
	  else if(!(centipede1[i][move_left]))
	  {
	  centipede1[i][x]-=32;
	  }

	  if( centipede1[i][x]<=0 || centipede1[i][x]>=928 )
	  {

	  centipede1[i][move_left]=!centipede1[i][move_left];

	  centipede1[i][y]+=32;
	  if(centipede1[i][y]>=928)
	  {
	  centipede1[i][down]=!centipede1[i][down];
	  centipede1[i][y]-=32;
		 }
		 }
	 }
}




//whenever player collides with centipede it is terminated
bool playertermination(float centipede1[12][6], float player[], int gameGrid[][gameColumns])
{
	int PgridX = (player[x] / boxPixelsX);
	int PgridY = (player[y] / boxPixelsY);


	for (int i = 0; i < 12; i++)
	{
		int cgridX = (centipede1[i][x] / boxPixelsX);
		int cgridY = (centipede1[i][y] / boxPixelsY);


		if (centipede1[i][exists] == true && (cgridX == PgridX) && (cgridY == PgridY))
		{
			player[exists] = false;
			gameOver = true;
			return true;

		}
		gameGrid[PgridX][PgridY] = 0;
		
	}
	return false;
}


//Movement of player in player area
void movePlayer(float player[], sf::Clock& PlayerClock)
{
	if (PlayerClock.getElapsedTime().asMilliseconds() < 70)
		return;

	PlayerClock.restart();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (player[y] >= 728)
			player[y] -= 32;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (player[y] < 928)
			player[y] += 32;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (player[x] > 0)
			player[x] -= 32;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (player[x] < 928)
			player[x] += 32;
	}
}

//Drawing of Bullet
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite)
{
	if (bullet[exists] == true)
	{
		bulletSprite.setPosition(bullet[x], bullet[y]);
		window.draw(bulletSprite);
	}
}
