#include <iostream>
#include "SFML/Graphics.hpp"
#include "functions.h"

sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);

int main()
{
    Objects::Piece* currentPiece = nullptr;
    Objects::Piece* targetPiece = nullptr;
    int currentPieceLastPosX, currentPieceLastPosY;


    Assets::loadDirectoryElements(pathToOtherTextures);
    Assets::loadDirectoryElements(pathToPieceTextures);

    Assets::ObjectTexture* boardTexture = Assets::getObjectTexture("board");

    if (boardTexture == nullptr) 
    {
        std::cerr << "Error: Board texture not found!" << std::endl;
        return -1;
    }

    Objects::Board chessBoard(boardTexture);
    window.setFramerateLimit(fps);
    Functions::initGame(chessBoard);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                currentPiece = chessBoard.getPiece(mousePos);
                currentPieceLastPosX = currentPiece->sprite.getPosition().x;
                currentPieceLastPosY = currentPiece->sprite.getPosition().y;
                if (currentPiece->name != Objects::CELL && Functions::isNameInRange(currentPiece->name))
                {
                    while (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        currentPiece->sprite.setPosition((float)mousePos.x, (float)mousePos.y);

                        Functions::refreshFrame(window, chessBoard, currentPiece);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                //if none of the if statement's value is true then don't change the turn because the piece went back to the last pos by the player
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                targetPiece = chessBoard.getPiece(mousePos, currentPiece);
                chessBoard.snapPieceToTile(*currentPiece);
                std::cout << targetPiece->name << std::endl;
                if (targetPiece->color == currentPiece->color)
                {
                    chessBoard.snapPieceToTile(*currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
                else if (targetPiece->sprite.getPosition() == currentPiece->sprite.getPosition())
                {
                    chessBoard.removePiece(targetPiece);
                    chessBoard.snapPieceToTile(*targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
                else if (targetPiece->color == Objects::CELL)
                {
                    //just change turn
                }
            }

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        Functions::refreshFrame(window, chessBoard);
    }
    return 0;
}