#include "functions.h"

void Functions::refreshFrame(sf::RenderWindow& window, Objects::Board& board, std::shared_ptr<Objects::Piece> piece, bool promotoionOpen, Functions::PromotionWindow* promotionWindow, std::vector<std::shared_ptr<Objects::Indicator>>* checkLine, Functions::OutcomeWindow* outcomeWindow, bool gameEnd)
{
    window.clear();
    window.draw(board.sprite);
    
	for (auto& p : board.onBoard)
    {
		if (piece == p)
		{
			continue;
		}
		else
		{
			window.draw(p->sprite);
		}
    }

	if (!checkLine->empty())
	{
		for (auto& el : *checkLine)
		{
			el->sprite.setTexture(Assets::getObjectTexture("ind_test")->texture);
			window.draw(el->sprite);
		}
	}

	if (piece != nullptr)
	{
		if (!piece->legalMoves.empty())
		{
			for (auto& direction : piece->legalMoves)
			{
				for (auto& ind : direction)
				{
					if (ind != nullptr)
					{
						window.draw(ind->sprite);
					}
				}
			}
		}
		window.draw(piece->sprite);
	}

	if (promotoionOpen)
	{
		std::cout << "dsadsa" << std::endl;
		window.draw(promotionWindow->shape);
		window.draw(promotionWindow->title);
		std::cout << "dsadsa" << std::endl;
		for (auto& el : promotionWindow->options)
		{
			window.draw(el.shape);
		}
	}

	if (gameEnd)
	{
		window.draw(outcomeWindow->shape);
	}
    
	window.display();
}

void Functions::initGame(Objects::Board& board)
{
	Functions::sortPieceTextures();
	Functions::placePieces(board);
}

void Functions::placePieces(Objects::Board& board)
{
	uint8_t index = 0;
	std::shared_ptr<Objects::Piece> newPiece;
	
	for (auto& texture : Assets::pieceTextures)
	{
		Objects::PieceColor color;
		Objects::PieceName name;

		Functions::splitTextureName(texture->name, color, name);
 
		if (name == Objects::PAWN)
		{
			for (int i = 0; i < 8; i++)
			{
				newPiece = Functions::createNewPiece(board, name, color, texture, index);
				newPiece->sprite.setOrigin(newPiece->sprite.getLocalBounds().getSize().x / 2, newPiece->sprite.getLocalBounds().getSize().y / 2);
				newPiece->sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
				index++;
			}
			index--; //correction
		}
		else
		{
			newPiece = Functions::createNewPiece(board, name, color, texture, index);
		}

		index++;

		if (index == 16)
		{
			Functions::fillBlankWithCells(32, index, board);
		}

	}
}

void Functions::fillBlankWithCells(uint8_t amount, uint8_t& index, Objects::Board& board)
{
	std::shared_ptr<Assets::ObjectTexture> cellTexture = Assets::getObjectTexture("cell");
	for (int i = 0; i < amount; i++)
	{
		Functions::createNewPiece(board, Objects::CELL, Objects::NONE_COLOR, cellTexture, index);
		index++;
	}
}

void Functions::sortPieceTextures()
{
	std::vector<std::string>& currentSide = blackPieceOrder;
	std::vector<std::shared_ptr<Assets::ObjectTexture>> unsortedTextures = Assets::pieceTextures;

	Assets::pieceTextures.clear();

	for (int colorIndex = 0; colorIndex < colorOrder.size(); colorIndex++)
	{
		for (int pieceIndex = 0; pieceIndex < currentSide.size(); pieceIndex++)
		{
			for (int unsortedPiece = 0; unsortedPiece < unsortedTextures.size(); unsortedPiece++)
			{
				if ((colorOrder[colorIndex] + currentSide[pieceIndex]) == unsortedTextures[unsortedPiece]->name)
				{
					Assets::pieceTextures.emplace_back(unsortedTextures[unsortedPiece]);
					break;
				}
			}
		}
		currentSide = whitePieceOrder;
	}
}

void Functions::splitTextureName(std::string& initname, Objects::PieceColor& recolor, Objects::PieceName& rename)
{
	rename = Objects::convertStringToPieceName(initname.substr(1, initname.length()));
	recolor = Objects::convertCharToPieceColor(initname[0]);
}

bool Functions::isNameInRange(Objects::PieceName& name)
{
	if (Objects::ROOK <= name && Objects::INVALID_NAME > name)
	{
		return true;
	}
	return false;
}

bool Functions::isPieceMatchTurn(std::shared_ptr<Objects::Piece> piece, short turn)
{
	if (turn == piece->color)
	{
		return true;
	}
	return false;
}

void Functions::afterMove(std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece>& prevRoundPiece, short& turn, bool& check, Objects::Board& chessBoard, std::vector<std::shared_ptr<Objects::Indicator>>& checkLine, bool& alreadyCheckForBlock, bool& alreadyCheckForPromotion, std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces)
{
	if (check && currentPiece->name != Objects::KING)
	{
		currentPiece->isPinned = true;
		prevRoundPiece->pinnedPiece = currentPiece;
		pinnedPieces.emplace_back(currentPiece);
	}
	else
	{
		currentPiece->getPinnedPieces(pinnedPieces, chessBoard);
	}

	checkLine.clear();
	check = false;

	if (currentPiece->name != Objects::KING) // get checkLine
	{
		currentPiece->getLegalMoves(chessBoard, true);
		check = chessBoard.checkForCheck(currentPiece, chessBoard.getKingByColor(Objects::getOpposingColor(currentPiece->color)), checkLine);
		currentPiece->deleteLegalMoves();
	}

	chessBoard.deleteAllMoves();
	chessBoard.resetPieceBools(turn);
	prevRoundPiece = currentPiece;
	alreadyCheckForBlock = false;
	alreadyCheckForPromotion = false;
	currentPiece->firstMove = false;
	turn *= -1;
}

