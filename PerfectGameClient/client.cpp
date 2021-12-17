#include <iostream>
#include <memory>
#include <chrono>
#include<vector>
#include "UdpSocket.h"
#include "../GameState/GameState.h"
#include "SFML/Graphics.hpp"
using namespace sf;
//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
//The port on which to listen for incoming data
u_short const kPort = 8888;
size_t const kBufferSize = 512;
char buffer[kBufferSize];

void sleep(unsigned long us)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	while (microseconds.count() < us)
	{
		finish = std::chrono::high_resolution_clock::now();
		microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	}
}
class drawPlayer {

	std::vector<Image> imageTextures;

	std::map<std::string, Texture> textureOfPlayer;
	std::map<std::string, Sprite> spriteOfPlayer;
public:
	drawPlayer() {}
	drawPlayer(GameState& state, size_t countOfCharacter, std::string Texture_Folder) {
		imageTextures.resize(countOfCharacter);

		
		

	}
        Sprite setSprite(Texture& texture, size_t x = 0, size_t y = 0) {
		Sprite sprite;
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(x, y, 75, 100));
		return sprite;
	}

	

	void Draw(RenderWindow& window, GameState& state, size_t move = 0) {
		for (auto& map_pair : state.getListOfPlayers()) {
			if (map_pair.second.getStatus() == PlayerStatus::Active) {
				std::cout << '\n' << map_pair.second.getName() << " : x = " <<
					int(map_pair.second.getPos().first) << " y = " << int(map_pair.second.getPos().second);
				spriteOfPlayer[map_pair.second.getName()].
					setPosition(map_pair.second.getPos().first - 10, map_pair.second.getPos().second - 50); 

				
			}
		}
	}

	void updateDC(GameState& state, size_t countOfCharacter, std::string Texture_Folder) {
		for (auto& map_pair : state.getListOfPlayers()) {
			std::string tmp = map_pair.second.getName();
			if (!textureOfPlayer.count(tmp)) {
				Texture texture;
				srand(time(NULL));
				
				textureOfPlayer.insert(std::make_pair(tmp, texture));
				spriteOfPlayer.insert(std::make_pair(tmp, setSprite(textureOfPlayer[map_pair.first])));
			}
		}
	}
};

drawPlayer player;
	
	

sf::RenderWindow window;
sf::RectangleShape rectangle(sf::Vector2f(150, 70));
sf::CircleShape circle1(25, 30);
sf::CircleShape circle2(25, 30);
sf::Image image;
sf::Texture texture;
sf::Sprite sprite;
float speed = 0.1;
float CurrentFrame = 0;
int HeroDirection;




