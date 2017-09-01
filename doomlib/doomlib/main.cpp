
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "tgaimage.h"

#include "w_doom.hpp"
#include "z_doom.hpp"
#include "doom_video.hpp"

#include <cassert>
#include <cstdarg>
#include <Windows.h>
#include "linedrawing.h"


static constexpr bool islineending(int c) { return c == '\r' || c == '\n'; }
namespace doom_cpp {
	template<size_t N>
	size_t _Print(char (&buffer)[N], const char* fmt, va_list va) {
		int len = vsnprintf(buffer, N-1, fmt, va);
		assert(len > 3 && len < 1000);
		while (len && islineending(buffer[len - 1])) len--; // trim line endings off
		buffer[len++] = '\r';
		buffer[len++] = '\n';
		buffer[len++] = 0;
		return size_t(len);
	}
	void I_Print(const char* fmt, ...) {
		char buffer[1024];
		va_list va;
		va_start(va, fmt);
		_Print(buffer, fmt, va);
		va_end(va);
		OutputDebugString(buffer);
		fputs(buffer, stderr);
	}
	void I_Error(const char* fmt, ...) {
		char buffer[1024];
		va_list va;
		va_start(va, fmt);
		_Print(buffer, fmt, va);
		va_end(va);
		OutputDebugString(buffer);
		fputs(buffer, stderr);
		while (1) {} // debug
		assert(0);// die
		// error interface
	}


}
static constexpr size_t total_doom_memory = 8000000U;
static constexpr size_t screen_width = 640;
static constexpr size_t screen_height = 480;
uint8_t s_doom_memory[total_doom_memory];
//static std::vector<uint8_t> doom_memory;
class SFML_framebuffer {
	sf::Texture _texture;
	sf::Image _image;
public:
	SFML_framebuffer(size_t width, size_t height) { _image.create(width, height);  _texture.create(width, height);}
	void set(size_t x, size_t y, const TGAColor& v) {
		sf::Color c(v.r, v.g, v.b, v.a);
		_image.setPixel(x, y, c); 

		
	}
	sf::Texture& getTexture() {
		_texture.loadFromImage(_image);
		return _texture;
	}

	size_t get_width() const { return _image.getSize().x; }
	size_t get_height() const { return _image.getSize().x; }
};
static doom_cpp::Video<320, 200> screen;
void doom_start() {
	// because of scope, go here
	doom_cpp::WadLoader loader;
	assert(loader.loadfile("DOOM.WAD"));
	loader.debug_list_lumps();
}
#include "our_gl.h"
TinyTest<float> test(640, 480);
uint32_t* blackbox_refresh();
void blackbox_init(size_t width, size_t height);

#include "bitmap_view.h"
////////////////////////////////////////////////////////////
/// Function called whenever one of our windows receives a message
///
////////////////////////////////////////////////////////////
CBitmap m_bmp;
HWND button;


LRESULT CALLBACK onEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Quit when we close the main window
	case WM_CREATE:
		m_bmp.CreateCompatibleBitmap(GetDC(handle), screen_width, screen_height);
		break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Quit when we click the "quit" button
	case WM_COMMAND:
	{
		if (reinterpret_cast<HWND>(lParam) == button)
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	case WM_PAINT:


		break;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}


void tiny_test() {
	sf::Texture texture_out;
	sf::Image screen_image;
	sf::Image texutre_image;
	assert(texutre_image.loadFromFile("thing.png"));
	screen_image.create(screen_width, screen_height);

	blackbox_init(screen_width, screen_height);
#if 0

	XForm<float> xtest;
	xtest.AppendRotationX(32);
	render_texture<color_t> screen(const_cast<uint8_t*>(screen_image.getPixelsPtr()), screen_width, screen_height);
	render_texture<color_t> texture;
	screen.setup(const_cast<uint8_t*>(screen_image.getPixelsPtr()), screen_width, screen_height);

	std::vector<Vertex> verts;

	auto edge = CreateEdge(verts.begin(), verts.end(), verts.end());
	edge.setup(verts.begin());
#endif
	test.init();
	test.load("diablo3_pose.obj");

	SFML_framebuffer image(640, 480);
	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "SFML Pong",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	sf::Sprite sprite;
	sf::Clock clock; // starts the clock
	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}
		}
		sf::Time elapsed1 = clock.getElapsedTime();
		if (elapsed1.asMilliseconds() > 10)
			clock.restart();
		//std::cout << elapsed1.asSeconds() << std::endl;


		clock.restart();
		// Clear the window
		window.clear(sf::Color(50, 200, 50));
