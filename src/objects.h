#pragma once

#include "assets.h"
#include "settings.h"

namespace Objects
{
    class Board;

    enum GameOutcome
    {
        WHITE_WIN = 0,
        BLACK_WIN = 1,
        STALEMATE = 2,
        NO_OUTCOME = 3
    };

    enum PieceColor
    {
        WHITE = 1,
        BLACK = -1,
        NONE_COLOR = 2,
        INVALID_COLOR = 3
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
        NORTH_WEST = 4,
        NORTH_EAST = 5,
        SOUTH_EAST = 6,
        SOUTH_WEST = 7
    };

    class Indicator : public std::enable_shared_from_this<Indicator>
    {
    public:
        sf::Sprite sprite;
        Objects::PieceName targetName;
        bool enpassant;

        Indicator() = default;
        Indicator(const sf::Sprite& sprite, const Objects::PieceName& targetname, const bool enpassant);
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

   
    class Piece : public std::enable_shared_from_this<Piece>
    {
    public:
        bool canBlock;
        bool firstMove;
        bool isPinned;
        bool enpassantLeft;
        bool enpassantRight;
        PieceColor color;
        PieceName name;
        sf::Sprite sprite;
        std::vector<std::vector<std::shared_ptr<Indicator>>> legalMoves;
        std::shared_ptr<Piece> pinnedPiece;

        Piece(PieceName name, PieceColor color, sf::Texture& texture);
        ~Piece();

        void getLegalMoves(Objects::Board& board, bool onlyAttacks = false);
        void getLegalMovesNoRestrictions(Objects::Board& board);
        void checkPawnAttack(Objects::Board& board, int x, int direction, bool onlyAttack = false); // x = -1 or 1 -> depends on the side
        void deletePiece();
        void deleteLegalMoves();
        void setTexture(const sf::Texture& texture);
        void createLegalMove(uint8_t direction, std::shared_ptr<Objects::Piece> targetCell, bool enpassant = false);
        bool isTargetInMoves(std::shared_ptr<Objects::Piece> target);
        void resetPiece();
        bool isMoveEnpassant();
        void getKnightMoves(Objects::Board& board);
        void transformPiece(std::string name);
        void changesForPawn(Objects::Board& board, uint8_t& indicatorAmount, bool onlyAttacks);
        bool isLegalMovesEmpty();

        void kingMoveGetter(Objects::Board& board);
        void getKingMoveNoRestriction(Objects::Board& board);
        void sortKingMoves(std::set<sf::Vector2f, Objects::Vector2fComparator>& dangerZone);
        void getDangerZone(Objects::Board& board, std::set<sf::Vector2f, Objects::Vector2fComparator>& cells) const;
        void getPinnedPieces(std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces, Objects::Board& board);
        void revaluePinningPieces(std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces);
    };

    class Board
    {
    public:
        sf::Sprite sprite;
        std::vector<std::shared_ptr<Piece>> onBoard;
        std::vector<std::vector<float>> tilePoints;
        void removePiece(std::shared_ptr<Piece> piece);

        Board(std::shared_ptr<Assets::ObjectTexture> texture);
        
        void printAllPiece();

        void createTiles();
        std::shared_ptr<Objects::Piece> getPieceByMouse(sf::Vector2i& mousePos, std::shared_ptr<Objects::Piece> skipPiece = nullptr);
        void snapPieceToTile(std::shared_ptr<Objects::Piece> piece, float x = -1.f, float y = -1.f);
        bool isTargetOnBoard(std::shared_ptr<Objects::Piece> piece);
        void setAllEnpassantFalse();
        void checkEnpassant(std::shared_ptr<Objects::Piece> currentPiece);
        void startingPosition();
        std::shared_ptr<Objects::Piece> getPromotingPiece();
        bool checkForCheck(std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> king, std::vector<std::shared_ptr<Objects::Indicator>> &checkLine);
        void getBlockingPieces(short turn, std::vector<std::shared_ptr<Objects::Indicator>>* checkLine);
        bool canPieceBlock(std::shared_ptr<Objects::Piece> piece);
        std::shared_ptr<Objects::Piece> getKingByColor(Objects::PieceColor color);
        void deleteAllMoves();
        void removeEnpassantPiece(sf::Vector2f pos, short turn);
        void resetPieceBools(short turn);
        Objects::GameOutcome checkForOutcome(Objects::PieceColor currentColor, bool check, bool reversedColor);
    };

    PieceName convertStringToPieceName(const std::string& name);
    PieceColor convertCharToPieceColor(char color);
    void getMoveProperties(std::shared_ptr<Objects::Piece> piece, std::vector<Objects::Directions>& directions, uint8_t& amount);
    void getDirectionMultiplier(Objects::Directions direction, short& x, short& y);
    bool isTargetCellValid(std::shared_ptr<Objects::Piece> targetCell, std::shared_ptr<Objects::Piece> piece, Objects::Directions direction, bool onlyAttack = false);

    Objects::Directions addTwoDirections(Objects::Directions vertical, Objects::Directions horizontal);
    bool isVerticalDir(Objects::Directions dir);
    bool isHorizontalDir(Objects::Directions dir);
    bool isDiagonalDir(Objects::Directions dir);

    Objects::PieceColor getOpposingColor(Objects::PieceColor color);
    std::shared_ptr<Objects::Indicator> makeIndicator(sf::Sprite sprite, Objects::PieceName targetName, bool enpassant = false);

    void getPieceIndexesByTurn(short turn, uint8_t& firstIndex, uint8_t& lastIndex, bool reversed);
    void getPieceIndexesByColor(Objects::PieceColor color, uint8_t& firstIndex, uint8_t& lastIndex, bool reversed);

    std::string forDevNameToString(Objects::PieceName name);
    char pieceColorToChar(Objects::PieceColor color);
    std::string forDevDirToString(Objects::Directions dir);
};
