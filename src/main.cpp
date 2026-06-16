#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;
const int CELL = 4;
const int COLS = WIDTH / CELL;
const int ROWS = HEIGHT / CELL;

enum Material {
    EMPTY = 0,
    SAND,
    WATER,
    FIRE,
    SMOKE,
    STONE,
    WOOD,
    LAVA
};

struct Cell {
    Material mat = EMPTY;
    bool updated = false;
    float velocity = 0.f;
    int lifetime = 0;
};

Cell grid[COLS][ROWS];
Cell nextGrid[COLS][ROWS];

sf::Color getColor(Material m, int lifetime = 0) {
    switch (m) {
        case SAND:  return sf::Color(194, 178, 128);
        case WATER: return sf::Color(30, 100, 200, 180);
        case FIRE: {
            int r = 255;
            int g = std::max(0, 100 - lifetime * 2);
            int b = 0;
            return sf::Color(r, g, b);
        }
        case SMOKE: return sf::Color(100 + rand()%30, 100 + rand()%30, 100 + rand()%30, 150);
        case STONE: return sf::Color(120, 120, 120);
        case WOOD:  return sf::Color(101, 67, 33);
        case LAVA:  return sf::Color(207, 16 + rand()%40, 32);
        default:    return sf::Color(15, 15, 20);
    }
}

bool inBounds(int x, int y) {
    return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

bool isEmpty(int x, int y) {
    return inBounds(x, y) && grid[x][y].mat == EMPTY;
}

bool isLiquid(int x, int y) {
    return inBounds(x, y) && (grid[x][y].mat == WATER || grid[x][y].mat == LAVA);
}

void updateSand(int x, int y) {
    if (isEmpty(x, y+1)) {
        nextGrid[x][y+1] = grid[x][y];
        nextGrid[x][y].mat = EMPTY;
    } else if (isLiquid(x, y+1)) {
        std::swap(nextGrid[x][y], nextGrid[x][y+1]);
    } else {
        int dir = (rand() % 2) ? 1 : -1;
        if (isEmpty(x+dir, y+1)) {
            nextGrid[x+dir][y+1] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        } else if (isEmpty(x-dir, y+1)) {
            nextGrid[x-dir][y+1] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        }
    }
}

void updateWater(int x, int y) {
    if (isEmpty(x, y+1)) {
        nextGrid[x][y+1] = grid[x][y];
        nextGrid[x][y].mat = EMPTY;
    } else {
        int dir = (rand() % 2) ? 1 : -1;
        if (isEmpty(x+dir, y)) {
            nextGrid[x+dir][y] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        } else if (isEmpty(x-dir, y)) {
            nextGrid[x-dir][y] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        } else if (isEmpty(x+dir, y+1)) {
            nextGrid[x+dir][y+1] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        } else if (isEmpty(x-dir, y+1)) {
            nextGrid[x-dir][y+1] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        }
    }
}

void updateFire(int x, int y) {
    nextGrid[x][y].lifetime++;
    if (nextGrid[x][y].lifetime > 30 + rand() % 20) {
        if (rand() % 2 == 0 && isEmpty(x, y-1)) {
            nextGrid[x][y-1] = {SMOKE, false, 0, 0};
        }
        nextGrid[x][y].mat = EMPTY;
        return;
    }
    // spread fire to wood
    int dx[] = {0,0,1,-1};
    int dy[] = {1,-1,0,0};
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (inBounds(nx, ny) && grid[nx][ny].mat == WOOD && rand() % 20 == 0) {
            nextGrid[nx][ny] = {FIRE, false, 0, 0};
        }
    }
    // rise slightly
    if (isEmpty(x, y-1) && rand() % 3 == 0) {
        nextGrid[x][y-1] = {FIRE, false, 0, nextGrid[x][y].lifetime};
        nextGrid[x][y].mat = EMPTY;
    }
}

void updateSmoke(int x, int y) {
    nextGrid[x][y].lifetime++;
    if (nextGrid[x][y].lifetime > 60 + rand() % 40) {
        nextGrid[x][y].mat = EMPTY;
        return;
    }
    int dir = (rand() % 2) ? 1 : -1;
    if (isEmpty(x, y-1)) {
        nextGrid[x][y-1] = grid[x][y];
        nextGrid[x][y].mat = EMPTY;
    } else if (isEmpty(x+dir, y-1)) {
        nextGrid[x+dir][y-1] = grid[x][y];
        nextGrid[x][y].mat = EMPTY;
    } else if (isEmpty(x-dir, y-1)) {
        nextGrid[x-dir][y-1] = grid[x][y];
        nextGrid[x][y].mat = EMPTY;
    }
}

void updateLava(int x, int y) {
    if (isEmpty(x, y+1)) {
        nextGrid[x][y+1] = grid[x][y];
        nextGrid[x][y].mat = EMPTY;
    } else {
        int dir = (rand() % 2) ? 1 : -1;
        if (isEmpty(x+dir, y)) {
            nextGrid[x+dir][y] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        } else if (isEmpty(x-dir, y)) {
            nextGrid[x-dir][y] = grid[x][y];
            nextGrid[x][y].mat = EMPTY;
        }
    }
    // lava + water = stone
    int dx[] = {0,0,1,-1};
    int dy[] = {1,-1,0,0};
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (inBounds(nx, ny) && grid[nx][ny].mat == WATER) {
            nextGrid[x][y] = {STONE, false, 0, 0};
            nextGrid[nx][ny] = {EMPTY, false, 0, 0};
            return;
        }
    }
}