#if 0
		test.render(image);

#else
		blackbox_refresh();
		texture_out.loadFromImage(screen_image);
		sprite.setTexture(texture_out);
#endif
		window.draw(sprite);

		// Display things on screen
		window.display();
	}

}

// main.cpp:
CComModule _Module;

int simple_window() {
	blackbox_init(screen_width, screen_height);
	HINSTANCE instance = GetModuleHandle(NULL);
	_Module.Init(NULL, instance);
	CBitmapWindow wndMain;
	MSG msg;
	
	// Create & show our main window
	if (NULL == wndMain.Create(NULL, CWindow::rcDefault,
		_T("My First ATL Window")))
	{
		// Bad news, window creation failed
		return 1;
	}
	msg.message = static_cast<UINT>(~WM_QUIT);
	wndMain.ShowWindow(1);
	wndMain.UpdateWindow();
	wndMain.CreateBitmap(screen_width, screen_height);
	// Run the message loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If a message was waiting in the message queue, process it
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// idle stuff
			wndMain.Update(blackbox_refresh());
		}
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	blackbox_refresh();
	_Module.Term();
	return msg.wParam;


	// Define a class for our main window
	WNDCLASS windowClass;
	windowClass.style = 0;
	windowClass.lpfnWndProc = &onEvent;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = instance;
	windowClass.hIcon = NULL;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = TEXT("SFML App");
	RegisterClass(&windowClass);

	// Let's create the main window
	HWND window = CreateWindow(TEXT("SFML App"), TEXT("SFML Win32"), WS_SYSMENU | WS_VISIBLE, 200, 200, 660, 520, NULL, NULL, instance, NULL);
	// Add a button for exiting
	button = CreateWindow(TEXT("BUTTON"), TEXT("Quit"), WS_CHILD | WS_VISIBLE, 560, 440, 80, 40, window, NULL, instance, NULL);

	// Loop until a WM_QUIT message is received
	MSG message;
	message.message = static_cast<UINT>(~WM_QUIT);
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			// If a message was waiting in the message queue, process it
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
#if 0
			float time = clock.getElapsedTime().asSeconds();

			// Clear views
			SFMLView1.clear();
			SFMLView2.clear();

			// Draw sprite 1 on view 1
			sprite1.setRotation(time * 100);
			SFMLView1.draw(sprite1);

			// Draw sprite 2 on view 2
			sprite2.setPosition(std::cos(time) * 100.f, 0.f);
			SFMLView2.draw(sprite2);

			// Display each view on screen
			SFMLView1.display();
			SFMLView2.display();
#endif
		}
	}

	// Destroy the main window (all its child controls will be destroyed)
	DestroyWindow(window);

	// Don't forget to unregister the window class
	UnregisterClass(TEXT("SFML App"), instance);
	return 0;
}

