#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "objects.h"

class Functions
{
public:
    class Button
    {
    public:
        std::string name;
        sf::Sprite shape;


        Button() = default;
        Button(sf::Vector2f pos, const std::string& name);
    };

    class PromotionWindow : public Button
    {
    public:
        std::vector<Button> options;
        sf::Sprite title;


        PromotionWindow(std::vector<std::string> options);

        void createOptions(std::vector<std::string> options);
        void reArrangeButtons();
        std::string getPromotionButton(sf::Vector2i pos);
    };

    class OutcomeWindow
    {
    public:
        sf::Sprite shape;

        OutcomeWindow(const sf::Vector2f& center);

        void changeTexture(Objects::GameOutcome outcome);
    };

    static void refreshFrame(sf::RenderWindow& window, Objects::Board& board, std::shared_ptr<Objects::Piece> piece = nullptr, bool promotionOpen = false, Functions::PromotionWindow* promotionWindow = nullptr, std::vector<std::shared_ptr<Objects::Indicator>>* checkLine = {}, Functions::OutcomeWindow* outcomeWindow = nullptr, bool gameEnd = false);
    static void initGame(Objects::Board& board);
    static void placePieces(Objects::Board& board);
    static void fillBlankWithCells(uint8_t amount, uint8_t& index, Objects::Board& board);
    static void sortPieceTextures();
    static void splitTextureName(std::string& initname, Objects::PieceColor& recolor, Objects::PieceName& rename);
    static bool isNameInRange(Objects::PieceName& name);
    static bool isPieceMatchTurn(std::shared_ptr<Objects::Piece> piece, short turn);
    static void afterMove(std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece>& prevRoundPiece, short& turn, bool& check, Objects::Board& chessBoard, std::vector<std::shared_ptr<Objects::Indicator>>& checkLine, bool& alreadyCheckForBlock, bool& alreadyCheckForPromotion, std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces);
    static void changePlace(Objects::Board& chessBoard, std::shared_ptr<Objects::Piece> currentPiece, std::shared_ptr<Objects::Piece> targetPiece, float currentPieceLastPosX, float currentPieceLastPosY);
    static std::shared_ptr<Objects::Piece> createNewPiece(Objects::Board& board, Objects::PieceName name, Objects::PieceColor color, std::shared_ptr<Assets::ObjectTexture> texture, uint8_t index);
    static std::shared_ptr<Objects::Piece> getCurrentPiece(sf::RenderWindow& window, Objects::Board& chessBoard);
    static void blockingPieces(Objects::Board* chessBoard, bool check, bool& alreadyCheckForBlock, short turn, std::vector<std::shared_ptr<Objects::Indicator>>* checkLine);
};

#endif