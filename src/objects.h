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

    class Board;

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
        INDICATOR = 7,
        INVALID_NAME = 8
    };

    enum Directions
    {
        EAST = 0,
        SOUTH = 1,
        NORTH = 2,
        WEST = 3,
        NORTHWEST = 4,
        NORTHEAST = 5,
        SOUTHEAST = 6,
        SOUTHWEST = 7
    };

    struct Indicator
    {
        sf::Sprite sprite;
        bool enpassant;
    };

    class Piece
    {
    public:
        bool firstMove;
        bool isPinned;
        bool enpassantLeft;
        bool enpassantRight;
        PieceColor color;
        PieceName name;
        sf::Sprite sprite;
        std::vector<std::vector<Indicator*>> legalMoves;

        Piece();
        Piece(PieceName name, PieceColor color);
        Piece(PieceName name, PieceColor color, const sf::Texture&);

        void getLegalMoves(Objects::Board& board);
        void checkPawnAttack(Objects::Board& board, int x, int direction); // x = -1 or 1 -> depends on the side
        void deletePiece();
        void deleteLegalMoves();
        void setTexture(const sf::Texture& texture);
        void createLegalMove(int direction, Objects::Piece* targetCell, bool enpassant = false);
        bool isTargetInMoves(Objects::Piece* target);
        void resetPiece();
        bool isMoveEnpassant();
    };

    class Board
    {
    public:
        sf::Sprite sprite;
        std::vector<Piece> onBoard;
        std::vector<std::vector<float>> tilePoints;
        void removePiece(Piece* piece);

        Board(Assets::ObjectTexture* texture);
        void createTiles();
        Objects::Piece* getPieceByMouse(sf::Vector2i& mousePos, Objects::Piece* skipPiece = nullptr);
        void snapPieceToTile(Objects::Piece& piece, float x = -1.f, float y = -1.f);
        bool isTargetOnBoard(Objects::Piece* piece);
        void setAllEnpassantFalse();
        void checkEnpassant(Objects::Piece* currentPiece);
        void startingPosition();
        Objects::Piece* checkPromotion();
    };

    static PieceName convertStringToPieceName(std::string& name);
    static PieceColor convertCharToPieceColor(char color);
    static Piece noPiece;
    static void getMoveProperties(Objects::Piece* piece, std::vector<Objects::Directions>& directions, int& amount);
    static void knightMoves(Objects::Piece* piece, Objects::Board& board);
    static void getDirectionMultiplier(Objects::Directions direction, int& x, int& y);
    static bool isTargetCellValid(Objects::Piece* targetCell, Objects::Piece* piece, int direction);
};

#endif