void Functions::changePlace(Objects::Board& chessBoard, std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> targetPiece, float currentPieceLastPosX, float currentPieceLastPosY)
{
	chessBoard.snapPieceToTile(targetPiece, currentPieceLastPosX, currentPieceLastPosY);
	chessBoard.setAllEnpassantFalse();
	currentPiece->deleteLegalMoves();
}

std::shared_ptr<Objects::Piece> Functions::createNewPiece(Objects::Board& board, Objects::PieceName name, Objects::PieceColor color, std::shared_ptr<Assets::ObjectTexture> texture, uint8_t index)
{
	// creates a piece and adds it to onBoard vector

	std::shared_ptr<Objects::Piece> newPiece = std::make_shared<Objects::Piece>(name, color, texture->texture);

	newPiece->sprite.setOrigin(newPiece->sprite.getLocalBounds().getSize().x / 2, newPiece->sprite.getLocalBounds().getSize().y / 2);
	newPiece->sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
	newPiece->sprite.setScale(pieceScale, pieceScale);
	
	board.onBoard.emplace_back(newPiece);
	
	return newPiece;
}

std::shared_ptr<Objects::Piece> Functions::getCurrentPiece(sf::RenderWindow& window, Objects::Board& chessBoard)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	return chessBoard.getPieceByMouse(mousePos);
}

void Functions::blockingPieces(Objects::Board* chessBoard, bool check, bool& alreadyCheckForBlock, short turn, std::vector<std::shared_ptr<Objects::Indicator>>* checkLine)
{
	if (check)
	{
		chessBoard->getBlockingPieces(turn, checkLine);
		alreadyCheckForBlock = true;
	}
}

Functions::PromotionWindow::PromotionWindow(std::vector<std::string> options)
{
	this->shape.setRotation(90.f);
	this->shape.setTexture(Assets::getObjectTexture("promotion")->texture);
	this->shape.setOrigin(this->shape.getLocalBounds().width / 2, this->shape.getLocalBounds().height / 2);
	this->shape.setScale(pieceScale, pieceScale);
	this->shape.setPosition((float)windowWidth/2, (float)windowHeight/2);

	this->title.setTexture(Assets::getObjectTexture("promotion_title")->texture);
	this->title.setScale(pieceScale, pieceScale);
	this->title.setOrigin(this->title.getGlobalBounds().width / 2, this->title.getGlobalBounds().height / 2);
	this->title.setPosition(472.f, 327.f);
	
	this->createOptions(options);
	this->reArrangeButtons();
}

void Functions::PromotionWindow::createOptions(std::vector<std::string> options)
{
	sf::Vector2f mainWindowSize = this->shape.getPosition();
	for (short i = 0; i < options.size(); i++)
	{
		sf::Vector2f pos(0.f, 0.f);
		Functions::Button temp(pos, options[i]);
		this->options.emplace_back(temp);
	}
}

void Functions::PromotionWindow::reArrangeButtons()
{
	sf::FloatRect mainWindowSize = this->shape.getGlobalBounds();
	float firstX = mainWindowSize.left + 17.f;
	float fixedY = mainWindowSize.top + mainWindowSize.height / 2;
	float gapX = (mainWindowSize.width - 20.f) / this->options.size();
	
	for (short i = 0; i < options.size(); i++)
	{
		sf::Vector2f pos(firstX + (i * gapX), fixedY);
		options[i].shape.setPosition(pos);
	}
}

std::string Functions::PromotionWindow::getPromotionButton(sf::Vector2i pos)
{
	for (auto& button : this->options)
	{
		if (button.shape.getGlobalBounds().contains(pos.x, pos.y))
		{
			return button.name;
		}
	}
	return "invalid";
}

Functions::Button::Button(sf::Vector2f pos, const std::string& name)
{
	this->shape.setPosition(pos);
	std::shared_ptr<Assets::ObjectTexture> texture = Assets::getObjectTexture(name);
	if (texture != nullptr)
	{
		this->shape.setTexture(texture->texture);
	}
	this->name = name.substr(name.find("_") + 1);
}

Functions::OutcomeWindow::OutcomeWindow(const sf::Vector2f& center)
{
	std::string textName;
	
	this->shape.setOrigin(70.f,75.f);
	this->shape.scale(pieceScale, pieceScale);
	this->shape.setPosition(center);
}

void Functions::OutcomeWindow::changeTexture(Objects::GameOutcome outcome)
{
	if (outcome == Objects::BLACK_WIN)
	{
		std::cout << "blackWIn" << std::endl;
		this->shape.setTexture(Assets::getObjectTexture("black_win")->texture);
	}
	else if (outcome == Objects::WHITE_WIN)
	{
		std::cout << "white win" << std::endl;
		this->shape.setTexture(Assets::getObjectTexture("white_win")->texture);
	}
	else if (outcome == Objects::STALEMATE)
	{
		std::cout << "stalemate" << std::endl;
		this->shape.setTexture(Assets::getObjectTexture("stalemate")->texture);
	}
	else
	{
		return;
	}
}
