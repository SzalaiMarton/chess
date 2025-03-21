#pragma once

#include <string>
#include <vector>
#include "assets.h"
#include "SFML/Graphics.hpp"

class Objects
{
public:
    static const int nullIndex = 100;

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
        
        void deletePiece();
        void setTexture(sf::Texture texture);
    };

    class Board
    {
    public:
        sf::Sprite sprite;
        std::vector<std::vector<Piece>> board;
        void removePiece(Piece*);

        Board(Assets::ObjectTexture*);
    };

    static std::string getPieceNameString(Objects::PieceName piece);
    static std::vector<Piece> allPieces;
    static std::vector<Indicator> allIndicators;
};