#include "functions.h"

void Functions::refreshFrame(sf::RenderWindow& window, Objects::Board& board, std::shared_ptr<Objects::Piece> piece)
{
    window.clear();
    window.draw(board.sprite);
    for (auto& obj : board.onBoard)
    {
		if (piece == obj)
		{
			continue;
		}
        window.draw(obj->sprite);
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
	for (auto& texture : Assets::pieceTextures)
	{
		Objects::PieceColor color;
		Objects::PieceName name;

		Functions::splitTextureName(texture->name, color, name);
		auto newPiece = Functions::createNewPiece(board, name, color, texture, index);
		if (name == Objects::PAWN)
		{
			for (int i = 0; i < 8; i++)
			{
				newPiece->sprite.setOrigin(newPiece->sprite.getLocalBounds().getSize().x / 2, newPiece->sprite.getLocalBounds().getSize().y / 2);
				newPiece->sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
				board.onBoard.emplace_back(newPiece);
				index++;
			}
			index--; //correction
		}
		else
		{
			board.onBoard.emplace_back(newPiece);
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
		board.onBoard.emplace_back(Functions::createNewPiece(board, Objects::CELL, Objects::NONE, Assets::getObjectTexture("cell"), index));
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

void Functions::choosePieceForPromotion(std::shared_ptr<Objects::Piece> piecePromoted) // make a window where the player can select the desired piece
{

}

void Functions::afterMove(std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> prevRoundPiece, short& turn, bool& check, Objects::Board& chessBoard, std::vector<std::shared_ptr<Objects::Indicator>>& checkLine, bool& alreadyCheckForBlock, bool& alreadyCheckForPromotion, std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces)
{
	if (check && prevRoundPiece != nullptr)
	{
		currentPiece->isPinned = true;
		prevRoundPiece->pinnedPiece = currentPiece;
		pinnedPieces.emplace_back(currentPiece);
	}
	check = false;
	currentPiece->firstMove = false;
	turn *= -1;
	if (currentPiece->name != Objects::KING)
	{
		currentPiece->getLegalMoves(chessBoard, true);
		check = chessBoard.checkForCheck(currentPiece, chessBoard.getKingByColor(Objects::getOpposingColor(currentPiece->color)), checkLine);
		currentPiece->deleteLegalMoves();
	}
	chessBoard.deleteAllMoves();
	currentPiece->getPinnedPieces(pinnedPieces, chessBoard);
	prevRoundPiece = currentPiece;
	alreadyCheckForBlock = false;
	alreadyCheckForPromotion = false;
}

void Functions::changePlace(Objects::Board& chessBoard, std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> targetPiece, float currentPieceLastPosX, float currentPieceLastPosY)
{
	chessBoard.snapPieceToTile(targetPiece, currentPieceLastPosX, currentPieceLastPosY);
	chessBoard.setAllEnpassantFalse();
	currentPiece->deleteLegalMoves();
}

std::shared_ptr<Objects::Piece> Functions::createNewPiece(Objects::Board& board, Objects::PieceName name, Objects::PieceColor color, std::shared_ptr<Assets::ObjectTexture> texture, uint8_t index)
{
	Objects::Piece newPiece = *new Objects::Piece(name, color, texture->texture);
	newPiece.sprite.setOrigin(newPiece.sprite.getLocalBounds().getSize().x / 2, newPiece.sprite.getLocalBounds().getSize().y / 2);
	newPiece.sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
	newPiece.sprite.setScale(pieceScale, pieceScale);
	return std::make_shared<Objects::Piece>(newPiece);
}
