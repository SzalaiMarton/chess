#include "functions.h"

sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);

int main()
{
    std::shared_ptr<Objects::Piece> currentPiece = nullptr;
    std::shared_ptr<Objects::Piece> targetPiece = nullptr;
    std::shared_ptr<Objects::Piece> prevPiece = nullptr;
    std::shared_ptr<Objects::Piece> prevRoundPiece = nullptr;
    std::shared_ptr<Objects::Piece> toBePromoted = nullptr;

    std::vector<std::shared_ptr<Objects::Piece>> pinnedPieces{};
	std::vector<std::shared_ptr<Objects::Indicator>> checkLine{};

    float currentPieceLastPosX{}, currentPieceLastPosY{};
    short turn = 1; //1 -> white, -1 -> black
    bool check = false;
	bool alreadyCheckForBlock = false;
    bool alreadyCheckForPromotion = false;
    bool promotionWindowOpen = false;

    Assets::loadDirectoryElements(pathToOtherTextures);
    Assets::loadDirectoryElements(pathToPieceTextures);

    Functions::PromotionWindow promotionWindow({"promotion_knight_white", "promotion_knight_white", "promotion_knight_white", "promotion_bishop_white"});
    
    std::shared_ptr<Assets::ObjectTexture> boardTexture = Assets::getObjectTexture("board");

    if (boardTexture == nullptr) 
    {
        std::cerr << "Error: Board texture not found!" << std::endl;
        return -1;
    }

    Objects::Board chessBoard(boardTexture);
    window.setFramerateLimit(60);
    Functions::initGame(chessBoard);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::MouseButtonPressed)
            {
                
                if (check && !alreadyCheckForBlock)
                {
                    chessBoard.getBlockingPieces(turn, checkLine);
                    alreadyCheckForBlock = true;
                }

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
                    if ((check && !chessBoard.canBlock(currentPiece) && currentPiece->name != Objects::KING))
                    {
                        continue;
                    }
                    if (currentPiece != prevPiece && currentPiece->legalMoves.size() == 0 && !currentPiece->isPinned)
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
                            chessBoard.snapPieceToTile(currentPiece, currentPieceLastPosX, currentPieceLastPosY);
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
                
                chessBoard.snapPieceToTile(currentPiece);
                
                if (targetPiece == nullptr || targetPiece->color == currentPiece->color) // no move
                {
                    chessBoard.snapPieceToTile(currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
                else if (targetPiece->name == Objects::CELL && currentPiece->isTargetInMoves(targetPiece)) // regular move
                {
                    if (currentPiece->isMoveEnpassant())
                    {
                        chessBoard.removeEnpassantPiece(currentPiece->sprite.getPosition(), turn);
                    }
                
                    Functions::changePlace(chessBoard, currentPiece, targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                    
                    if (currentPiece->name == Objects::PAWN)
                    {
                        chessBoard.checkEnpassant(currentPiece);
                    }
                    
                    Functions::afterMove(currentPiece, prevRoundPiece, turn, check, chessBoard, checkLine, alreadyCheckForBlock, alreadyCheckForPromotion, pinnedPieces);
                }
                else if (targetPiece->color != currentPiece->color && currentPiece->isTargetInMoves(targetPiece)) // attack move
                {
                    chessBoard.removePiece(targetPiece);
                    
                    Functions::changePlace(chessBoard, currentPiece, targetPiece, currentPieceLastPosX, currentPieceLastPosY);
                    
                    if (currentPiece->name == Objects::PAWN)
                    {
                        chessBoard.checkEnpassant(currentPiece);
                    }
                    
                    Functions::afterMove(currentPiece, prevRoundPiece, turn, check, chessBoard, checkLine, alreadyCheckForBlock, alreadyCheckForPromotion, pinnedPieces);
                }
                else // not on board
                {
                    chessBoard.snapPieceToTile(currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
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
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
                {
                    promotionWindowOpen = true;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
                {
                    promotionWindowOpen = false;
                }
            }
        }
        Functions::refreshFrame(window, chessBoard, currentPiece, promotionWindowOpen, &promotionWindow);
    }
    return 0;
}