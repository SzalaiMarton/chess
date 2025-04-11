#include "objects.h"
#include "assets.h"

void Objects::Piece::deletePiece()
{
    Assets::ObjectTexture* temp = Assets::getObjectTexture("cell");

    if (temp == nullptr)
    {
        std::cerr << "Failed to delete " << this->color << " " << this->name << std::endl;
    }
    
    this->name = PieceName::CELL;
    this->color = PieceColor::NONE;
    this->isPinned = false;
    this->legalMoves.clear();
    this->sprite.setTexture(temp->texture);
}

void Objects::Piece::setTexture(const sf::Texture& texture)
{
    this->sprite.setTexture(texture);
}

Objects::Piece::Piece()
{
}

Objects::Piece::Piece(PieceName name, PieceColor color)
{
    this->name = name;
    this->color = color;
    this->firstMove = true;
    this->isPinned = false;
}

Objects::Piece::Piece(PieceName name, PieceColor color, const sf::Texture& texture)
{
    this->name = name;
    this->color = color;
    this->firstMove = true;
    this->isPinned = false;
    this->setTexture(texture);
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
        default: return Objects::INVALID;
    }
}

void Objects::Board::snapPieceToTile(Objects::Piece& piece, int x, int y)
{
    //get the piece pos and snap it to the closest tile
    //to get the closest tile: get the distance between piece and current tile the compare that with a range and see if its in it
    int posX, posY;
    if (x != -1 && y != -1)
    {
        piece.sprite.setPosition(x, y);
    }
    else
    {
        for (auto& tile : this->tilePoints)
        {
            float distanceX = std::abs(tile[0] - piece.sprite.getPosition().x);
            float distanceY = std::abs(tile[1] - piece.sprite.getPosition().y);
            if (distanceX < cellWidth / 2 && distanceY < cellHeight / 2)
            {
                piece.sprite.setPosition(tile[0], tile[1]);
                return;
            }
        }
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
    this->createTiles();
}

void Objects::Board::createTiles()
{
    float currentX = cellWidth/2;
    float currentY = cellHeight/2;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            this->tilePoints.push_back({ currentX + (cellWidth * x), currentY + (cellHeight*y) });
        }
    }
}

Objects::Piece* Objects::Board::getPiece(sf::Vector2i& mousePos, Objects::Piece* skipPiece)
{
    for (auto& piece : this->onBoard)
    {
        if (skipPiece != nullptr && skipPiece == &piece)
        {
            continue;
        }
        if (piece.sprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
        {
            return &piece;
        }
    }
}