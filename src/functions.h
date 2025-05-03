#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "objects.h"
#include "SFML/Graphics.hpp"

class Functions
{
public:
    static void refreshFrame(sf::RenderWindow& window, Objects::Board& board, std::shared_ptr<Objects::Piece> piece = nullptr);
    static void initGame(Objects::Board& board);
    static void placePieces(Objects::Board& board);
    static void fillBlankWithCells(uint8_t amount, uint8_t& index, Objects::Board& board);
    static void sortPieceTextures();
    static void splitTextureName(std::string& initname, Objects::PieceColor& recolor, Objects::PieceName& rename);
    static bool isNameInRange(Objects::PieceName& name);
    static bool isPieceMatchTurn(std::shared_ptr<Objects::Piece> piece, short turn);
    static void choosePieceForPromotion(std::shared_ptr<Objects::Piece> piecePromoted); //---------------------
    static void afterMove(std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> prevRoundPiece, short& turn, bool& check, Objects::Board& chessBoard, std::vector<std::shared_ptr<Objects::Indicator>>& checkLine, bool& alreadyCheckForBlock, bool& alreadyCheckForPromotion, std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces);
    static void changePlace(Objects::Board& chessBoard, std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> targetPiece, float currentPieceLastPosX, float currentPieceLastPosY);
    static std::shared_ptr<Objects::Piece> createNewPiece(Objects::Board& board, Objects::PieceName name, Objects::PieceColor color, std::shared_ptr<Assets::ObjectTexture> texture, uint8_t index);
};

#endif