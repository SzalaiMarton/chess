#include "objects.h"
#include "assets.h"

void Objects::Piece::deletePiece()
{
    Assets::ObjectTexture temp = Assets::getObjectTexture("cell");
    if(temp.name == noName)
    {
        std::cerr << "Failed to delete " << this->color << " " << this->name << std::endl;
    }
    this->name = PieceName::CELL;
    this->color = PieceColor::NONE;
    this->isPinned = false;
    this->pinnedByIndex = noIndex;
    this->legalMoves.clear();
    this->sprite.setTexture(temp.texture);
}

void Objects::Piece::setTexture(const sf::Texture& texture)
{
    this->sprite.setTexture(texture);
}

Objects::Piece::Piece(PieceName name, PieceColor color, int x, int y)
{
    this->name = name;
    this->color = color;
    this->x = x;
    this->y = y;
    this->firstMove = true;
    this->isPinned = false;
    this->pinnedByIndex = noIndex;
}

Objects::Piece::Piece(PieceName name, PieceColor color, int x, int y, const sf::Texture& texture)
{
    this->name = name;
    this->color = color;
    this->x = x;
    this->y = y;
    this->firstMove = true;
    this->isPinned = false;
    this->pinnedByIndex = noIndex;
    this->setTexture(texture);
}

std::string Objects::getPieceNameString(Objects::PieceName piece) {
    switch (piece) 
    {
        case Objects::KING: return "king";
        case Objects::QUEEN: return "queen";
        case Objects::BISHOP: return "bishop";
        case Objects::KNIGHT: return "knight";
        case Objects::ROOK: return "rook";
        case Objects::PAWN: return "pawn";
        default: return "cell";
    }
}

Objects::PieceName Objects::convertStringToPieceName(std::string& name)
{
    if (name == "king")
    {
        return Objects::KING;
    }
    else if (name == "queen")
    {
        return Objects::QUEEN;
    }
    else if (name == "bishop")
    {
        return Objects::BISHOP;
    }
    else if (name == "knight")
    {
        return Objects::KNIGHT;
    }
    else if (name == "rook")
    {
        return Objects::ROOK;
    }
    else if (name == "pawn")
    {
        return Objects::PAWN;
    }
    else
    {
        return Objects::CELL;
    }
}

Objects::PieceColor Objects::convertCharToPieceColor(char color)
{
    switch (color)
    {
        case 'w': return Objects::WHITE;
        case 'b': return Objects::BLACK;
        case 'n': return Objects::NONE;
    }
}

void Objects::Board::removePiece(Piece* piece)
{
    piece->deletePiece();
}

Objects::Board::Board(Assets::ObjectTexture* objTexture)
{
    this->sprite.setTexture(objTexture->texture);
    this->sprite.setScale(boardScale, boardScale);
}