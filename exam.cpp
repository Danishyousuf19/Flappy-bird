#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float GRAVITY = 0.12f;
const float JUMP_STRENGTH = -4.f;
const float PIPE_SPEED = 1.8f;
const int PIPE_WIDTH = 70;
const int PIPE_GAP = 150;
const int NUM_PIPES = 3;
const int PIPE_SPACING = 300;

// Pipe structure
struct Pipe {
    RectangleShape top, bottom;
    float x;
    bool scored = false; // to avoid scoring same pipe multiple times

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
    srand(static_cast<unsigned int>(time(nullptr)));

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird Clone");
    window.setFramerateLimit(60);

    // Load bird texture
    Texture birdTexture;
    if (!birdTexture.loadFromFile("be.png")) {
        cerr << "Error: Unable to load bird texture!" << endl;
        return -1;
    }
    Sprite bird(birdTexture);
    bird.setPosition(100, WINDOW_HEIGHT / 2);
    bird.setScale(0.12f, 0.12f);

    // Load font
    Font font;
    if (!font.loadFromFile("pdark.ttf")) {
        cerr << "Error: Unable to load font!" << endl;
        return -1;
    }

    // Game over text
    Text gameOverText("Press Space to Restart", font, 30);
    gameOverText.setFillColor(Color::Red);
    FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition((WINDOW_WIDTH - textBounds.width) / 2,
                             (WINDOW_HEIGHT - textBounds.height) / 2);

    // Score text
    int score = 0;
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20, 20);
    scoreText.setString("Score 0");

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
        score = 0;
        scoreText.setString("Score 0");
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

            // Move pipes and check for score
            for (auto& p : pipes) {
                p.move();
                if (!p.scored && (p.x + PIPE_WIDTH) < (bird.getPosition().x - 10)) {
                    p.scored = true;
                    score++;
                    scoreText.setString("Score " + to_string(score));
                }
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
            for (auto& p : pipes) {
                if (birdBounds.intersects(p.top.getGlobalBounds()) ||
                    birdBounds.intersects(p.bottom.getGlobalBounds())) {
                    gameOver = true;
                    break;
                }
            }
        }

        // Draw everything
        window.clear();
        window.draw(bird);
        for (auto& p : pipes) {
            window.draw(p.top);
            window.draw(p.bottom);
        }
        if (gameOver) {
            window.draw(gameOverText);
        }
        window.draw(scoreText);
        window.display();
    }

    return 0;
}
