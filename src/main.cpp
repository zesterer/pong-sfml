#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <sstream>

std::string intToStr(const int value)
{
	static std::string result;
	std::ostringstream convert;

	convert << value;
	result = convert.str();

	return result;
}

//The generic bat structure
struct Bat
{
	public:
	sf::RectangleShape rect;
	
	sf::Vector2f pos;
	sf::Vector2f speed;
	
	bool ai;
	
	float win_w;
	float win_h;
	
	public: void setup(float win_w, float win_h, int position, bool ai = false)
	{
		//Create the rectangle
		this->rect = sf::RectangleShape(sf::Vector2f(4.0, 64.0));
		
		//Create the speed
		this->speed = sf::Vector2f(0.0f, 0.0f);
		
		//What's the window size?
		this->win_w = win_w;
		this->win_h = win_h;
		
		this->ai = ai;
		
		//If position is 0, it's on the left. If it's 1, it's on the right
		if (position)
			this->pos = sf::Vector2f(win_w - this->rect.getSize().x, win_h / 2 - this->rect.getSize().y / 2);
		else
			this->pos = sf::Vector2f(0.0f, win_h / 2 - this->rect.getSize().y / 2);
	}
	
	public: void step(float bally)
	{
		//Slow the bat down
		this->speed.x /= 1.2;
		this->speed.y /= 1.2;
		
		//Increment position according to speed
		this->pos.x += this->speed.x;
		this->pos.y += this->speed.y;
		
		//Define the maximum and minimum bounds it can go to
		float min_y = 0;
		float max_y = this->win_h - this->rect.getSize().y;
		
		//Keep the bat on the screen
		if (this->pos.y < min_y)
			this->pos.y = min_y;
		if (this->pos.y > max_y)
			this->pos.y = max_y;
		
		if (this->ai)
		{
			if (bally > this->pos.y + this->rect.getSize().y / 2 - 16)
			{
				this->speed.y += 0.9;
			}
		
			if (bally < this->pos.y + this->rect.getSize().y / 2 + 16)
			{
				this->speed.y -= 0.9;
			}
		}
		
		//Update the bat's rectangle with the position
		this->rect.setPosition(this->pos);
	}
};

struct Ball
{
	public:
	sf::CircleShape circle;
	
	sf::Vector2f pos;
	sf::Vector2f speed;
	
	float win_w;
	float win_h;
	
	int pause;
	
	public: void setup(float win_w, float win_h, int position)
	{
		//Create the circle
		this->circle = sf::CircleShape(6.0f);
		
		//Create the speed
		this->speed = sf::Vector2f(4.5f, 0.0f);
		
		//What's the window size?
		this->win_w = win_w;
		this->win_h = win_h;
		
		this->pos = sf::Vector2f(win_w / 2, win_h / 2);
		
		this->pause = 50;
	}
	
