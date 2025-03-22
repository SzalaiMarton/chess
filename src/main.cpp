#include <iostream>
#include "SFML/Graphics.hpp"
#include "assets.h"
#include "functions.h"

sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");

int main()
{
    Assets assets;
    Assets::loadDirectoryElements(assets.pathToOtherTextures);
    Assets::loadDirectoryElements(assets.pathToPieceTextures);

    Assets::ObjectTexture boardTexture = Assets::getObjectTexture("board");
    Objects::Board chessBoard(&boardTexture);

    window.setFramerateLimit(60);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            
            



            Functions::refresFrame(window, chessBoard);
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        window.display();
    }
    return 0;
}