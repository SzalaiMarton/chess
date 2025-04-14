#include <iostream>
#include "SFML/Graphics.hpp"
#include "functions.h"

sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);

int main()
{
    Objects::Piece* currentPiece = nullptr;
    Objects::Piece* targetPiece = nullptr;
    Objects::Piece* prevPiece = nullptr;
    float currentPieceLastPosX{}, currentPieceLastPosY{};
    int turn = 1; //1 -> white, -1 -> black

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
                if (currentPiece != nullptr)
                {
                    currentPiece->deleteLegalMoves();
                }
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                currentPiece = chessBoard.getPiece(mousePos);
                if (currentPiece == nullptr)
                {
                    continue;
                }
                currentPieceLastPosX = currentPiece->sprite.getPosition().x;
                currentPieceLastPosY = currentPiece->sprite.getPosition().y;
                if (currentPiece->name != Objects::CELL && Functions::isNameInRange(currentPiece->name) && Functions::isPieceMatchTurn(currentPiece, turn))
                {
                    currentPiece->getLegalMoves(chessBoard);
                    prevPiece = currentPiece;
                    while (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        currentPiece->sprite.setPosition((float)mousePos.x, (float)mousePos.y);
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                        {
                            chessBoard.snapPieceToTile(*currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                            break;
                        }
                        Functions::refreshFrame(window, chessBoard, currentPiece);
                    }
                }
                else
                {
                    currentPiece = nullptr;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && currentPiece != nullptr)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                targetPiece = chessBoard.getPiece(mousePos, currentPiece);
                chessBoard.snapPieceToTile(*currentPiece);
                if (targetPiece == nullptr || targetPiece->color == currentPiece->color)
                {
                    chessBoard.snapPieceToTile(*currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
                else if (targetPiece->name == Objects::CELL && currentPiece->isTargetInMoves(targetPiece))
                {
                    chessBoard.snapPieceToTile(*targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                    currentPiece->deleteLegalMoves();
                    currentPiece->firstMove = false;
                    turn *= -1;
                }
                else if (targetPiece->color != currentPiece->color && currentPiece->isTargetInMoves(targetPiece))
                {
                    chessBoard.removePiece(targetPiece);
                    chessBoard.snapPieceToTile(*targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                    currentPiece->deleteLegalMoves();
                    currentPiece->firstMove = false;
                    turn *= -1;
                }
                else
                {
                    chessBoard.snapPieceToTile(*currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
            }

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        if (currentPiece != nullptr)
        {
            Functions::refreshFrame(window, chessBoard, currentPiece);
        }
        else
        {
            Functions::refreshFrame(window, chessBoard);
        }
    }
    return 0;
}