int main(int argc, const char* argv[]) {

	//doom_memory.resize(total_doom_memory); // 8 megs
	doom_cpp::Z_Init(s_doom_memory, total_doom_memory);
	//doom_cpp::Z_FileDumpHeap(stderr);
//	doom_start();
	vector_ops::Vec<3, int> test1 = { 4 ,3 };
	vector_ops::Vec<8, int> test2 = { 2,2,2,2 };
	vector_ops::Vec<12, int> combine = test1 + test2;
	assert(simple_window() == 0);
	//tiny_test();
	while (1) {}

#if 0

	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Define some constants
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const int gameHeight = 600;
	sf::Vector2f paddleSize(25, 100);
	float ballRadius = 10.f;

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	// Load the sounds used in the game
	sf::SoundBuffer ballSoundBuffer;
	if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
		return EXIT_FAILURE;
	sf::Sound ballSound(ballSoundBuffer);

	// Create the left paddle
	sf::RectangleShape leftPaddle;
	leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
	leftPaddle.setOutlineThickness(3);
	leftPaddle.setOutlineColor(sf::Color::Black);
	leftPaddle.setFillColor(sf::Color(100, 100, 200));
	leftPaddle.setOrigin(paddleSize / 2.f);

	// Create the right paddle
	sf::RectangleShape rightPaddle;
	rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
	rightPaddle.setOutlineThickness(3);
	rightPaddle.setOutlineColor(sf::Color::Black);
	rightPaddle.setFillColor(sf::Color(200, 100, 100));
	rightPaddle.setOrigin(paddleSize / 2.f);

	// Create the ball
	sf::CircleShape ball;
	ball.setRadius(ballRadius - 3);
	ball.setOutlineThickness(3);
	ball.setOutlineColor(sf::Color::Black);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(ballRadius / 2, ballRadius / 2);

	// Load the text font
	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf"))
		return EXIT_FAILURE;

	// Initialize the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(40);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");

	// Define the paddles properties
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(0.1f);
	const float paddleSpeed = 400.f;
	float rightPaddleSpeed = 0.f;
	const float ballSpeed = 400.f;
	float ballAngle = 0.f; // to be changed later

	sf::Clock clock;
	bool isPlaying = false;
	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				if (!isPlaying)
				{
					// (re)start the game
					isPlaying = true;
					clock.restart();

					// Reset the position of the paddles and ball
					leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
					rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
					ball.setPosition(gameWidth / 2, gameHeight / 2);

					// Reset the ball angle
					do
					{
						// Make sure the ball initial angle is not too much vertical
						ballAngle = (std::rand() % 360) * 2 * pi / 360;
					} while (std::abs(std::cos(ballAngle)) < 0.7f);
				}
			}
		}

		if (isPlaying)
		{
			float deltaTime = clock.restart().asSeconds();

			// Move the player's paddle
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
				(leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
			{
				leftPaddle.move(0.f, -paddleSpeed * deltaTime);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
				(leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
			{
				leftPaddle.move(0.f, paddleSpeed * deltaTime);
			}

			// Move the computer's paddle
			if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
				((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
			{
				rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);
			}

			// Update the computer's paddle direction according to the ball position
			if (AITimer.getElapsedTime() > AITime)
			{
				AITimer.restart();
				if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
					rightPaddleSpeed = paddleSpeed;
				else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
					rightPaddleSpeed = -paddleSpeed;
				else
					rightPaddleSpeed = 0.f;
			}

			// Move the ball
			float factor = ballSpeed * deltaTime;
			ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

			// Check collisions between the ball and the screen
			if (ball.getPosition().x - ballRadius < 0.f)
			{
				isPlaying = false;
				pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
			}
			if (ball.getPosition().x + ballRadius > gameWidth)
			{
				isPlaying = false;
				pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
			}
			if (ball.getPosition().y - ballRadius < 0.f)
			{
				ballSound.play();
				ballAngle = -ballAngle;
				ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
			}
			if (ball.getPosition().y + ballRadius > gameHeight)
			{
				ballSound.play();
				ballAngle = -ballAngle;
				ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
			}

			// Check the collisions between the ball and the paddles
			// Left Paddle
			if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 &&
				ball.getPosition().x - ballRadius > leftPaddle.getPosition().x &&
				ball.getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
				ball.getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.getPosition().y > leftPaddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

				ballSound.play();
				ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
			}

			// Right Paddle
			if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
				ball.getPosition().x + ballRadius < rightPaddle.getPosition().x &&
				ball.getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
				ball.getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.getPosition().y > rightPaddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

				ballSound.play();
				ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
			}
		}

		// Clear the window
		window.clear(sf::Color(50, 200, 50));

		if (isPlaying)
		{
			// Draw the paddles and the ball
			window.draw(leftPaddle);
			window.draw(rightPaddle);
			window.draw(ball);
		}
		else
		{
			// Draw the pause message
			window.draw(pauseMessage);
		}

		// Display things on screen
		window.display();
	}
#endif
	return EXIT_SUCCESS;
}