#include <iostream>
#include "SFML/Graphics.hpp"
#include "functions.h"

sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);

int main()
{
    Objects::Piece* currentPiece = nullptr;
    Objects::Piece* targetPiece = nullptr;
    Objects::Piece* prevPiece = nullptr;
    Objects::Piece* toBePromoted = nullptr;

	std::vector<Objects::Indicator*> checkLine{};

    float currentPieceLastPosX{}, currentPieceLastPosY{};
    int turn = 1; //1 -> white, -1 -> black
    bool check = false;
	bool alreadyCheckForCheck = false;
    bool alreadyCheckForPromotion = false;

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
                currentPiece = chessBoard.getPieceByMouse(mousePos);
                if (currentPiece == nullptr)
                {
                    continue;
                }

                currentPieceLastPosX = currentPiece->sprite.getPosition().x;
                currentPieceLastPosY = currentPiece->sprite.getPosition().y;
                if (currentPiece->name != Objects::CELL && Functions::isNameInRange(currentPiece->name) && Functions::isPieceMatchTurn(currentPiece, turn))
                {
                    if (check && !chessBoard.canBlock(currentPiece) && currentPiece->name != Objects::KING)
                    {
                        continue;
                    }
                    if (currentPiece != prevPiece && currentPiece->legalMoves.size() == 0)
                    {
                        currentPiece->getLegalMoves(chessBoard);
                    }
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
                targetPiece = chessBoard.getPieceByMouse(mousePos, currentPiece);
                chessBoard.snapPieceToTile(*currentPiece);
                if (targetPiece == nullptr || targetPiece->color == currentPiece->color) // no move
                {
                    chessBoard.snapPieceToTile(*currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
                else if (targetPiece->name == Objects::CELL && currentPiece->isTargetInMoves(targetPiece)) // regular move
                {
                    if (currentPiece->isMoveEnpassant())
                    {
                        sf::Vector2i cell{};
                        cell.x = currentPiece->sprite.getPosition().x;
                        cell.y = currentPiece->sprite.getPosition().y + (cellHeight * turn);
                        Objects::Piece* pieceToDelete = chessBoard.getPieceByMouse(cell);
                        chessBoard.removePiece(pieceToDelete);
                    }
                    chessBoard.snapPieceToTile(*targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                    currentPiece->deleteLegalMoves();
                    chessBoard.setAllEnpassantFalse();
                    if (currentPiece->name == Objects::PAWN)
                    {
                        chessBoard.checkEnpassant(currentPiece);
                    }
                    currentPiece->firstMove = false;
                    turn *= -1;
                    alreadyCheckForCheck = false;
                    alreadyCheckForPromotion = false;
                }
                else if (targetPiece->color != currentPiece->color && currentPiece->isTargetInMoves(targetPiece)) // attack move
                {
                    chessBoard.removePiece(targetPiece);
                    chessBoard.snapPieceToTile(*targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                    currentPiece->deleteLegalMoves();
                    chessBoard.setAllEnpassantFalse();
                    if (currentPiece->name == Objects::PAWN)
                    {
                        chessBoard.checkEnpassant(currentPiece);
                    }
                    currentPiece->firstMove = false;
                    turn *= -1;
                    alreadyCheckForCheck = false;
                    alreadyCheckForPromotion = false;
                }
                else // not on board
                {
                    chessBoard.snapPieceToTile(*currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
            }

            if (!alreadyCheckForCheck)
            {
                check = chessBoard.checkForCheck(turn, checkLine);
                if (check && !alreadyCheckForCheck)
                {
                    chessBoard.getBlockingPieces(turn, checkLine);
                }
                alreadyCheckForCheck = true;
            }
            
            if (!alreadyCheckForPromotion)
            {
                toBePromoted = chessBoard.checkPromotion();
                alreadyCheckForPromotion = true;
                if (toBePromoted != nullptr)
                {
                    std::cout << "promote" << std::endl;
                }
            }
            
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
                {
                    chessBoard.startingPosition();
                    turn = 1;
                }
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