#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <vector>
#include "assets.h"
#include "settings.h"
#include "SFML/Graphics.hpp"

class Objects
{
public:

    enum PieceColor
    {
        WHITE = 'w',
        BLACK = 'b',
        NONE = 'n',
    };

    enum PieceName
    {
        KING = 0,
        QUEEN = 1,
        BISHOP = 2,
        KNIGHT = 3,
        ROOK = 4,
        PAWN = 5,
        CELL = 6
    };

    struct Indicator
    {
        int x, y;
        sf::Sprite sprite;
    };

    class Piece
    {
    public:
        int x, y;
        bool firstMove;
        bool isPinned;
        int pinnedByIndex;
        PieceColor color;
        PieceName name;
        sf::Sprite sprite;
        std::vector<std::vector<Indicator>> legalMoves;

        Piece(PieceName name, PieceColor color, int x, int y);
        Piece(PieceName name, PieceColor color, int x, int y, const sf::Texture&);

        void deletePiece();
        void setTexture(const sf::Texture& texture);
    };

    class Board
    {
    public:
        sf::Sprite sprite;
        std::vector<Piece> onBoard;
        void removePiece(Piece*);

        Board(Assets::ObjectTexture*);
    };

    static std::string getPieceNameString(Objects::PieceName);
    static PieceName convertStringToPieceName(std::string&);
    static PieceColor convertCharToPieceColor(char);
};

#endif