	public: void step(Bat& bat_l, Bat& bat_r, int& points_0, int& points_1)
	{
		if (!this->pause)
		{
			//Increment position according to speed
			this->pos.x += this->speed.x;
			this->pos.y += this->speed.y;
		}
		else
		{
			this->pause --;
		}
		
		//Keep the bat on the screen (Y)
		if (this->pos.y < 0)
			this->speed.y = abs(this->speed.y);
		if (this->pos.y > this->win_h - this->circle.getRadius() * 2)
			this->speed.y = -abs(this->speed.y);
		
		//Keep the bat on the screen (X)
		if (this->pos.x < bat_l.rect.getSize().x)
		{
			//Check it's on the bat
			bool check_top = this->pos.y + this->circle.getRadius() * 2 > bat_l.pos.y;
			bool check_bottom = this->pos.y < bat_l.pos.y + bat_l.rect.getSize().y;
			
			if (check_top && check_bottom)
			{
				this->speed.x = abs(this->speed.x);
				this->speed.y = ((this->pos.y + this->circle.getRadius()) - (bat_l.pos.y + bat_l.rect.getSize().y / 2)) / 6.0f;
			}
			else if (this->pos.x < -this->circle.getRadius() * 2)
			{
				this->pos.x = this->win_w / 2 - this->circle.getRadius();
				this->pos.y = this->win_h / 2 - this->circle.getRadius();
				this->speed.y = 0.0f;
				points_1 += 1;
				this->pause = 50;
				bat_l.pos.y = this->win_h / 2 - bat_l.rect.getSize().y / 2;
				bat_r.pos.y = this->win_h / 2 - bat_r.rect.getSize().y / 2;
			}
		}
		if (this->pos.x > this->win_w - this->circle.getRadius() * 2 - bat_l.rect.getSize().x)
		{
			//Check it's on the bat
			bool check_top = this->pos.y + this->circle.getRadius() * 2 > bat_r.pos.y;
			bool check_bottom = this->pos.y < bat_r.pos.y + bat_r.rect.getSize().y;
			
			if (check_top && check_bottom)
			{
				this->speed.x = -abs(this->speed.x);
				this->speed.y = ((this->pos.y + this->circle.getRadius()) - (bat_r.pos.y + bat_r.rect.getSize().y / 2)) / 6.0f;
			}
			else if (this->pos.x > this->win_w)
			{
				this->pos.x = this->win_w / 2 - this->circle.getRadius();
				this->pos.y = this->win_h / 2 - this->circle.getRadius();
				this->speed.y = 0.0f;
				points_0 += 1;
				this->pause = 50;
				bat_l.pos.y = this->win_h / 2 - bat_l.rect.getSize().y / 2;
				bat_r.pos.y = this->win_h / 2 - bat_r.rect.getSize().y / 2;
			}
		}
		
		//Update the bat's rectangle with the position
		this->circle.setPosition(this->pos);
	}
};

int main()
{
	//Create the window
	sf::RenderWindow window(sf::VideoMode(400, 256), "Pong");

	//Create the left bat
	Bat bat_l;
	bat_l.setup(window.getSize().x, window.getSize().y, 0, true);
	
	//Create the right bat
	Bat bat_r;
	bat_r.setup(window.getSize().x, window.getSize().y, 1);
	
	//Create the points
	int points[2] = {0, 0};
	
	//Create the ball
	int ball_number = 1;
	Ball ball[ball_number];
	for (int count = 0; count < ball_number; count ++)
	{
		ball[count].setup(window.getSize().x, window.getSize().y, 1);
	}
	
	//Find the font
	sf::Font font;
	font.loadFromFile("resources/commodore.ttf");
	
	//Create some text
	sf::Text text;
	text.setFont(font);
	text.setPosition(sf::Vector2f(32.f, 32.f));
	text.setCharacterSize(10);
	text.setColor(sf::Color::White);
	
	while (window.isOpen())
	{
		//-----BALL-----
		for (int count = 0; count < ball_number; count ++)
			ball[count].step(bat_l, bat_r, points[0], points[1]);
		
		//-----BAT LEFT-----
		bat_l.step(ball[0].pos.y + ball[0].circle.getRadius());
		
		//Get keyboard
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			bat_l.speed.y -= 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			bat_l.speed.y += 1;
		
		//-----BAT RIGHT-----
		bat_r.step(ball[0].pos.y + ball[0].circle.getRadius());
		
		//Get keyboard
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			bat_r.speed.y -= 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			bat_r.speed.y += 1;
		
		//Loop events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//Draw everything
		window.clear();
		
		text.setString(intToStr(points[0]) + ":" + intToStr(points[1]));
		text.setPosition(sf::Vector2f(window.getSize().x / 2 - text.getGlobalBounds().width / 2, 32));
		window.draw(text);
		
		window.draw(bat_l.rect);
		window.draw(bat_r.rect);
		for (int count = 0; count < ball_number; count ++)
			window.draw(ball[count].circle);
		window.display();
	}
	
	return 0;
}
