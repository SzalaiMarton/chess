#include "functions.h"

std::vector<Assets::ObjectTexture> Functions::unsortedTextures;

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
	Functions::placePieces(board);
}

void Functions::placePieces(Objects::Board& board)
{
	int cellX = 0;
	int cellY = 0;
	int index = 0;
	for (int y = 0; y < 8; y++)
	{
		if (y == 3)
		{
			Functions::fillBlankWithCells(32, 0, y, board);
		}
		for (int x = 0; x < 8; x++)
		{
			std::string name = "asd";
			char color = 'w';
			Functions::splitTextureName(Assets::pieceTextures[index].name, color, name);
			board.onBoard.emplace_back(new Objects::Piece(Objects::convertStringToPieceName(name), Objects::convertCharToPieceColor(color), x, y, Assets::getObjectTexture(name).texture));
			index++;
			cellX += cellWidth;
		}
		cellY += cellHeight;
		cellX = 0;
	}
}

void Functions::fillBlankWithCells(int ammount, int x, int y, Objects::Board& board)
{
	int cellX = cellWidth * x;
	int cellY = cellHeight * y;
	for (int index = 0; index < ammount; index++)
	{
		board.onBoard.emplace_back(new Objects::Piece(Objects::CELL, Objects::NONE, x, y, Assets::getObjectTexture("cell").texture));
		if (x == 8)
		{
			x = 0;
			cellX = 0;
			y++;
			cellY += cellHeight;
		}
	}
}

void Functions::sortPieceTextures()
{
	bool blackDone = false;
	int colorIndex = 0;
	int pieceIndex = 0;
	std::vector<std::string>& currentSide = blackPieceOrder;
	Functions::unsortedTextures = Assets::pieceTextures;
	Assets::pieceTextures.clear();
	while (!unsortedTextures.empty())
	{
		for (auto& element : unsortedTextures)
		{
			if (element.name.find("pawn") != 0)
			{
				for (int i = 0; i < 8; i++)
				{
					Assets::pieceTextures.emplace_back(element.name);
				}
				if (colorIndex == 0)
				{
					colorIndex++;
					currentSide = whitePieceOrder;
				}
			}
			else if (element.name == (colorOrder[colorIndex] + currentSide[pieceIndex]))
			{
				Assets::pieceTextures.emplace_back(element.name);
			}
		}
	}
}

void Functions::splitTextureName(std::string& name, char color, std::string& rename)
{
	color = name[0];
	rename = name.substr(1, name.length());
}
