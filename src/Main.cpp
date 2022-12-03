#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <format>
#include <array>
#include "../include/Main.hpp"


namespace Pong
{
    void drawNet(sf::RenderWindow& window, sf::RectangleShape rect, int length)
    {
        const int steps = c_WINDOW_HEIGHT / length;
        const int middle = c_WINDOW_WIDTH / 2;
        for (int i = 0; i < steps; i++)
        {
            rect.setPosition(middle, i * length);
            window.draw(rect);
        }
    }

    Shape::Shape(sf::Vector2f position, sf::Vector2f velocity) :
        position(position), velocity(velocity)
    {
    };

    void Shape::draw(sf::RenderWindow& window, sf::RectangleShape rect)
    {
        rect.setPosition(position);
        window.draw(rect);
    }

    void Shape::update(float dt)
    {
        position += velocity * dt;
    }

    void Paddle::update(float dt)
    {
        Shape::update(dt);
        if (position.y < 0)
            position.y = 0;
        if (position.y > (c_WINDOW_HEIGHT - c_PADDLE_HEIGHT))
            position.y = c_WINDOW_HEIGHT - c_PADDLE_HEIGHT;

    }



    void handleInput(sf::Window& window, std::array<bool, 4>& keys)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                    keys[Pong::Buttons::LeftDown] = true;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                    keys[Pong::Buttons::LeftUp] = true;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    keys[Pong::Buttons::RightDown] = true;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    keys[Pong::Buttons::RightUp] = true;
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::S)
                    keys[Pong::Buttons::LeftDown] = false;
                if (event.key.code == sf::Keyboard::W)
                    keys[Pong::Buttons::LeftUp] = false;
                if (event.key.code == sf::Keyboard::Down)
                    keys[Pong::Buttons::RightDown] = false;
                if (event.key.code == sf::Keyboard::Up)
                    keys[Pong::Buttons::RightUp] = false;
            }
        }
        
    }

    namespace CollisionHandling
    {
        Contact checkPaddleCollision(const Ball& ball, const Paddle& paddle)
        {
            // calculate x and y levels of Shapes
            float ballTop = ball.position.y;
            float ballBottom = ball.position.y + c_BALL_SIZE;
            float ballLeft = ball.position.x;
            float ballRight = ball.position.x + c_BALL_SIZE;
            
            float paddleTop = paddle.position.y;
            float paddleBottom = paddle.position.y + c_PADDLE_HEIGHT;
            float paddleLeft = paddle.position.x;
            float paddleRight = paddle.position.x + c_PADDLE_WIDTH;

            if (ballBottom <= paddleTop)
                return Contact::None;
            if (ballTop >= paddleBottom)
                return Contact::None;
            if (ballRight <= paddleLeft)
                return Contact::None;
            if (ballLeft >= paddleRight)
			return Contact::None;

			float partitionSize = c_PADDLE_HEIGHT / 3;
			float topPartitionUpperBound = paddle.position.y;
			float middlePartitionUpperBound = topPartitionUpperBound + partitionSize;
			float bottomPartitionUpperBound = middlePartitionUpperBound + partitionSize;

            if (ballBottom < middlePartitionUpperBound)
                return Contact::Top;
            else if (ballBottom >= middlePartitionUpperBound && ballBottom < bottomPartitionUpperBound)
                return Contact::Middle;
            else 
                return Contact::Bottom;

        }


        Contact checkWallCollision(const Ball& ball)
        {

            float ballTop = ball.position.y;
            float ballBottom = ball.position.y + c_BALL_SIZE;
            float ballLeft = ball.position.x;
            float ballRight = ball.position.x + c_BALL_SIZE;

            if (ballTop < 0)
                return Contact::Top;
            if (ballBottom > c_WINDOW_HEIGHT)
                return Contact::Bottom;
            if (ballLeft < 0)
                return Contact::Left;
            if (ballRight > c_WINDOW_WIDTH)
                return Contact::Right;
            return Contact::None;

        }


        Contact handleCollission(Ball& ball, const Paddle& left_paddle, const Paddle& right_paddle)
        {
            auto paddle_contact = ball.velocity.x < 0 ? checkPaddleCollision(ball, left_paddle) :
                                                                               checkPaddleCollision(ball, right_paddle);
            if (paddle_contact != Contact::None)
            {
                ball.velocity.x = -ball.velocity.x;
				switch (paddle_contact)
				{
				case Contact::Top:
					ball.velocity.y = -0.75;
					break;
				case Contact::Middle:
					ball.velocity.y = 0;
					break;
				case Contact::Bottom:
					ball.velocity.y = 0.75;
					break;
				}
                return Contact::None;
            }
            auto wall_contact = checkWallCollision(ball);
            if (wall_contact != Contact::None)
            {
                switch (wall_contact)
                {
                case Contact::Top:
                    ball.velocity.y = 0.75;
                    break;
                case Contact::Bottom:
                    ball.velocity.y = -0.75;
                    break;
                case Contact::Left:
                    return Contact::Left;
                case Contact::Right:
                    return Contact::Right;
                }
            }
            return Contact::None;
        }



    }

    std::tuple<Ball, Paddle,Paddle> initShapes() {
        return { Ball({ c_WINDOW_WIDTH / 2, c_WINDOW_HEIGHT / 2 }, {0.5,0}),
                Paddle({ c_RIGHT_PADDLE_X, c_WINDOW_HEIGHT / 2 - (c_PADDLE_HEIGHT / 2)}, { 0,0 }),
                Paddle({c_LEFT_PADDLE_X, c_WINDOW_HEIGHT / 2 - (c_PADDLE_HEIGHT / 2)}, {0,0}) };
    }

}

