#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "objects.h"
#include "SFML/Graphics.hpp"

class Functions
{
public:
    static void refreshFrame(sf::RenderWindow& window, Objects::Board& board, Objects::Piece* piece = nullptr);
    static void initGame(Objects::Board& board);
    static void placePieces(Objects::Board& board);
    static void fillBlankWithCells(int amount, int& index, Objects::Board& board);
    static void sortPieceTextures();
    static void splitTextureName(std::string& initname, char& recolor, std::string& rename);
    static bool isNameInRange(Objects::PieceName& name);
    static void hidePiece(sf::RenderWindow& window, Objects::Board& board, Objects::Piece* piece);

    static std::vector<Assets::ObjectTexture*> unsortedTextures;
};

#endif