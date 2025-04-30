#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <vector>
#include <algorithm>
#include <set>
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
        Objects::PieceName targetName;
        bool enpassant;
    };

    struct Vector2fComparator
    {
        bool operator()(const sf::Vector2f& a, const sf::Vector2f& b) const 
        {
            if (a.x < b.x) {
                return true;
            }
            if (a.x > b.x) {
                return false;
            }
            return a.y < b.y;
        }
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
        Piece* pinnedPiece;

        Piece();
        Piece(PieceName name, PieceColor color);
        Piece(PieceName name, PieceColor color, const sf::Texture&);

        void getLegalMoves(Objects::Board& board, bool onlyAttacks = false);
        void getLegalMovesNoRestrictions(Objects::Board& board);
        void checkPawnAttack(Objects::Board& board, int x, int direction, bool onlyAttack = false); // x = -1 or 1 -> depends on the side
        void deletePiece();
        void deleteLegalMoves();
        void setTexture(const sf::Texture& texture);
        void createLegalMove(int direction, Objects::Piece* targetCell, bool enpassant = false);
        bool isTargetInMoves(Objects::Piece* target);
        void resetPiece();
        bool isMoveEnpassant();
        void getKnightMoves(Objects::Board& board);

        void kingMoveGetter(Objects::Board& board);
        void getKingMoveNoRestriction(Objects::Board& board);
        void sortKingMoves(std::set<sf::Vector2f, Objects::Vector2fComparator>& dangerZone);
        void getDangerZone(Objects::Board& board, std::set<sf::Vector2f, Objects::Vector2fComparator>& cells);
        void getPinnedPieces(std::vector<Objects::Piece*>& pinnedPieces, Objects::Board& board);
        void revaluePinningPieces(std::vector<Objects::Piece*>& pinnedPieces);
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
        bool checkForCheck(Objects::Piece* piece, Objects::Piece* king, std::vector<Objects::Indicator*>& checkLine);
        void getBlockingPieces(int turn, std::vector<Objects::Indicator*>& checkLine);
        bool canBlock(Objects::Piece* piece);
        Objects::Piece* getKingByColor(Objects::PieceColor color);
        void deleteAllMoves();
        void removeEnpassantPiece(sf::Vector2f pos, int turn);
    };

    static PieceName convertStringToPieceName(std::string& name);
    static PieceColor convertCharToPieceColor(char color);
    static Piece noPiece;
    static void getMoveProperties(Objects::Piece* piece, std::vector<Objects::Directions>& directions, uint8_t& amount);
    static void getDirectionMultiplier(Objects::Directions direction, int& x, int& y);
    static bool isTargetCellValid(Objects::Piece* targetCell, Objects::Piece* piece, Objects::Directions direction, bool onlyAttack = false);

    static Objects::Directions addTwoDirections(Objects::Directions vertical, Objects::Directions horizontal);
    static bool isVerticalDir(Objects::Directions dir);
    static bool isHorizontalDir(Objects::Directions dir);
    static bool isDiagonalDir(Objects::Directions dir);

    static Objects::PieceColor getOpposingColor(Objects::PieceColor color);
    static Objects::Indicator* makeIndicator(sf::Sprite sprite, Objects::PieceName targetName, bool enpassant = false);

    static std::string forDevNameToString(Objects::PieceName name);
    static char forDevColorToChar(Objects::PieceColor color);
    static std::string forDevDirToString(Objects::Directions dir);
};

#endif