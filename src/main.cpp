#include <iostream>
#include "SFML/Graphics.hpp"
#include "objects.h"
#include "assets.h"
#include "functions.h"

sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");
Assets assets;

void main()
{
    Assets::loadDirectoryElements(assets.pathToOtherTextures);
    Assets::loadDirectoryElements(assets.pathToPieceTextures);

    Objects::Board chessBoard(&Assets::getObjectTexture("board"));

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
}