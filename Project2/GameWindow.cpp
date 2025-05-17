#include "GameWindow.h"
#include <iostream>

GameWindow::GameWindow(int width, int height, const std::string& title) {
    window.create(sf::VideoMode(width, height), title);

    keyPressCallback = [](sf::Keyboard::Key key) {};
    mouseClickCallback = [](int x, int y) {}; // For further mouse click implementation
}

void GameWindow::clear() {
    window.clear();
}

void GameWindow::display() {
    window.display();
}

bool GameWindow::isOpen() const {
    return window.isOpen();
}

void GameWindow::pollEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            keyPressCallback(event.key.code);
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                mouseClickCallback(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
}

sf::RenderWindow& GameWindow::getWindow() {
    return window;
}

void GameWindow::setKeyPressCallback(std::function<void(sf::Keyboard::Key)> callback) {
    keyPressCallback = callback;
}

void GameWindow::setMouseClickCallback(std::function<void(int x, int y)> callback) {
    mouseClickCallback = callback;
}