int main(int argc, const char* argv[])
{
	size_t sizeOfBlocks;
	std::cout << "Enter size of blocks (max_size = 200) : ";
	std::cin >> sizeOfBlocks;
	std::unique_ptr<UdpSocket> sock_ptr;
	std::string name = argv[1];
	GameState state;
	sf::RenderWindow window;
	//sf::Window window;
	window.create(sf::VideoMode(640, 480), "PerfectGame");
	window.setFramerateLimit(60);

	rectangle.setFillColor(sf::Color::Green);
	rectangle.setPosition(200, 200);

	circle1.setFillColor(sf::Color::Green);
	circle1.setPosition(300, 300);

	circle2.setFillColor(sf::Color::Green);
	circle2.setPosition(400, 300);



	image.loadFromFile("5DU3U.PNG");
	texture.loadFromImage(image);
	sprite.setTexture(texture);
	sprite.setPosition(50, 50);
	sprite.setTextureRect(sf::IntRect(0, 0, 64, 96));



	try
	{
		sock_ptr = std::make_unique<UdpSocket>(kIpAddr, kPort);
	}
	catch (std::exception const& err)
	{
		std::cout << "Couldn't init socket: " << err.what() << "\n";
		exit(EXIT_FAILURE);
	}

	//start communication
	//send the message
	if (sock_ptr->send(name.c_str(), name.length()) != 0)
	{
		std::cout << "Failed to send\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "request sent\n";

	while (1) {
		size_t sz = kBufferSize;
		if (sock_ptr->recv(buffer, sz) != 0)
		{
			std::cout << "No data to recv\n";
			sleep(1e5);
			continue;
			exit(EXIT_FAILURE);
		}
		state.deserialize(buffer, sz);
		break;
	}

	sf::Clock clock;
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;
		
		//receive a reply and print it
		size_t sz = kBufferSize;

		if (sock_ptr->recv(buffer, sz) != 0)
		{
			std::cout << "No data to recv\n";
			sleep(1e5);
			continue;
			exit(EXIT_FAILURE);
		}


		std::cout << "Received game state: " << sz << "\n";
		state.deserialize(buffer, sz);
		Player* p = state.getPlayer(name);
		PlayerPos curr_pos = p->getPos();
		std::cout << "Received player pos: (";
		std::cout << (int)curr_pos.first << "," << (int)curr_pos.second << ")\n";
		++curr_pos.second;
		++curr_pos.first;
		p->updatePos(curr_pos.first, curr_pos.second);

		Event event;

		if (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				HeroDirection = 0;
				CurrentFrame += 0.005 * time;
				std::cout << CurrentFrame << std::endl;
				if (CurrentFrame > 4) CurrentFrame -= 4;
                sprite.setTextureRect(sf::IntRect(int(CurrentFrame) * 64, 96, 64, 96));
				sprite.move(-speed * time, 0);
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first - speed , curr_pos.second );

			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				HeroDirection = 1;
				CurrentFrame += 0.005 * time;
				std::cout << CurrentFrame << std::endl;
				if (CurrentFrame > 4) CurrentFrame -= 4;
				sprite.setTextureRect(sf::IntRect(int(CurrentFrame) * 64, 192, 64, 96));
				sprite.move(speed * time, 0);
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first + speed, curr_pos.second);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				HeroDirection = 2;
				CurrentFrame += 0.005 * time;
				std::cout << CurrentFrame << std::endl;
				if (CurrentFrame > 4) CurrentFrame -= 4;
				sprite.setTextureRect(sf::IntRect(int(CurrentFrame) * 64, 288, 64, 96));
				sprite.move(0, -speed * time);
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first, curr_pos.second - speed);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				HeroDirection = 3;
				CurrentFrame += 0.005 * time;
				std::cout << CurrentFrame << std::endl;
				if (CurrentFrame > 4) CurrentFrame -= 4;
				sprite.setTextureRect(sf::IntRect(int(CurrentFrame) * 64, 0, 64, 96));
				sprite.move(0, speed * time);
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first, curr_pos.second +speed );
			}


			sf::FloatRect spriteBounds = sprite.getGlobalBounds();
			sf::FloatRect rectangleBounds = rectangle.getGlobalBounds();
			sf::FloatRect circle1Bounds = circle1.getGlobalBounds();
			sf::FloatRect circle2Bounds = circle2.getGlobalBounds();

			if (spriteBounds.intersects(rectangleBounds))
			{
				if (HeroDirection == 0)
					sprite.move(speed * time, 0);
				if (HeroDirection == 1)
					sprite.move(-speed * time, 0);
				if (HeroDirection == 2)
					sprite.move(0, speed * time);
				if (HeroDirection == 3)
					sprite.move(0, -speed * time);

			}
			if (spriteBounds.intersects(circle1Bounds))
				circle1.setFillColor(sf::Color::Black);
			if (spriteBounds.intersects(circle2Bounds)) {
				if (HeroDirection == 0)
					circle2.move(-speed * time, 0);
				if (HeroDirection == 1)
					circle2.move(speed * time, 0);
				if (HeroDirection == 2)
					circle2.move(0, -speed * time);
				if (HeroDirection == 3)
					circle2.move(0, speed * time);
			}
		}
		drawPlayer();
		window.clear(sf::Color::Black);
	    window.draw(rectangle);
		window.draw(circle1);
		window.draw(circle2);
		window.draw(sprite);
		window.display();
		sz = kBufferSize;
		p->serialize(buffer, sz);

		if (sock_ptr->send(buffer, sz) != 0)
		{
			std::cout << "Failed to send pos\n";
			exit(EXIT_FAILURE);
		}

	}
	return 0;
}


 
      