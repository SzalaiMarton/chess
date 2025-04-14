#include "objects.h"
#include "assets.h"

void Objects::Piece::getLegalMoves(Objects::Board& board)
{
    int multiplierX{}, multiplierY{};
    sf::Vector2i cell{};
    Objects::Piece* targetCell{};
    std::vector<Objects::Directions> directions;
    int amount{};
    Objects::getMoveProperties(this, directions, amount);
    if (directions.empty())
    {
        Objects::knightMoves(this);
        return;
    }
    this->legalMoves.resize(directions.size());
    if (this->name == Objects::PAWN && !this->firstMove)
    {
        amount--;
    }
    if (this->name == Objects::PAWN)
    {
        this->checkPawnAttack(board, -1, 0); // left
        this->checkPawnAttack(board, 1, 0); // right
    }
    for (int direction = 0; direction < directions.size(); direction++)
    {
        cell.x = (int)this->sprite.getPosition().x;
        cell.y = (int)this->sprite.getPosition().y; 
        multiplierX = 0;
        multiplierY = 0;
        Objects::getDirectionMultiplier(directions[direction], multiplierX, multiplierY);
        for (int i = 0; i < amount; i++)
        {
            cell.x = cell.x + (cellWidth * multiplierX);
            cell.y = cell.y + (cellHeight * multiplierY);
            targetCell = board.getPiece(cell);
            if (!Objects::isTargetCellValid(targetCell, this, direction))
            {
                break;
            }
        }
    }
}

bool Objects::isTargetCellValid(Objects::Piece* targetCell, Objects::Piece* piece, int direction)
{
    if (targetCell != nullptr)
    {
        if (targetCell->name == Objects::CELL)
        {
            piece->createLegalMove(direction, targetCell);
        }
        else if (piece->name != Objects::PAWN && piece->color != targetCell->color)
        {
            piece->createLegalMove(direction, targetCell);
        }
        if (targetCell->color != Objects::NONE)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void Objects::Piece::checkPawnAttack(Objects::Board& board, int x, int direction)
{
    sf::Vector2i cell;
    int y = 0;
    if (this->color == Objects::BLACK)
    {
        y = 1;
    }
    else
    {
        y = -1;
    }
    cell.x = this->sprite.getPosition().x + (cellWidth * x);
    cell.y = this->sprite.getPosition().y + (cellWidth * y);
    Objects::Piece* targetCell = board.getPiece(cell);
    if (targetCell != nullptr && targetCell->color != this->color && targetCell->color != Objects::NONE)
    {
        if (targetCell->color != this->color && targetCell->color != Objects::NONE)
        {
            this->createLegalMove(direction, targetCell);
        }
    }
    return;
}

void Objects::Piece::deleteLegalMoves()
{
    this->legalMoves.clear();
}

void Objects::knightMoves(Objects::Piece* piece)
{

}

void Objects::Piece::createLegalMove(int direction, Objects::Piece* targetCell)
{
    Assets::ObjectTexture* indicatorTexture = Assets::getObjectTexture("indicator");
    if (indicatorTexture == nullptr)
    {
        return;
    }
    Objects::Indicator* legalMove = new Objects::Indicator();
    legalMove->sprite = targetCell->sprite;
    legalMove->sprite.setTexture(indicatorTexture->texture);
    this->legalMoves[direction].emplace_back(legalMove);
}

bool Objects::Piece::isTargetInMoves(Objects::Piece* target)
{
    for (auto& direction : this->legalMoves)
    {
        for (auto& move : direction)
        {
            if (target->sprite.getPosition() == move->sprite.getPosition())
            {
                return true;
            }
        }
    }
    return false;
}

void Objects::getDirectionMultiplier(Objects::Directions direction, int& x, int& y)
{
    switch (direction)
    {
    case Objects::SOUTH:
        y = 1;
        break;
    case Objects::EAST:
        x = 1;
        break;
    case Objects::NORTH:
        y = -1;
        break;
    case Objects::WEST:
        x = -1;
        break;
    case Objects::NORTHWEST:
        y = -1;
        x = -1;
        break;
    case Objects::NORTHEAST:
        y = -1;
        x = 1;
        break;
    case Objects::SOUTHEAST:
        y = 1;
        x = 1;
        break;
    case Objects::SOUTHWEST:
        y = 1;
        x = -1;
        break;
    }
}




void Objects::Piece::deletePiece()
{
    Assets::ObjectTexture* temp = Assets::getObjectTexture("cell");

    if (temp == nullptr)
    {
        std::cerr << "Failed to delete " << this->color << " " << this->name << std::endl;
        return;
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
    this->name = Objects::INVALID_NAME;
    this->color = Objects::INVALID_COLOR;
    this->firstMove = false;
    this->isPinned = false;
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
    else if (name == "cell")
    {
        return Objects::CELL;
    }
    else
    {
        return Objects::INVALID_NAME;
    }
}

Objects::PieceColor Objects::convertCharToPieceColor(char color)
{
    switch (color)
    {
        case 'w': return Objects::WHITE;
        case 'b': return Objects::BLACK;
        case 'n': return Objects::NONE;
        default: return Objects::INVALID_COLOR;
    }
}

void Objects::getMoveProperties(Objects::Piece* piece, std::vector<Objects::Directions>& directions, int& amount)
{
    switch (piece->name)
    {
        case Objects::PAWN:
        {
            if (piece->color == Objects::BLACK)
            {
                directions = { Objects::SOUTH };
                amount = 2;
                break;
            }
            else if (piece->color == Objects::WHITE)
            {
                directions = { Objects::NORTH };
                amount = 2;
                break;
            }
            break;
        }
        case Objects::BISHOP:
        {
            directions = { Objects::NORTHEAST, Objects::NORTHWEST, Objects::SOUTHEAST, Objects::SOUTHWEST };
            amount = 8;
            break;
        }
        case Objects::ROOK:
        {
            directions = { Objects::NORTH, Objects::WEST, Objects::EAST, Objects::SOUTH };
            amount = 8;
            break;
        }
        case Objects::KING:
        {
            directions = { Objects::NORTHEAST, Objects::NORTHWEST, Objects::SOUTHEAST, Objects::SOUTHWEST, Objects::NORTH, Objects::WEST, Objects::EAST, Objects::SOUTH };
            amount = 1;
            break;
        }
        case Objects::QUEEN:
        {
            directions = { Objects::NORTHEAST, Objects::NORTHWEST, Objects::SOUTHEAST, Objects::SOUTHWEST, Objects::NORTH, Objects::WEST, Objects::EAST, Objects::SOUTH };
            amount = 8;
            break;
        }
    }
}



void Objects::Board::snapPieceToTile(Objects::Piece& piece, float x, float y)
{
    //get the piece pos and snap it to the closest tile
    //to get the closest tile: get the distance between piece and current tile the compare that with a range and see if its in it
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
    for (float y = 0; y < 8; y++)
    {
        for (float x = 0; x < 8; x++)
        {
            this->tilePoints.push_back({ currentX + (cellWidth * x), currentY + (cellHeight * y) });
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
    return nullptr;
}