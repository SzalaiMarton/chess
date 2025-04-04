#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "objects.h"
#include "SFML/Graphics.hpp"

class Functions
{
public:
    static void refreshFrame(sf::RenderWindow&, Objects::Board&);
    static void initGame(Objects::Board&);
    static void placePieces(Objects::Board&);
    static void fillBlankWithCells(int, int, int, Objects::Board&);
    static void sortPieceTextures();
    static void splitTextureName(std::string&, char, std::string&);

    static std::vector<Assets::ObjectTexture> unsortedTextures;

};

#endif