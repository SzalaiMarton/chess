#include "objects.h"
#include "assets.h"

void Objects::Piece::getLegalMoves(Objects::Board& board, bool onlyAttacks)
{
    if (this->name == Objects::KNIGHT)
    {
        this->getKnightMoves(board);
        return;
    }
    
    short multiplierX{}, multiplierY{};
    sf::Vector2i cellCords{};
    std::shared_ptr<Objects::Piece> targetCell{};
    std::vector<Objects::Directions> directions;
    uint8_t indicatorAmount{};

    Objects::getMoveProperties(std::make_shared<Objects::Piece>(this), directions, indicatorAmount);
    this->legalMoves.resize(8);

    if (this->name == Objects::PAWN && !this->firstMove)
    {
        indicatorAmount--;
    }

    if (this->name == Objects::PAWN)
    {
        this->checkPawnAttack(board, -1, 0, onlyAttacks); // left
        this->checkPawnAttack(board, 1, 0, onlyAttacks); // right
        if (onlyAttacks)
        {
            return;
        }
    }

    for (auto& direction : directions)
    {
        cellCords.x = (int)this->sprite.getPosition().x;
        cellCords.y = (int)this->sprite.getPosition().y;
        multiplierX = 0;
        multiplierY = 0;
        Objects::getDirectionMultiplier(direction, multiplierX, multiplierY);
        for (int i = 0; i < indicatorAmount; i++)
        {
            cellCords.x = cellCords.x + (cellWidth * multiplierX);
            cellCords.y = cellCords.y + (cellHeight * multiplierY);
            targetCell = board.getPieceByMouse(cellCords);
            if (Objects::isTargetCellValid(targetCell, std::make_shared<Objects::Piece>(this), direction, onlyAttacks) == false)
            {
                break;
            }
        }
    }

    if (this->name == Objects::KING && !onlyAttacks)
    {
        this->kingMoveGetter(board);
    }
}

void Objects::Piece::getLegalMovesNoRestrictions(Objects::Board& board)
{
    // used to get pinned piece
    // only used for rook, bishop, queen
    // only restriction is: ally is blocking since those cannot be pinned by ally

    short multiplierX{}, multiplierY{};
    sf::Vector2i cellCords{};
    std::shared_ptr<Objects::Piece> targetCell{};
    std::vector<Objects::Directions> directions;
    uint8_t indicatorAmount{};

    Objects::getMoveProperties(std::make_shared<Objects::Piece>(this), directions, indicatorAmount);
    this->legalMoves.resize(8);

    for (auto& direction : directions)
    {
        cellCords.x = (int)this->sprite.getPosition().x;
        cellCords.y = (int)this->sprite.getPosition().y;
        multiplierX = 0;
        multiplierY = 0;
        Objects::getDirectionMultiplier(direction, multiplierX, multiplierY);
        for (int i = 0; i < indicatorAmount; i++)
        {
            cellCords.x = cellCords.x + (cellWidth * multiplierX);
            cellCords.y = cellCords.y + (cellHeight * multiplierY);
            targetCell = board.getPieceByMouse(cellCords);
            if (targetCell == nullptr || targetCell->color == this->color)
            {
                return;
            }
            else
            {
                this->createLegalMove(direction, targetCell);
            }
        }
    }
}

