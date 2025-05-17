#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class GameWindow {
public:
    GameWindow(int width, int height, const std::string& title);
    void clear();
    void display();
    bool isOpen() const;
    void pollEvents();                        // Handle window events
    sf::RenderWindow& getWindow();            // Get the SFML window

  
    void setKeyPressCallback(std::function<void(sf::Keyboard::Key)> callback);
    void setMouseClickCallback(std::function<void(int x, int y)> callback);

private:
    sf::RenderWindow window;

    // Event callbacks
    std::function<void(sf::Keyboard::Key)> keyPressCallback;
    std::function<void(int x, int y)> mouseClickCallback; 
};