void simulate() {
    for (int x = 0; x < COLS; x++)
        for (int y = 0; y < ROWS; y++)
            nextGrid[x][y] = grid[x][y];

    for (int y = ROWS-1; y >= 0; y--) {
        for (int x = 0; x < COLS; x++) {
            if (grid[x][y].updated) continue;
            switch (grid[x][y].mat) {
                case SAND:  updateSand(x, y); break;
                case WATER: updateWater(x, y); break;
                case FIRE:  updateFire(x, y); break;
                case SMOKE: updateSmoke(x, y); break;
                case LAVA:  updateLava(x, y); break;
                default: break;
            }
        }
    }

    for (int x = 0; x < COLS; x++)
        for (int y = 0; y < ROWS; y++) {
            grid[x][y] = nextGrid[x][y];
            grid[x][y].updated = false;
        }
}

void placeMaterial(int mx, int my, Material mat, int radius = 3) {
    int cx = mx / CELL;
    int cy = my / CELL;
    for (int dx = -radius; dx <= radius; dx++)
        for (int dy = -radius; dy <= radius; dy++)
            if (inBounds(cx+dx, cy+dy) && dx*dx+dy*dy <= radius*radius)
                if (mat == EMPTY || grid[cx+dx][cy+dy].mat == EMPTY)
                    grid[cx+dx][cy+dy] = {mat, false, 0, 0};
}

int main() {
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH, (unsigned)HEIGHT}), "Sand Simulation");
    window.setFramerateLimit(60);

    sf::Image image;
    image.resize({(unsigned)WIDTH, (unsigned)HEIGHT}, sf::Color(15,15,20));
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    Material selectedMat = SAND;

    // UI rects
    sf::RectangleShape ui[7];
    Material mats[] = {SAND, WATER, FIRE, STONE, WOOD, LAVA, EMPTY};
    std::string labels[] = {"Sand","Water","Fire","Stone","Wood","Lava","Erase"};
    for (int i = 0; i < 7; i++) {
        ui[i].setSize({70.f, 25.f});
        ui[i].setPosition({(float)(10 + i * 80), 5.f});
        ui[i].setFillColor(getColor(mats[i] == EMPTY ? STONE : mats[i]));
        ui[i].setOutlineColor(sf::Color::White);
        ui[i].setOutlineThickness(1.f);
    }

    sf::Font font;
    bool fontLoaded = false;
    // Try to load a system font
    if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) fontLoaded = true;

    sf::Clock clock;
    sf::Clock fpsClock;
    int frames = 0;
    int fps = 0;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                if (kp->scancode == sf::Keyboard::Scancode::Num1) selectedMat = SAND;
                if (kp->scancode == sf::Keyboard::Scancode::Num2) selectedMat = WATER;
                if (kp->scancode == sf::Keyboard::Scancode::Num3) selectedMat = FIRE;
                if (kp->scancode == sf::Keyboard::Scancode::Num4) selectedMat = STONE;
                if (kp->scancode == sf::Keyboard::Scancode::Num5) selectedMat = WOOD;
                if (kp->scancode == sf::Keyboard::Scancode::Num6) selectedMat = LAVA;
                if (kp->scancode == sf::Keyboard::Scancode::Num0) selectedMat = EMPTY;
                if (kp->scancode == sf::Keyboard::Scancode::C) {
                    for (int x=0;x<COLS;x++) for(int y=0;y<ROWS;y++) grid[x][y]={EMPTY,false,0,0};
                }
            }
            if (auto* mp = event->getIf<sf::Event::MouseButtonPressed>()) {
                for (int i = 0; i < 7; i++) {
                    if (ui[i].getGlobalBounds().contains(sf::Vector2f((float)mp->position.x, (float)mp->position.y)))
                        selectedMat = mats[i];
                }
            }
        }

        // Mouse hold to place
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            auto mp = sf::Mouse::getPosition(window);
            if (mp.y > 35)
                placeMaterial(mp.x, mp.y, selectedMat, 3);
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            auto mp = sf::Mouse::getPosition(window);
            placeMaterial(mp.x, mp.y, EMPTY, 5);
        }

        simulate();

        // Draw grid to image
        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                sf::Color c = getColor(grid[x][y].mat, grid[x][y].lifetime);
                for (int px = 0; px < CELL; px++)
                    for (int py = 0; py < CELL; py++)
                        image.setPixel({(unsigned)(x*CELL+px), (unsigned)(y*CELL+py)}, c);
            }
        }
        texture.update(image);

        frames++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.f) {
            fps = frames;
            frames = 0;
            fpsClock.restart();
        }

        window.clear();
        window.draw(sprite);

        // Draw UI buttons
        for (int i = 0; i < 7; i++) {
            ui[i].setOutlineThickness(selectedMat == mats[i] ? 3.f : 1.f);
            window.draw(ui[i]);
        }

        if (fontLoaded) {
            sf::Text fpsText(font, "FPS: " + std::to_string(fps) + "  [1]Sand [2]Water [3]Fire [4]Stone [5]Wood [6]Lava [0]Erase [C]Clear", 12);
            fpsText.setPosition({10.f, 570.f});
            fpsText.setFillColor(sf::Color::White);
            window.draw(fpsText);

            sf::Text selText(font, "Selected: " + labels[selectedMat == EMPTY ? 6 : (int)selectedMat - 1], 13);
            selText.setPosition({650.f, 5.f});
            selText.setFillColor(sf::Color::Yellow);
            window.draw(selText);
        }

        window.display();
    }
    return 0;
}
