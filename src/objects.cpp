#include "objects.h"
#include "assets.h"

void Objects::Piece::deletePiece()
{
    Assets::ObjectTexture temp = Assets::getObjectTexture("cell");
    if(temp.name == "null")
    {
        std::cerr << "Failed to delete " << this->color << " " << this->name << std::endl;
    }
    this->name = PieceName::CELL;
    this->color = PieceColor::NONE;
    this->isPinned = false;
    this->pinnedByIndex = nullIndex;
    this->legalMoves.empty();
    this->sprite.setTexture(temp.texture);
}

void Objects::Piece::setTexture(sf::Texture texture)
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
    this->pinnedByIndex = nullIndex;
}

std::string Objects::getPieceNameString(Objects::PieceName piece) {
    switch (piece) {
        case Objects::KING: return "king";
        case Objects::QUEEN: return "queen";
        case Objects::BISHOP: return "bishop";
        case Objects::KNIGHT: return "knight";
        case Objects::ROOK: return "rook";
        case Objects::PAWN: return "pawn";
        default: return "cell";
    }
}

void Objects::Board::removePiece(Piece* piece)
{
    piece->deletePiece();
}

Objects::Board::Board()
{
    return;
}

Objects::Board::Board(Assets::ObjectTexture* objTexture)
{
    this->sprite.setTexture(objTexture->texture);
}