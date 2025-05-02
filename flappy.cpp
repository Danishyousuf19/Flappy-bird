#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = -10.f;
const float PIPE_SPEED = 3.f;
const int PIPE_WIDTH = 70;
const int PIPE_GAP = 150;
const int NUM_PIPES = 3;
const int PIPE_SPACING = 300;

// Pipe structure
struct Pipe {
    RectangleShape top, bottom;
    float x;

    Pipe(int xPos) {
        x = xPos;
        int heightTop = rand() % (WINDOW_HEIGHT - PIPE_GAP - 100) + 50;
        int heightBottom = WINDOW_HEIGHT - heightTop - PIPE_GAP;

        top.setSize(Vector2f(PIPE_WIDTH, heightTop));
        bottom.setSize(Vector2f(PIPE_WIDTH, heightBottom));
        top.setFillColor(Color::Green);
        bottom.setFillColor(Color::Green);
        top.setPosition(x, 0);
        bottom.setPosition(x, heightTop + PIPE_GAP);
    }

    void move() {
        x -= PIPE_SPEED;
        top.setPosition(x, 0);
        bottom.setPosition(x, top.getSize().y + PIPE_GAP);
    }
};

int main() {
    srand(time(nullptr));

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird Clone");
    window.setFramerateLimit(60);

    // Load bird texture
    Texture birdTexture;
    if (!birdTexture.loadFromFile("flappy.png")) {
        cerr << "Error: Unable to load bird texture!" << endl;
        return -1;
    }
    Sprite bird(birdTexture);
    bird.setPosition(100, WINDOW_HEIGHT / 2);

    // Load font for Game Over
    Font font;
    if (!font.loadFromFile("KOMIKAP.ttf")) {
        cerr << "Error: Unable to load font!" << endl;
        return -1;
    }
    Text gameOverText("Game Over! Press Space to Restart", font, 30);
    gameOverText.setFillColor(Color::Red);
    FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition((WINDOW_WIDTH - textBounds.width) / 2,
                             (WINDOW_HEIGHT - textBounds.height) / 2);

    float velocity = 0.f;
    vector<Pipe> pipes;
    bool gameOver = false;

    // Function to reset game state
    auto resetGame = [&]() {
        bird.setPosition(100, WINDOW_HEIGHT / 2);
        velocity = 0.f;
        pipes.clear();
        for (int i = 0; i < NUM_PIPES; ++i) {
            pipes.emplace_back(WINDOW_WIDTH + i * PIPE_SPACING);
        }
        gameOver = false;
    };

    resetGame();

    // Game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
                if (gameOver) {
                    resetGame();
                } else {
                    velocity = JUMP_STRENGTH;
                }
            }
        }

        if (!gameOver) {
            // Apply gravity and move bird
            velocity += GRAVITY;
            bird.move(0, velocity);

            // Move pipes
            for (auto &p : pipes) {
                p.move();
            }

            // Recycle pipes
            if (!pipes.empty() && pipes.front().x < -PIPE_WIDTH) {
                pipes.erase(pipes.begin());
                int newX = static_cast<int>(pipes.back().x) + PIPE_SPACING;
                pipes.emplace_back(newX);
            }

            // Collision/out of bounds check
            FloatRect birdBounds = bird.getGlobalBounds();
            if (birdBounds.top < 0 || birdBounds.top + birdBounds.height > WINDOW_HEIGHT) {
                gameOver = true;
            }
            for (auto &p : pipes) {
                if (birdBounds.intersects(p.top.getGlobalBounds()) ||
                    birdBounds.intersects(p.bottom.getGlobalBounds())) {
                    gameOver = true;
                    break;
                }
            }
        }

        // Draw everything
        window.clear(Color::Cyan);
        window.draw(bird);
        for (auto &p : pipes) {
            window.draw(p.top);
            window.draw(p.bottom);
        }

        if (gameOver) {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
