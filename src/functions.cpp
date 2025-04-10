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
			y = 6;
		}
		for (int x = 0; x < 8; x++)
		{
			std::string name = "asd";
			char color = 'w';
			std::cout << Assets::pieceTextures[index]->name;
			Functions::splitTextureName(Assets::pieceTextures[index]->name, color, name);
			Assets::ObjectTexture* tempTexture = Assets::getObjectTexture(name);
			Objects::Piece newPiece(Objects::convertStringToPieceName(name), Objects::convertCharToPieceColor(color), x, y, tempTexture->texture);
			board.onBoard.emplace_back(newPiece);
			index++;
			cellX += cellWidth;
		}
		cellY += cellHeight;
		cellX = 0;
	}
}

void Functions::fillBlankWithCells(int amount, int x, int y, Objects::Board& board)
{
	int cellX = cellWidth * x;
	int cellY = cellHeight * y;
	for (int index = 0; index < amount; index++)
	{
		Assets::ObjectTexture* tempTexture = Assets::getObjectTexture("cell");
		board.onBoard.emplace_back(Objects::Piece(Objects::CELL, Objects::NONE, x, y, tempTexture->texture));
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
			if (element->name.find("pawn") != 0)
			{
				for (int i = 0; i < 8; i++)
				{
					Assets::pieceTextures.emplace_back(element);
				}
				if (colorIndex == 0)
				{
					colorIndex++;
					currentSide = whitePieceOrder;
				}
			}
			else if (element->name == (colorOrder[colorIndex] + currentSide[pieceIndex]))
			{
				Assets::pieceTextures.emplace_back(element);
			}
		}
	}
}

void Functions::splitTextureName(std::string& name, char color, std::string& rename)
{
	color = name[0];
	rename = name.substr(1, name.length());
}
