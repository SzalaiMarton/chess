#include <iostream>
#include "SFML/Graphics.hpp"
#include "functions.h"

sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);

int main()
{
    Assets::loadDirectoryElements(pathToOtherTextures);
    Assets::loadDirectoryElements(pathToPieceTextures);

    Assets::ObjectTexture* boardTexture = Assets::getObjectTexture("board");

    if (boardTexture == nullptr) 
    {
        std::cerr << "Error: Board texture not found!" << std::endl;
        return -1;
    }

    Objects::Board chessBoard(boardTexture);

    std::cout << "dddd" << std::endl;

    window.setFramerateLimit(fps);
    Functions::initGame(chessBoard);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        Functions::refreshFrame(window, chessBoard);
    }
    return 0;
}