#pragma once

namespace
{
    const int c_WINDOW_HEIGHT = 400;
    const int c_WINDOW_WIDTH = 800;
    const int c_PADDLE_WIDTH = 10;
    const int c_PADDLE_HEIGHT = 120;
    const int c_LEFT_PADDLE_X = 5;
    const int c_RIGHT_PADDLE_X = c_WINDOW_WIDTH - (c_LEFT_PADDLE_X + c_PADDLE_WIDTH);
    const int c_BALL_SIZE = 10;
}


namespace Pong
{
    enum Buttons
    {
        LeftUp = 0,
        LeftDown,
        RightUp,
        RightDown
    };


    class Shape
    {
    public:
        sf::Vector2f position, velocity;
        Shape(sf::Vector2f position, sf::Vector2f velocity);
        void draw(sf::RenderWindow& window, sf::RectangleShape rect);
        void update(float dt);
    };

    class Paddle : public Shape
    {
    public:
        using Shape::Shape;
        void update(float dt);
    };

    class Ball : public Shape
    {
    public:
        using Shape::Shape;
    };

    namespace CollisionHandling
    {
        enum class Contact {
            Top,
            Middle,
            Bottom,
            Left,
            Right,
            None,
        };

        Contact checkPaddleCollision(const Ball& ball, const Paddle& paddle);
        Contact checkWallCollision(const Ball& ball);
        Contact handleCollission(const Ball& ball, const Paddle& left_paddle, const Paddle& right_paddle);
    }
    void handleInput(sf::Window& window, std::array<bool, 4>& keys);

    void drawNet(sf::RenderWindow& window, sf::RectangleShape rect, int length);

    std::tuple<Ball, Paddle, Paddle> initShapes();
}
