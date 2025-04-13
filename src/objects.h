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
        WHITE = 1,
        BLACK = -1,
        NONE = 2,
        INVALID_COLOR
    };

    enum PieceName
    {
        ROOK = 0,
        KNIGHT = 1,
        BISHOP = 2,
        KING = 3,
        QUEEN = 4,
        PAWN = 5,
        CELL = 6,
        INVALID_NAME = 7
    };

    struct Indicator
    {
        int x, y;
        sf::Sprite sprite;
    };

    class Piece
    {
    public:
        bool firstMove;
        bool isPinned;
        PieceColor color;
        PieceName name;
        sf::Sprite sprite;
        std::vector<std::vector<Indicator>> legalMoves;

        Piece();
        Piece(PieceName name, PieceColor color);
        Piece(PieceName name, PieceColor color, const sf::Texture&);

        void deletePiece();
        void setTexture(const sf::Texture& texture);
    };

    class Board
    {
    public:
        sf::Sprite sprite;
        std::vector<Piece> onBoard;
        std::vector<std::vector<float>> tilePoints;
        void removePiece(Piece*);

        Board(Assets::ObjectTexture*);
        void createTiles();
        Objects::Piece* getPiece(sf::Vector2i& mousePos, Objects::Piece* skipPiece = nullptr);
        void snapPieceToTile(Objects::Piece& piece, int x = -1, int y = -1);
    };

    static PieceName convertStringToPieceName(std::string&);
    static PieceColor convertCharToPieceColor(char);
    static Piece noPiece;
};

#endif