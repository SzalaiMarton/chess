#include "functions.h"

std::vector<Assets::ObjectTexture*> Functions::unsortedTextures;

void Functions::refreshFrame(sf::RenderWindow& window, Objects::Board& board)
{
    window.clear();
    window.draw(board.sprite);
    for (auto &obj : board.onBoard)
    {
        window.draw(obj.sprite);
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
	int cellX = 0;
	int cellY = 0;
	for (auto& texture : Assets::pieceTextures)
	{
		char color;
		std::string name;
		Functions::splitTextureName(texture->name, color, name);
		Objects::Piece newPiece(Objects::convertStringToPieceName(name), Objects::convertCharToPieceColor(color), texture->texture);
		newPiece.sprite.setPosition(cellX*cellWidth, cellY*cellHeight);
		newPiece.sprite.setScale(pieceScale, pieceScale);
		if (name == "pawn")
		{
			for (int i = 0; i < 8; i++)
			{
				newPiece.sprite.setPosition(i * cellWidth, cellY * cellHeight);
				board.onBoard.emplace_back(newPiece);
				cellX = 7;
			}
		}
		else
		{
			board.onBoard.emplace_back(newPiece);
		}
		cellX++;
		if (cellX >= 8)
		{
			cellX = 0;
			cellY++;
		}
		if (cellY == 2)
		{
			Functions::fillBlankWithCells(32, cellX, cellY, board);
		}
	}
}

void Functions::fillBlankWithCells(int amount, int& cellX, int& cellY, Objects::Board& board)
{
	for (int i = 0; i < amount; i++)
	{
		Objects::Piece newPiece(Objects::CELL, Objects::NONE, Assets::getObjectTexture("cell")->texture);
		newPiece.sprite.setPosition(cellX * cellWidth, cellY * cellHeight);
		newPiece.sprite.setScale(pieceScale, pieceScale);
		board.onBoard.emplace_back(newPiece);
		cellX++;
		if (cellX >= 8)
		{
			cellX = 0;
			cellY++;
		}
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

void Functions::splitTextureName(std::string& name, char& color, std::string& rename)
{
	color = name[0];
	rename = name.substr(1, name.length());
}
