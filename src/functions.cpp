#include "functions.h"

void Functions::refreshFrame(sf::RenderWindow& window, Objects::Board& board, std::shared_ptr<Objects::Piece> piece, bool promotoionOpen, Functions::PromotionWindow* promotionWindow)
{
    window.clear();
    window.draw(board.sprite);
    
	for (auto& obj : board.onBoard)
    {
		if (piece == obj)
		{
			continue;
		}
		else
		{
			window.draw(obj->sprite);
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
		window.draw(promotionWindow->shape);
		window.draw(promotionWindow->title);
		for (auto& el : promotionWindow->options)
		{
			window.draw(el.shape);
		}
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
	for (int i = 0; i < amount; i++)
	{
		Functions::createNewPiece(board, Objects::CELL, Objects::NONE, Assets::getObjectTexture("cell"), index);
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
	if (check)
	{
		currentPiece->isPinned = true;
		prevRoundPiece->pinnedPiece = currentPiece;
		pinnedPieces.emplace_back(currentPiece);
	}
	else
	{
		currentPiece->getPinnedPieces(pinnedPieces, chessBoard);
	}

	check = false;

	if (currentPiece->name != Objects::KING)
	{
		currentPiece->getLegalMoves(chessBoard, true);
		check = chessBoard.checkForCheck(currentPiece, chessBoard.getKingByColor(Objects::getOpposingColor(currentPiece->color)), checkLine);
		currentPiece->deleteLegalMoves();
	}
	
	chessBoard.deleteAllMoves();
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
		Functions::Button temp(pos, options[i].substr(1));
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

Objects::PieceName Functions::PromotionWindow::getPromotionPiece(sf::Vector2i pos)
{
	for (auto& button : this->options)
	{
		if (button.shape.getGlobalBounds().contains(pos.x, pos.y))
		{
			return Objects::convertStringToPieceName(button.name);
		}
	}
	return Objects::INVALID_NAME;
}

Functions::Button::Button(sf::Vector2f pos, const std::string& name)
{
	this->shape.setPosition(pos);
	std::shared_ptr<Assets::ObjectTexture> texture = Assets::getObjectTexture(name);
	if (texture != nullptr)
	{
		this->shape.setTexture(texture->texture);
	}
	this->name = name;
	std::cout << name << "\n";
}