int main()
{
    sf::RenderWindow window(sf::VideoMode(c_WINDOW_WIDTH, c_WINDOW_HEIGHT), "Pong");

    const int length{ 20 };
    sf::RectangleShape net_rect({ 3,length });
    net_rect.setOutlineThickness(3);
    net_rect.setOutlineColor(sf::Color::Black);
    net_rect.setFillColor(sf::Color::White);

    sf::RectangleShape paddle_rect({ c_PADDLE_WIDTH,c_PADDLE_HEIGHT });
    paddle_rect.setFillColor(sf::Color::White);

    sf::RectangleShape ball_rect({ c_BALL_SIZE, c_BALL_SIZE});
    ball_rect.setFillColor(sf::Color::White);

    auto [ball, right_paddle, left_paddle] = Pong::initShapes();

    const float paddle_speed = 1;
    int score_left = 0;
    int score_right = 0;

    std::array<bool, 4> keys{};

    float dt = 0.0f;

    while (window.isOpen())
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Pong::handleInput(window, keys);

        if (keys[Pong::Buttons::LeftUp])
            left_paddle.velocity.y = -paddle_speed;
        else if (keys[Pong::Buttons::LeftDown])
            left_paddle.velocity.y = paddle_speed;
        else left_paddle.velocity.y = 0;

        if (keys[Pong::Buttons::RightUp])
            right_paddle.velocity.y = -paddle_speed;
        else if (keys[Pong::Buttons::RightDown])
            right_paddle.velocity.y = paddle_speed;
        else right_paddle.velocity.y = 0;


        left_paddle.update(dt);
        right_paddle.update(dt);
        ball.update(dt);


        auto collision = Pong::CollisionHandling::handleCollission(ball, left_paddle, right_paddle);

        switch (collision)
        {
        case Pong::CollisionHandling::Contact::Left:
            score_right++;
        case Pong::CollisionHandling::Contact::Right:
            score_right++;
        std::tie(ball, right_paddle, left_paddle) = Pong::initShapes();
        }




        window.clear();
        Pong::drawNet(window, net_rect, length);
        left_paddle.draw(window, paddle_rect);
        right_paddle.draw(window, paddle_rect);
        ball.draw(window, ball_rect);
        window.display();

        auto stopTime = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
    }

    return 0;
}