bool Objects::isTargetCellValid(std::shared_ptr<Objects::Piece>targetCell, std::shared_ptr<Objects::Piece> piece, Objects::Directions direction, bool onlyAttack)
{
    //returns true if nothing blocking
    //returns false if something is blocking
    if (targetCell != nullptr)
    {
        if (targetCell->name == Objects::CELL) // regular move
        {
            piece->createLegalMove(direction, targetCell);
            return true;
        }
        else if (piece->name != Objects::PAWN && piece->color != targetCell->color) // attack 
        {
            piece->createLegalMove(direction, targetCell);
            if (onlyAttack && targetCell->name == Objects::KING)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (piece->color == targetCell->color) // blocking by ally
        {
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void Objects::Piece::kingMoveGetter(Objects::Board& board)
{
    std::set<sf::Vector2f, Objects::Vector2fComparator> dangerZone;
	this->getDangerZone(board, dangerZone);
    this->sortKingMoves(dangerZone);
}


Objects::Directions Objects::addTwoDirections(Objects::Directions vertical, Objects::Directions horizontal)
{
    if (vertical == Objects::NORTH)
    {
        if (horizontal == Objects::WEST)
        {
            return Objects::NORTHWEST;
        }
        else if (horizontal == Objects::EAST)
        {
            return Objects::NORTHEAST;
        }
    }
    else if (vertical == Objects::SOUTH)
    {
        if (horizontal == Objects::WEST)
        {
            return Objects::SOUTHWEST;
        }
        else if (horizontal == Objects::EAST)
        {
            return Objects::SOUTHEAST;
        }
    }
}

bool Objects::isVerticalDir(Objects::Directions dir)
{
    if (dir == Objects::NORTH || dir == Objects::SOUTH)
    {
        return true;
    }
    return false;
}

bool Objects::isHorizontalDir(Objects::Directions dir)
{
    if (dir == Objects::EAST || dir == Objects::WEST)
    {
        return true;
    }
    return false;
}

bool Objects::isDiagonalDir(Objects::Directions dir)
{
    if (dir == Objects::NORTHEAST || dir == Objects::NORTHWEST || dir == Objects::SOUTHEAST || dir == Objects::SOUTHWEST)
    {
        return true;
    }
    return false;
}

Objects::PieceColor Objects::getOpposingColor(Objects::PieceColor color)
{
    if (color == Objects::WHITE)
    {
        return Objects::BLACK;
    }
    else
    {
        return Objects::WHITE;
    }
}

std::shared_ptr<Objects::Indicator> Objects::makeIndicator(sf::Sprite sprite, Objects::PieceName targetName, bool enpassant)
{
    auto indicatorTexture = Assets::getObjectTexture("indicator");
    if (indicatorTexture == nullptr)
    {
        return nullptr;
    }
    auto legalMove = std::make_shared<Objects::Indicator>();
    legalMove->targetName = targetName;
    legalMove->sprite = sprite;
    legalMove->sprite.setTexture(indicatorTexture->texture);
    legalMove->enpassant = enpassant;
    return legalMove;
}


void Objects::Piece::getDangerZone(Objects::Board& board, std::set<sf::Vector2f, Objects::Vector2fComparator>& cells) const
{
	uint8_t firstInd{}, lastInd{};
    if (this->color == Objects::WHITE)
    {
		firstInd = firstBlackIndex;
		lastInd = lastBlackIndex;
	}
	else
	{
		firstInd = firstWhiteIndex;
		lastInd = lastWhiteIndex;
	}
    for (uint8_t pieceInd = firstInd; pieceInd < lastInd; pieceInd++)
    {
        board.onBoard[pieceInd]->getLegalMoves(board, true);
        for (auto& dir : board.onBoard[pieceInd]->legalMoves)
        {
            for (auto& move : dir)
            {
                cells.insert(move->sprite.getPosition());
            }
        }
        board.onBoard[pieceInd]->deleteLegalMoves();
    }
}

void Objects::Piece::getPinnedPieces(std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces, Objects::Board& board)
{
    // gets current piece's moves without enemy blocking its line, if an enemy's piece and king in the same line the piece gets added to pinnedPieces
    // else nothing happens

    if (this->name == Objects::PAWN || this->name == Objects::KNIGHT || this->name == Objects::KING)
    {
        return;
    }

    this->revaluePinningPieces(pinnedPieces);
    std::vector<std::shared_ptr<Objects::Piece>> inLinePieces;
    this->getLegalMovesNoRestrictions(board);
    for (auto& dir : this->legalMoves)
    {
        for (auto& move : dir)
        {
            if (move->targetName != Objects::CELL && move->targetName != Objects::KING)
            {
                sf::Vector2i cord(move->sprite.getPosition().x, move->sprite.getPosition().y);
                inLinePieces.emplace_back(board.getPieceByMouse(cord));
                if (inLinePieces.size() > 1)
                {
                    break;
                }
            }
            else if (move->targetName == Objects::KING)
            {
                if (inLinePieces.size() == 1)
                {
                    inLinePieces[0]->isPinned = true;
                    pinnedPieces.emplace_back(inLinePieces[0]);
                    this->pinnedPiece = inLinePieces[0];
                    break;
                }
            }
        }
        inLinePieces.clear();
    }
    this->deleteLegalMoves();
}

void Objects::Piece::revaluePinningPieces(std::vector<std::shared_ptr<Objects::Piece>>& pinnedPieces)
{
    for (int i = 0; i < pinnedPieces.size(); i++)
    {
        if (pinnedPieces[i] == this->pinnedPiece)
        {
            pinnedPieces.erase(pinnedPieces.begin()+i);
            pinnedPieces[i]->isPinned = false;
            this->pinnedPiece = nullptr;
            return;
        }
    }
}

void Objects::Piece::getKingMoveNoRestriction(Objects::Board& board)
{
	short multiplierX{}, multiplierY{};
	sf::Vector2i cell{};
    std::shared_ptr<Objects::Piece> targetCell{};
	std::vector<Directions> directions;
	uint8_t amount{};

    Objects::getMoveProperties(std::make_shared<Objects::Piece>(this), directions, amount);
    amount = 1;
	this->legalMoves.resize(8);

    for (auto& dir : directions)
    {
        cell.x = (int)this->sprite.getPosition().x;
        cell.y = (int)this->sprite.getPosition().y;
        multiplierX = 0;
        multiplierY = 0;
        Objects::getDirectionMultiplier(dir, multiplierX, multiplierY);
        for (uint8_t i = 0; i < amount; i++)
        {
            cell.x = cell.x + (cellWidth * multiplierX);
            cell.y = cell.y + (cellHeight * multiplierY);
            targetCell = board.getPieceByMouse(cell);
            if (!Objects::isTargetCellValid(targetCell, std::make_shared<Objects::Piece>(this), dir))
            {
                break;
            }
        }
    }
}

void Objects::Piece::sortKingMoves(std::set<sf::Vector2f, Objects::Vector2fComparator>& dangerZone)
{
    bool found = false;
    for (auto& danger : dangerZone)
    {
        found = false;
        for (auto& dir : this->legalMoves)
        {
            for (uint8_t moveInd = 0; moveInd < dir.size(); moveInd++)
            {
                if (dir[moveInd]->sprite.getPosition().x == danger.x && dir[moveInd]->sprite.getPosition().y == danger.y)
                {
					dir.erase(dir.begin() + moveInd);
                    found = true;
                    break;
                }
            }
            if (found)
            {
                break;
            }
        }
    }
}

std::string Objects::forDevNameToString(Objects::PieceName name)
{
    if (name == Objects::KING)
    {
        return "KING";
    }
    else if (name == Objects::QUEEN)
    {
        return "QUEEN";
    }
    else if (name == Objects::KNIGHT)
    {
        return "KNIGHT";
    }
    else if (name == Objects::ROOK)
    {
        return "ROOK";
    }
    else if (name == Objects::BISHOP)
    {
        return "BISHOP";

    }
    else if (name == Objects::PAWN)
    {
        return "PAWN";
    }
    else
    {
        return "null";
    }
}

char Objects::forDevColorToChar(Objects::PieceColor color)
{
    if (color == Objects::WHITE)
    {
        return 'w';
    }
    else if (color == Objects::BLACK)
    {
        return 'b';
    }
    else if (color == Objects::NONE)
    {
        return 'n';
    }
    else
    {
        return 'q';
    }
}

std::string Objects::forDevDirToString(Objects::Directions dir)
{
    if (dir == Objects::NORTH)
    {
        return "NORTH";
    }
    else if (dir == Objects::SOUTH)
    {
        return "SOUTH";
    }
    else if (dir == Objects::EAST)
    {
        return "EAST";
    }
    else if (dir == Objects::WEST)
    {
        return "WEST";
    }
    else if (dir == Objects::SOUTHEAST)
    {
        return "SOUTHEAST";
    }
    else if (dir == Objects::SOUTHWEST)
    {
        return "SOUTHWEST";
    }
    else if (dir == Objects::NORTHEAST)
    {
        return "NORTHEAST";
    }
    else if (dir == Objects::NORTHWEST)
    {
        return "NORTHWEST";
    }
    else
    {
        return "nulldir";
    }
}

void Objects::Piece::checkPawnAttack(Objects::Board& board, int x, int direction, bool onlyAttack)
{
    int y = 0;
    if (this->color == Objects::BLACK)
    {
        y = 1;
    }
    else
    {
        y = -1;
    }

    sf::Vector2i cell(this->sprite.getPosition().x + (cellWidth * x), this->sprite.getPosition().y + (cellHeight * y));
    auto targetCell = board.getPieceByMouse(cell);
    
    if (targetCell != nullptr)
    {
        if (onlyAttack)
        {
            this->createLegalMove(direction, targetCell);
        }
        else if ((targetCell->color != this->color || targetCell->color == Objects::NONE) && this->enpassantRight && x == 1)
        {
            this->createLegalMove(direction, targetCell, true);
        }
        else if ((targetCell->color != this->color || targetCell->color == Objects::NONE) && this->enpassantLeft && x == -1)
        {
            this->createLegalMove(direction, targetCell, true);
        }
        else if (targetCell->color != this->color && targetCell->color != Objects::NONE)
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

void Objects::Piece::getKnightMoves(Objects::Board& board)
{
    std::vector<Directions> directions;
    uint8_t amount{};
    short multiplierX{}, multiplierY{};
    short offsetX{}, offsetY{};
    sf::Vector2i cell{};
    std::shared_ptr<Objects::Piece> targetCell{};

    Objects::getMoveProperties(std::make_shared<Objects::Piece>(this), directions, amount);
    this->legalMoves.resize(directions.size());
    
    for (auto& direction : directions)
    {
        cell.x = (int)this->sprite.getPosition().x;
        cell.y = (int)this->sprite.getPosition().y;

        multiplierX = 0; multiplierY = 0;
        offsetX = 0; offsetY = 0;

        Objects::getDirectionMultiplier(direction, multiplierX, multiplierY);
        
        multiplierX *= 2; multiplierY *= 2;

        if (multiplierX == 0)
        {
            offsetX = cellWidth;
            offsetY = 0;
        }
        else if (multiplierY == 0)
        {
            offsetX = 0;
            offsetY = cellHeight;
        }

        for (int i = 0; i < 2; i++)
        {
            cell.x = (int)this->sprite.getPosition().x;
            cell.y = (int)this->sprite.getPosition().y;
            if (i == 1)
            {
                offsetX *= -1; offsetY *= -1;
            }
            cell.x += (cellWidth * multiplierX) + offsetX;
            cell.y += (cellHeight * multiplierY) + offsetY;
            targetCell = board.getPieceByMouse(cell);
            if (board.isTargetOnBoard(targetCell))
            {
                Objects::isTargetCellValid(targetCell, std::make_shared<Objects::Piece>(this), direction);
            }
            else
            {
                continue;
            }
        }
    }
}

void Objects::Piece::createLegalMove(uint8_t direction, std::shared_ptr<Objects::Piece> targetCell, bool enpassant)
{
    this->legalMoves[direction].emplace_back(Objects::makeIndicator(targetCell->sprite, targetCell->name, enpassant));
}

bool Objects::Piece::isTargetInMoves(std::shared_ptr<Objects::Piece> target)
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

void Objects::Piece::resetPiece()
{
    this->enpassantLeft = false;
    this->enpassantRight = false;
    this->firstMove = true;
    this->isPinned = false;
    this->pinnedPiece = nullptr;
    this->legalMoves.clear();
}

bool Objects::Piece::isMoveEnpassant()
{
    for (auto& dir : this->legalMoves)
    {
        for (auto& move : dir)
        {
            if (move->sprite.getPosition() == this->sprite.getPosition())
            {
                return move->enpassant;
            }
        }
    }
}

void Objects::getDirectionMultiplier(Objects::Directions direction, short& x, short& y)
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
    auto temp = Assets::getObjectTexture("cell");

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
    this->enpassantLeft = false;
    this->enpassantRight = false;
}

Objects::Piece::Piece(PieceName name, PieceColor color)
{
    this->name = name;
    this->color = color;
    this->firstMove = true;
    this->isPinned = false;
    this->enpassantLeft = false;
    this->enpassantRight = false;
}

Objects::Piece::Piece(PieceName name, PieceColor color, const sf::Texture& texture)
{
    this->name = name;
    this->color = color;
    this->firstMove = true;
    this->isPinned = false;
    this->setTexture(texture);
    this->enpassantLeft = false;
    this->enpassantRight = false;
}

Objects::PieceName Objects::convertStringToPieceName(const std::string& name)
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

void Objects::getMoveProperties(std::shared_ptr<Objects::Piece> piece, std::vector<Objects::Directions>& directions, uint8_t& amount)
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
        case Objects::KNIGHT:
        {
            directions = { Objects::NORTH, Objects::WEST, Objects::EAST, Objects::SOUTH };
            amount = 0;
            break;
        }
    }
}



void Objects::Board::snapPieceToTile(std::shared_ptr<Objects::Piece> piece, float x, float y)
{
    //get the piece pos and snap it to the closest tile
    //to get the closest tile: get the distance between piece and current tile the compare that with a range and see if its in it
    
    if (x != -1 && y != -1)
    {
        piece->sprite.setPosition(x, y);
    }
    else
    {
        for (auto& tile : this->tilePoints)
        {
            float distanceX = std::abs(tile[0] - piece->sprite.getPosition().x);
            float distanceY = std::abs(tile[1] - piece->sprite.getPosition().y);
            if (distanceX < cellWidth / 2 && distanceY < cellHeight / 2)
            {
                piece->sprite.setPosition(tile[0], tile[1]);
                return;
            }
        }
    }
}

bool Objects::Board::isTargetOnBoard(std::shared_ptr<Objects::Piece> piece)
{
    if (piece == nullptr)
    {
        return false;
    }
    if ((piece->sprite.getPosition().x < 0 || piece->sprite.getPosition().x > windowWidth) && (piece->sprite.getPosition().y < 0 || piece->sprite.getPosition().y > windowHeight))
    {
        return false;
    }
    return true;
}

void Objects::Board::setAllEnpassantFalse()
{
    for (auto& piece : this->onBoard)
    {
        if (piece->name == Objects::PAWN)
        {
            piece->enpassantLeft = false;
            piece->enpassantRight = false;
        }
    }
}

void Objects::Board::removePiece(std::shared_ptr<Piece> piece)
{
    piece->deletePiece();
}

Objects::Board::Board(std::shared_ptr<Assets::ObjectTexture> objTexture)
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

std::shared_ptr<Objects::Piece> Objects::Board::getPieceByMouse(sf::Vector2i& mousePos, std::shared_ptr<Objects::Piece> skipPiece)
{
    for (auto& piece : this->onBoard)
    {
        if (skipPiece != nullptr && skipPiece == piece)
        {
            continue;
        }
        if (piece->sprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
        {
            return piece;
        }
    }
    return nullptr;
}

void Objects::Board::checkEnpassant(std::shared_ptr<Objects::Piece> currentPiece)
{
    if (!currentPiece->firstMove)
    {
        return;
    }
    std::shared_ptr<Objects::Piece> targetCell{};
    sf::Vector2i cell(currentPiece->sprite.getPosition().x + cellWidth, currentPiece->sprite.getPosition().y);
    
    targetCell = this->getPieceByMouse(cell);
    
    if (targetCell != nullptr && targetCell->name == Objects::PAWN && targetCell->color != currentPiece->color && targetCell->color != Objects::NONE)
    {
        targetCell->enpassantLeft = true;
    }
    
    cell.x = currentPiece->sprite.getPosition().x - cellWidth;
    targetCell = this->getPieceByMouse(cell);
    
    if (targetCell != nullptr && targetCell->name == Objects::PAWN && targetCell->color != currentPiece->color && targetCell->color != Objects::NONE)
    {
        targetCell->enpassantRight = true;
    }
}

void Objects::Board::startingPosition()
{
    for (int index = 0; index < this->tilePoints.size(); index++) // using the same index for two vector since the sizes are always the same
    {
        this->snapPieceToTile(this->onBoard[index], this->tilePoints[index][0], this->tilePoints[index][1]);
        if (this->onBoard[index]->name != Objects::CELL && this->onBoard[index]->name != Objects::INVALID_NAME)
        {
            this->onBoard[index]->resetPiece();
        }
    }
}

std::shared_ptr<Objects::Piece> Objects::Board::checkPromotion()
{
    for (auto& piece : this->onBoard)
    {
        if (piece->name == Objects::PAWN && (piece->sprite.getPosition().y == this->tilePoints[0][1] || piece->sprite.getPosition().y == this->tilePoints[this->tilePoints.size()-1][1]))
        {
            return piece;
        }
    }
    return nullptr;
}

bool Objects::Board::checkForCheck(std::shared_ptr<Objects::Piece> piece, std::shared_ptr<Objects::Piece> king, std::vector<std::shared_ptr<Objects::Indicator>>& checkLine)
{
    std::vector<Objects::Indicator*> moveCollector;
    for (auto& dir : piece->legalMoves)
    {
        for (int moveInd = 0; moveInd < dir.size(); moveInd++)
        {
            if (king->sprite.getPosition() == dir[moveInd]->sprite.getPosition())
            {
                moveCollector.emplace_back(Objects::makeIndicator(piece->sprite, piece->name));
                checkLine.insert(checkLine.begin(), moveCollector.begin(), moveCollector.end());
                return true;
            }
            moveCollector.emplace_back(dir[moveInd]);
        }
        moveCollector.clear();
    }
    return false;
}

void Objects::Board::getBlockingPieces(short turn, std::vector<std::shared_ptr<Objects::Indicator>>& checkLine)
{
	uint8_t firstInd{}, lastInd{};
    uint8_t dirIndex = 0;
    std::vector<std::vector<std::shared_ptr<Objects::Indicator>>> tempLegalMoves{};
    if (turn == 1)
    {
        firstInd = firstWhiteIndex;
        lastInd = lastWhiteIndex;
    }
    else
    {
        firstInd = firstBlackIndex;
        lastInd = lastBlackIndex;
    }
	for (uint8_t i = firstInd; i < lastInd; i++)
	{
        tempLegalMoves.resize(8);
		if (this->onBoard[i]->name == Objects::KING)
		{
			continue;
		}
		this->onBoard[i]->getLegalMoves(*this);
        for (auto& dir : this->onBoard[i]->legalMoves)
		{
            for (auto& move : dir)
			{
				for (auto& check : checkLine)
				{
                    if (move->sprite.getPosition() == check->sprite.getPosition())
					{
                        tempLegalMoves[dirIndex].emplace_back(check);
					}
				}
			}
            dirIndex++;
		}
        dirIndex = 0;
		this->onBoard[i]->deleteLegalMoves();
        this->onBoard[i]->legalMoves = tempLegalMoves;
        tempLegalMoves.clear();
	}
}

bool Objects::Board::canBlock(std::shared_ptr<Objects::Piece> piece)
{
    if (piece->legalMoves.size() != 0)
    {
        return true;
    }
    return false;
}

std::shared_ptr<Objects::Piece> Objects::Board::getKingByColor(Objects::PieceColor color)
{
    if (color == Objects::BLACK)
    {
        return this->onBoard[blackKingIndex];
    }
    else if (color == Objects::WHITE)
    {
        return this->onBoard[whiteKingIndex];
    }
}

void Objects::Board::deleteAllMoves()
{
    for (auto& piece : this->onBoard)
    {
        piece->deleteLegalMoves();
    }
}

void Objects::Board::removeEnpassantPiece(sf::Vector2f pos, short turn)
{
    sf::Vector2i cell(pos.x, pos.y + (cellHeight * turn));
    auto pieceToDelete = this->getPieceByMouse(cell);
    this->removePiece(pieceToDelete);
}