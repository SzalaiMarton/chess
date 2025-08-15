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
    bool gameEnd = false;
	bool alreadyCheckForBlock = false;
    bool alreadyCheckForPromotion = false;
    bool promotionWindowOpen = false;

    Assets::loadDirectoryElements(pathToOtherTextures);
    Assets::loadDirectoryElements(pathToPieceTextures);

    std::shared_ptr<Assets::ObjectTexture> boardTexture = Assets::getObjectTexture("board");

    if (boardTexture == nullptr) 
    {
        std::cerr << "Error: Board texture not found!" << std::endl;
        return -1;
    }

    Objects::Board chessBoard(boardTexture);
    window.setFramerateLimit(60);
    
    sf::Vector2f* center = new sf::Vector2f(chessBoard.sprite.getGlobalBounds().width / 2, chessBoard.sprite.getGlobalBounds().height / 2);
    Functions::OutcomeWindow outcomeWindow(*center);
    delete center;

    Functions::PromotionWindow promotionWindow({ "promotion_knight", "promotion_rook", "promotion_queen", "promotion_bishop" });
    Functions::initGame(chessBoard);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (!alreadyCheckForBlock)
                {
                    Functions::blockingPieces(&chessBoard, check, alreadyCheckForBlock, turn, &checkLine);
                }

                currentPiece = Functions::getCurrentPiece(window, chessBoard);
                if (currentPiece == nullptr)
                {
                    continue;
                }

                currentPieceLastPosX = currentPiece->sprite.getPosition().x;
                currentPieceLastPosY = currentPiece->sprite.getPosition().y;

                if (currentPiece->name != Objects::CELL && Functions::isNameInRange(currentPiece->name) && Functions::isPieceMatchTurn(currentPiece, turn))
                {
                    if ((check && !chessBoard.canPieceBlock(currentPiece) && currentPiece->name != Objects::KING) /*|| currentPiece->isPinned*/)
                    {
                        continue;
                    }
                    else if (currentPiece != prevPiece && currentPiece->legalMoves.size() == 0 && !currentPiece->isPinned)
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
                            currentPiece = nullptr;
                            break;
                        }

                        Functions::refreshFrame(window, chessBoard, currentPiece, false, nullptr, &checkLine);
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

                    Objects::GameOutcome outcome = chessBoard.checkForOutcome(currentPiece->color, check, true);
                    if (outcome != Objects::NO_OUTCOME)
                    {
                        outcomeWindow.changeTexture(outcome);
                        gameEnd = true;
                    }
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

                    Objects::GameOutcome outcome = chessBoard.checkForOutcome(currentPiece->color, check, true);
                    if (outcome != Objects::NO_OUTCOME)
                    {
                        outcomeWindow.changeTexture(outcome);
                        gameEnd = true;
                    }
                }
                else // not on board
                {
                    chessBoard.snapPieceToTile(currentPiece, currentPieceLastPosX, currentPieceLastPosY);
                }
            }

            if (!alreadyCheckForPromotion) // check promotion
            {
                toBePromoted = chessBoard.getPromotingPiece();
                alreadyCheckForPromotion = true;
                if (toBePromoted != nullptr)
                {
                    promotionWindowOpen = true;
                    Functions::refreshFrame(window, chessBoard, currentPiece, promotionWindowOpen, &promotionWindow, &checkLine);

                    while (promotionWindowOpen)
                    {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            std::string name = promotionWindow.getPromotionButton(mousePos);
                            if (name != "invalid")
                            {
                                toBePromoted->transformPiece(name);
                                toBePromoted->getLegalMoves(chessBoard, false);
                                check = chessBoard.checkForCheck(toBePromoted, chessBoard.getKingByColor(Objects::getOpposingColor(currentPiece->color)), checkLine);
                                promotionWindowOpen = false;
                            }
                        }
                    }
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

        Functions::refreshFrame(window, chessBoard, currentPiece, promotionWindowOpen, &promotionWindow, &checkLine, &outcomeWindow, gameEnd);

        if (gameEnd)
        {
            break;
        }
    }
    std::cin.get();
    return 0;
}