#include "functions.h"

std::vector<Assets::ObjectTexture*> Functions::unsortedTextures;

void Functions::refreshFrame(sf::RenderWindow& window, Objects::Board& board, Objects::Piece* piece)
{
    window.clear();
    window.draw(board.sprite);
    for (auto& obj : board.onBoard)
    {
		if (piece == &obj)
		{
			continue;
		}
        window.draw(obj.sprite);
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
	int index = 0;
	for (auto& texture : Assets::pieceTextures)
	{
		char color;
		std::string name;
		Functions::splitTextureName(texture->name, color, name);
		Objects::Piece newPiece(Objects::convertStringToPieceName(name), Objects::convertCharToPieceColor(color), texture->texture);
		newPiece.sprite.setOrigin(newPiece.sprite.getLocalBounds().getSize().x / 2, newPiece.sprite.getLocalBounds().getSize().y / 2);
		newPiece.sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
		newPiece.sprite.setScale(pieceScale, pieceScale);
		if (name == "pawn")
		{
			for (int i = 0; i < 8; i++)
			{
				newPiece.sprite.setOrigin(newPiece.sprite.getLocalBounds().getSize().x / 2, newPiece.sprite.getLocalBounds().getSize().y / 2);
				newPiece.sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
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

void Functions::fillBlankWithCells(int amount, int& index, Objects::Board& board)
{
	for (int i = 0; i < amount; i++)
	{
		Objects::Piece newPiece(Objects::CELL, Objects::NONE, Assets::getObjectTexture("cell")->texture);
		newPiece.sprite.setOrigin(newPiece.sprite.getLocalBounds().getSize().x / 2, newPiece.sprite.getLocalBounds().getSize().y / 2);
		newPiece.sprite.setPosition(board.tilePoints[index][0], board.tilePoints[index][1]);
		newPiece.sprite.setScale(pieceScale, pieceScale);
		board.onBoard.emplace_back(newPiece);
		index++;
	}
}

void Functions::sortPieceTextures()
{
	std::vector<std::string>& currentSide = blackPieceOrder;
	Functions::unsortedTextures = Assets::pieceTextures;
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

void Functions::splitTextureName(std::string& initname, char& recolor, std::string& rename)
{
	recolor = initname[0];
	rename = initname.substr(1, initname.length());
}

bool Functions::isNameInRange(Objects::PieceName& name)
{
	if (Objects::ROOK <= name && Objects::INVALID_NAME > name)
	{
		return true;
	}
	return false;
}

bool Functions::isPieceMatchTurn(Objects::Piece* piece, int turn)
{
	if (turn == piece->color)
	{
		return true;
	}
	return false;
}
