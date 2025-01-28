#include <iostream>
#include "SFML/Graphics.hpp"
#include "load.cpp"

using namespace sf;
using namespace std;

//ACTIONS--------------
const string regularAction = "regular";
const string attackAction = "attacking";
const string enpassantAction = "enpassant";
const string blockingAction = "blocking";
const string castlingAction = "castle";
const string noneAction = "none";

//NAMES----------------
const string pawnName = "pawn";
const string rookName = "rook";
const string bishopName = "bishop";
const string knightName = "knight";
const string queenName = "queen";
const string kingName = "king";
const string cellName = "cell";

//COLORS---------------
const char whiteColor = 'w';
const char blackColor = 'b';
const char noneColor = 'n';

struct Indicator
{
	int x, y;
	string action;
	Sprite ind;
};

struct Piece
{
	int x, y, direction;
	char color;
	bool firstMove = true;
	bool isPinned = false;
	int pinnedByIndex;
	Sprite piece;
	string name;
	vector<vector<Indicator>> legalMoves;
};

//OTHER VARIABLES---------------
const float					endScreenX = 150, endScreenY = 100;
const float					ratio = 1.4f;
const int					width = 532 * ratio, height = 532 * ratio;  //532 small, 863 big
const float					pieceSize = 64.f * ratio;                 // 64 small, 104 big
int							activePieceIndex = 0, selectedCellIndex, blockingPieceIndex;
int							turn = 1; //1 - white; (-1) - black
int							wkingIndex = 60, bkingIndex = 4;
string						activeAction;
bool						enpassantTurn = false;
bool						check = false;
bool						checkmate = false;
bool						stalemate = false;
bool						pieceSelected = false;
bool						checkmateInspection = false;
bool						stalemateInspection = false;
Sprite						sMark;
Sprite						sBoard;
Sprite						sWhiteWin;
Sprite						sBlackWin;
Sprite						sStalemate;
RenderWindow				window(VideoMode(width, height), "chess");
LoadAssets					texture;
vector<Piece>				board;
vector<vector<Indicator>>	check_line;
vector<vector<Indicator>>	check_line_for_king;
vector<vector<Indicator>>	pinning_line;
vector<int>					enpassantIndexes;
//----------------------------------------------------------------

void			initializeGame();
Piece			createPiece(int x, int y, char color, string name, int direction);
void			spriteProperties();
bool			isClickOnWindow();
vector<int>		getPossibleCheckGivingPieceIndex(char allyColor);
bool			isSelectedCellCastling();
char			colorInverze(char color);
int				getPieceByMouse(int x, int y);
int				getIndexByCalculator(int x, int y);
int				getOpposingKingIndex(char allyColor);
vector<int>		getMultiplier(int direction);
string			getAction(int x, int y);
char			convertTurnToColor();
void			calculateMoves(int customIndex, bool custom = false, bool noLegalmoveCreation = false, bool forCheck = false);
void			createLegalMoves(bool noLegalmoveCreation);
void			refreshGameFrames();
void			changePiecePosition();
vector<char>	getCastlingSides(int activePiece);
Indicator		createIndicator(int baseX, int baseY, int plusForX, int plusForY, string action);
void			clearLegalMoves(int index);
void			createCastlingSides(int customIndex);
bool			checkForCheckmate();
bool			checkForStalemate();
void			defineIndexesForEnemySide(char color, int& startingIndex, int& finishingIndex);
void			gettingUnsortedCords(int customIndex, vector<vector<int>>& cords);
void			sortCordsForKing(int& startingIndex, int& finishingIndex, vector<vector<int>>& cords);
void			generatingLegalMovesForKing(int customIndex, vector<vector<int>>& cords);
void			getCheckAndPinningLines(vector<int>& checkGivingIndexes);
void			inCheckLineCordDelete(vector<vector<int>>& cords);
void			resetIsPinnedValue(char color);
void			checkmateScreen(char winningColor);
void			stalemateScreen();
void			checkPromotion();
void			checkTargetCellForEnpassant(int index, int pieceX, int pieceY, char color);
void			checkCastlingSide(int activePiece, int sideRook, vector<char>& castle, char side);
bool			isCellInEnemyLegalMoves(int targetIndex);
bool			isLegalMovesEmpty(int index);


void			kingMoves(int customIndex, bool skipCordSorting);
void			findEnpassant(int pieceX, int pieceY, char color);
void			theCellAdjacentToPawn(int pieceX, int pieceY, char color, char side, bool custom = false, int customIndex = 100);
void			theCellInFrontOfPawn(int customIndex, int pieceX, int pieceY, char color, int ammount);
int				cordChangeForPawn(int cord, int multiplier, char color, bool forX);
void			pawnMoves(int customIndex, bool custom);
void			knightMoves(int customIndex);
void			moveCalcForRookBishopOrQueen(int customIndex, bool custom, string pieceName);
void			calculateMovesForPinnedOrBlocking(int index, vector<vector<Indicator>> targetVector, bool noMoves);

//----------------------------------------------------------------


//MAIN LOOP-------------------------------------------------------
int main()
{
	texture.loadTextures();
	window.display();
	initializeGame();
	while (window.isOpen()) 
	{
		Event e;
		while (window.pollEvent(e)) 
		{
			if (e.type == Event::Closed) 
			{
				window.close();
			}
			
			if (!stalemateInspection)
			{
				stalemateInspection = true;
				cout << "-----------------------------------------------------------" << endl;
				if (checkForStalemate())
				{
					stalemate = true;
					break;
				}
			}
			if (check && !checkmateInspection)
			{
				checkmateInspection = true;
				if (checkForCheckmate())
				{
					checkmate = true;
					break;
				}
			}
			if (Mouse::isButtonPressed(Mouse::Left) && isClickOnWindow()) //SELECTING A PIECE
			{
				clearLegalMoves(activePieceIndex);
				activePieceIndex = getPieceByMouse(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
				if (activePieceIndex != 100)
				{
					if (board[activePieceIndex].color == convertTurnToColor())
					{
						sMark.setPosition(board[activePieceIndex].x * pieceSize, board[activePieceIndex].y * pieceSize);
						if (board[activePieceIndex].isPinned)
						{
							calculateMovesForPinnedOrBlocking(activePieceIndex, pinning_line, false);
						}
						else if (check)
						{
							calculateMovesForPinnedOrBlocking(activePieceIndex, check_line, false);
						}
						else
						{
							calculateMoves(activePieceIndex);
						}
					}
					else
					{
						sMark.setPosition(10 * pieceSize, 10 * pieceSize);
					}
				}
			}

			if (Mouse::isButtonPressed(Mouse::Right) && isClickOnWindow()) //MOVING A PIECE
			{
				selectedCellIndex = getPieceByMouse(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
				activeAction = getAction(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
				if (activeAction != noneAction)
				{
					check = false;
					changePiecePosition();
				}
			}
			refreshGameFrames();
		}
		
		if (checkmate)
		{
			refreshGameFrames();
			checkmateScreen(colorInverze(convertTurnToColor()));
			break;
		}
		if (stalemate)
		{
			refreshGameFrames();
			stalemateScreen();
			break;
		}
	}
	while (window.isOpen())
	{
		if (Mouse::isButtonPressed(Mouse::Left) && isClickOnWindow())
		{
			window.close();
		}
	}
	return 0;
}
//MAIN LOOP-------------------------------------------------------




//PREGAME---------------------------------------------------------------
Piece createPiece(int x, int y, char color, string name, int direction)
{
	Piece tempPiece;
	tempPiece.x = x;
	tempPiece.y = y;
	tempPiece.color = color;
	tempPiece.name = name;
	tempPiece.direction = direction;
	return tempPiece;
}

void spriteProperties()
{
	string name;
	char color;

	sBoard.setTexture(texture.board);
	sBoard.setScale(ratio, ratio);
	window.draw(sBoard);

	sMark.setTexture(texture.mark);
	sMark.setScale(ratio, ratio);
	sMark.setColor(Color(255, 255, 255, 200));
	sMark.setPosition(10 * pieceSize, 10 * pieceSize);
	window.draw(sMark);

	for (int i = 0; i < board.size(); i++)
	{
		name = board[i].name;
		color = board[i].color;
		if (board[i].color == blackColor)
		{
			if (board[i].name == rookName)
			{
				board[i].piece.setTexture(texture.brook);
			}
			else if (board[i].name == bishopName)
			{
				board[i].piece.setTexture(texture.bbishop);
			}
			else if (board[i].name == knightName)
			{
				board[i].piece.setTexture(texture.bknight);
			}
			else if (board[i].name == kingName)
			{
				board[i].piece.setTexture(texture.bking);
			}
			else if (board[i].name == queenName)
			{
				board[i].piece.setTexture(texture.bqueen);
			}
			else if (board[i].name == pawnName)
			{
				board[i].piece.setTexture(texture.bpawn);
			}
		}
		else if (board[i].color == whiteColor)
		{
			if (board[i].name == rookName)
			{
				board[i].piece.setTexture(texture.wrook);
			}
			else if (board[i].name == bishopName)
			{
				board[i].piece.setTexture(texture.wbishop);
			}
			else if (board[i].name == knightName)
			{
				board[i].piece.setTexture(texture.wknight);
			}
			else if (board[i].name == kingName)
			{
				board[i].piece.setTexture(texture.wking);
			}
			else if (board[i].name == queenName)
			{
				board[i].piece.setTexture(texture.wqueen);
			}
			else if (board[i].name == pawnName)
			{
				board[i].piece.setTexture(texture.wpawn);
			}
		}
		else
		{
			board[i].piece.setTexture(texture.cell);
		}
		board[i].piece.setScale(ratio, ratio);
		board[i].piece.setPosition(pieceSize * board[i].x, pieceSize * board[i].y);
		window.draw(board[i].piece);
	}
	window.display();
}

void initializeGame()
{
	string name;
	char color = noneColor;
	int direction = 0;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			switch (y)
			{
			case 0: //BLACK SIDE
				if (x == 0 || x == 7)
				{
					name = rookName;
					color = blackColor;
					direction = 4;
				}
				else if (x == 1 || x == 6)
				{
					name = knightName;
					color = blackColor;
					direction = 4;
				}
				else if (x == 2 || x == 5)
				{
					name = bishopName;
					color = blackColor;
					direction = 4;
				}
				else if (x == 3)
				{
					name = queenName;
					color = blackColor;
					direction = 8;
				}
				else if (x == 4)
				{
					name = kingName;
					color = blackColor;
					direction = 8;
				}
				break;
			/*case 1:
				name = pawnName;
				color = blackColor;
				direction = 1;
				break;*/

			case 7: //WHITE SIDE
				if (x == 0 || x == 7)
				{
					name = rookName;
					color = whiteColor;
					direction = 4;
				}
				else if (x == 1 || x == 6)
				{
					name = knightName;
					color = whiteColor;
					direction = 4;
				}
				else if (x == 2 || x == 5)
				{
					name = bishopName;
					color = whiteColor;
					direction = 4;
				}
				else if (x == 3)
				{
					name = queenName;
					color = whiteColor;
					direction = 8;
				}
				else if (x == 4)
				{
					name = kingName;
					color = whiteColor;
					direction = 8;
				}
				break;
			case 6:
				name = pawnName;
				color = whiteColor;
				direction = 1;
				break;

			default: //EMPTY CELLS
				name = cellName;
				color = noneColor;
				direction = 0;
				break;
			}
			board.push_back(createPiece(x, y, color, name, direction));
		}
	}
	spriteProperties();
}
//PREGAME---------------------------------------------------------------



//FUNCTIONS-------------------------------------------------------------
bool isClickOnWindow()
{
	if ((Mouse::getPosition().x < window.getPosition().x + width && Mouse::getPosition().y < window.getPosition().y + height) && (Mouse::getPosition().x > window.getPosition().x && Mouse::getPosition().y > window.getPosition().y))
	{
		return true;
	}
	return false;
}

int getPieceByMouse(int x, int y) {
	for (int i = 0; i < board.size(); i++)
	{
		if (board[i].piece.getGlobalBounds().contains(x, y))
		{
			return i;
		}
	}
	return 100;
}

char convertTurnToColor()
{
	switch (turn)
	{
	case 1:
		return whiteColor;
	case -1:
		return blackColor;
	}
}

void createLegalMoves(bool noLegalmoveCreation)
{
	if (!noLegalmoveCreation)
	{
		for (int direction = 0; direction < board[activePieceIndex].legalMoves.size(); direction++)
		{
			for (int i = 0; i < board[activePieceIndex].legalMoves[direction].size(); i++)
			{
				board[activePieceIndex].legalMoves[direction][i].ind.setTexture(texture.legalMoveInd);
				board[activePieceIndex].legalMoves[direction][i].ind.setPosition(board[activePieceIndex].legalMoves[direction][i].x * pieceSize, board[activePieceIndex].legalMoves[direction][i].y * pieceSize);
				board[activePieceIndex].legalMoves[direction][i].ind.setScale(ratio, ratio);
				window.draw(board[activePieceIndex].legalMoves[direction][i].ind);
			}
		}
	}
}

int getIndexByCalculator(int x, int y)
{
	for (int i = 0; i < board.size(); i++)
	{
		if (board[i].x == x && board[i].y == y)
		{
			return i;
		}
	}
	return 100;
}

string getAction(int x, int y)
{
	for (int direction = 0; direction < board[activePieceIndex].legalMoves.size(); direction++)
	{
		for (int j = 0; j < board[activePieceIndex].legalMoves[direction].size(); j++)
		{
			if (board[activePieceIndex].legalMoves[direction][j].ind.getGlobalBounds().contains(x,y))
			{
				return board[activePieceIndex].legalMoves[direction][j].action;
			}
		}
	}
	return noneAction;
}

void refreshGameFrames()
{
	window.clear();
	window.draw(sBoard);
	window.draw(sMark);
	for (int i = 0; i < board.size(); i++)
	{
		if (!board[i].legalMoves.empty())
		{
			for (int direction = 0; direction < board[i].legalMoves.size(); direction++)
			{
				for (int j = 0; j < board[i].legalMoves[direction].size(); j++)
				{
					window.draw(board[i].legalMoves[direction][j].ind);
				}
			}
		}
		window.draw(board[i].piece);
	}
	if (activePieceIndex != 100)
	{
		if (!board[activePieceIndex].legalMoves.empty())
		{
			for (int direction = 0; direction < board[activePieceIndex].legalMoves.size(); direction++)
			{
				for (int j = 0; j < board[activePieceIndex].legalMoves[direction].size(); j++)
				{
					window.draw(board[activePieceIndex].legalMoves[direction][j].ind);
				}
			}
		}
	}
	window.display();
}

char colorInverze(char color)
{
	if (color == whiteColor)
	{
		return blackColor;
	}
	else
	{
		return whiteColor;
	}
}

vector<int> getPossibleCheckGivingPieceIndex(char allyColor)
{
	int opposingKingIndex = getOpposingKingIndex(allyColor);
	int startingIndex, finishingIndex;
	defineIndexesForEnemySide(colorInverze(allyColor), startingIndex, finishingIndex);
	vector<int> returnIndexes;
	bool indexAdded = false;
	check_line.clear();
	check_line_for_king.clear();
	pinning_line.clear();
	for (int index = startingIndex; index < finishingIndex; index++)
	{
		if (board[index].name == kingName)
		{
			continue;
		}
		calculateMoves(index, true, true, true);
		indexAdded = false;
		for (int direction = 0; direction < board[index].legalMoves.size(); direction++)
		{
			for (int i = 0; i < board[index].legalMoves[direction].size(); i++)
			{	
				if (board[index].legalMoves[direction][i].x == board[opposingKingIndex].x && board[index].legalMoves[direction][i].y == board[opposingKingIndex].y)
				{
					returnIndexes.push_back(index);
					indexAdded = true;
					break;
				}
			}
			if (indexAdded)
			{
				break;
			}
		}
		clearLegalMoves(index);
	}
	return returnIndexes;
}

int getOpposingKingIndex(char allyColor)
{
	if (allyColor == blackColor)
	{
		return wkingIndex;
	}
	else
	{
		return bkingIndex;
	}
}

bool isSelectedCellCastling()
{
	for (int direction = 0; direction < board[activePieceIndex].legalMoves.size(); direction++)
	{
		for (int i = 0; i < board[activePieceIndex].legalMoves[direction].size(); i++)
		{
			if (board[activePieceIndex].x == board[activePieceIndex].legalMoves[direction][i].x && board[activePieceIndex].y == board[activePieceIndex].legalMoves[direction][i].y && board[activePieceIndex].legalMoves[direction][i].action == castlingAction)
			{
				return true;
			}
		}
	}
	return false;
}

bool isCellInEnemyLegalMoves(int targetIndex)
{
	char color = convertTurnToColor();
	int start, finish;
	defineIndexesForEnemySide(color, start, finish);
	for (int index = start; index < finish; index++)
	{
		if (board[index].name == kingName)
		{
			continue;
		}
		calculateMoves(index, true, true);
		for (auto line : board[index].legalMoves)
		{
			for (auto ind : line)
			{
				if (ind.x == board[targetIndex].x && ind.y == board[targetIndex].y)
				{
					return true;
				}
			}
		}
		clearLegalMoves(index);
	}
	return false;
}

void checkCastlingSide(int activePiece, int sideRook, vector<char>& castle, char side)
{
	int index, passedCellCounter = 0, sideMultiplier = 1, targetCellAmmount = 0;
	if (side == 'q')
	{
		sideMultiplier = -1;
		targetCellAmmount = 3;
	}
	else if (side == 'k')
	{
		sideMultiplier = 1;
		targetCellAmmount = 2;
	}
	if (board[activePiece].firstMove && board[sideRook].firstMove)
	{
		for (int i = 0; i < targetCellAmmount; i++)
		{
			index = getIndexByCalculator((board[activePiece].x + ((i + 1) * sideMultiplier)), board[activePiece].y);
			if (board[index].color == noneColor && !isCellInEnemyLegalMoves(index))
			{
				passedCellCounter++;
			}
			else
			{
				return;
			}
		}
		if (passedCellCounter == targetCellAmmount)
		{
			castle.push_back(side);
		}
	}
}

vector<char> getCastlingSides(int activePiece)
{
	int queenSideRook = 0, kingSideRook = 0;
	vector<char> castle;
	switch (activePiece)
	{
	case 4:
		queenSideRook = 0;
		kingSideRook = 7;
		break;
	case 60:
		queenSideRook = 56;
		kingSideRook = 63;
		break;
	}
	checkCastlingSide(activePiece, queenSideRook, castle, 'q');
	checkCastlingSide(activePiece, kingSideRook, castle, 'k');
	return castle;
}

Indicator createIndicator(int baseX, int baseY, int plusForX, int plusForY, string action)
{
	Indicator ind;
	ind.x = baseX + plusForX;
	ind.y = baseY + plusForY;
	ind.action = action;
	return ind;
}

void clearLegalMoves(int index)
{	
	if (index == 100)
	{
		return;
	}
	if (board[index].name == pawnName && !board[index].legalMoves.empty() && enpassantTurn)
	{
		Indicator enpassantHolder;
		bool found = false;
		for (int i = 0; i < board[index].legalMoves.size(); i++)
		{
			for (int j = 0; j < board[index].legalMoves[i].size(); j++)
			{
				if (board[index].legalMoves[i][j].action == enpassantAction)
				{
					enpassantHolder = board[index].legalMoves[i][j];
					break;
				}
			}
			if (found)
			{
				break;
			}
		}
		board[index].legalMoves.clear();
		refreshGameFrames();
		board[index].legalMoves.resize(1);
		board[index].legalMoves[0].push_back(enpassantHolder);
	}
	else
	{
		board[index].legalMoves.clear();
	}
}

void moveCalcForRookBishopOrQueen(int customIndex, bool forCheck, string pieceName)
{
	int xMultiplier, yMultiplier;
	int directionStart, directionFinish;
	int targetIndex;
	int targetX, targetY;
	int pieceDirection = -1;
	if (pieceName == rookName)
	{
		directionStart = 0;
		directionFinish = 4;
	}
	else if(pieceName == bishopName)
	{
		directionStart = 4;
		directionFinish = 8;
	}
	else
	{
		directionStart = 0;
		directionFinish = 8;
	}
	for (int direction = directionStart; direction < directionFinish; direction++)
	{
		pieceDirection++;
		targetX = board[customIndex].x;
		targetY = board[customIndex].y;
		xMultiplier = getMultiplier(direction)[0];
		yMultiplier = getMultiplier(direction)[1];
		while (true)
		{
			targetX += xMultiplier;
			targetY += yMultiplier;
			targetIndex = getIndexByCalculator(targetX, targetY);
			if (targetIndex != 100)
			{
				if (board[targetIndex].color == noneColor)
				{
					board[customIndex].legalMoves[pieceDirection].push_back(createIndicator(board[targetIndex].x, board[targetIndex].y, 0, 0, regularAction));
				}
				else if (board[targetIndex].color == board[customIndex].color)
				{
					if (forCheck)
					{
						board[customIndex].legalMoves[pieceDirection].push_back(createIndicator(board[targetIndex].x, board[targetIndex].y, 0, 0, noneAction));
					}
					break;
				}
				else if (board[targetIndex].color != board[customIndex].color)
				{
					if (forCheck)
					{
						board[customIndex].legalMoves[pieceDirection].push_back(createIndicator(board[targetIndex].x, board[targetIndex].y, 0, 0, attackAction));
						continue;
					}
					else
					{
						board[customIndex].legalMoves[pieceDirection].push_back(createIndicator(board[targetIndex].x, board[targetIndex].y, 0, 0, attackAction));
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}

vector<int> getMultiplier(int direction)// adjacent - n, y, b -> (no, yes, both)
{
	int xMultiplier = 0, yMultiplier = 0;
	switch (direction)
	{
	case 0:
		xMultiplier = 0;
		yMultiplier = -1;
		break;
	case 1:
		xMultiplier = -1;
		yMultiplier = 0;
		break;
	case 2:
		xMultiplier = 0;
		yMultiplier = 1;
		break;
	case 3:
		xMultiplier = 1;
		yMultiplier = 0;
		break;
	case 4:
		xMultiplier = -1;
		yMultiplier = -1;
		break;
	case 5:
		xMultiplier = 1;
		yMultiplier = -1;
		break;
	case 6:
		xMultiplier = -1;
		yMultiplier = 1;
		break;
	case 7:
		xMultiplier = 1;
		yMultiplier = 1;
		break;
	}
	return { xMultiplier, yMultiplier };
}

int cordChangeForPawn(int cord, int multiplier, char color, bool forX)
{
	if (forX)
	{
		return cord + (1 * multiplier);
	}
	if (color == whiteColor)
	{
		return cord + ((-1) * multiplier);
	}
	else
	{
		return cord + (1 * multiplier);
	}
}

void defineIndexesForEnemySide(char color, int& startingIndex, int& finishingIndex)
{
	if (color == whiteColor)
	{
		startingIndex = 0;
		finishingIndex = 16;
	}
	else
	{
		startingIndex = 48;
		finishingIndex = 64;
	}
}

void inCheckLineCordDelete(vector<vector<int>>& cords)
{
	for (int line = 0; line < check_line_for_king.size(); line++)
	{
		for (int ind = 0; ind < check_line_for_king[line].size(); ind++)
		{
			for (int cord = 0; cord < cords.size(); cord++)
			{
				if (check_line_for_king[line][ind].x == cords[cord][0] && check_line_for_king[line][ind].y == cords[cord][1])
				{
					cords.erase(cords.begin() + cord);
					break;
				}
			}
		}
	}
}

void sortCordsForKing(int& startingIndex, int& finishingIndex, vector<vector<int>>& cords)
{
	for (int index = startingIndex; index < finishingIndex; index++)
	{
		calculateMoves(index, true, true, false);
		for (int direction = 0; direction < board[index].legalMoves.size(); direction++)
		{
			for (int ind = 0; ind < board[index].legalMoves[direction].size(); ind++)
			{
				for (int cord = 0; cord < cords.size(); cord++)
				{
					if ((board[index].legalMoves[direction][ind].x == cords[cord][0] && board[index].legalMoves[direction][ind].y == cords[cord][1]))
					{
						if (board[index].name == pawnName && board[index].legalMoves[direction][ind].action == attackAction)
						{
							cords.erase(cords.begin() + cord);
							break;
						}
						else
						{
							cords.erase(cords.begin() + cord);
							break;
						}
					}
				}
			}
		}
		clearLegalMoves(index);
	}
}

void checkTargetCellForEnpassant(int index, int pieceX, int pieceY, char color)
{
	if (index != 100)
	{
		if (board[index].name == pawnName && board[index].color != color)
		{
			board[index].legalMoves.resize(1);
			board[index].legalMoves[0].push_back(createIndicator(pieceX, cordChangeForPawn(pieceY, 1, color, false), 0, 0, enpassantAction));
			enpassantTurn = true;
			enpassantIndexes.push_back(index);
		}
	}
}

void findEnpassant(int pieceX, int pieceY, char color)
{
	int index;
	int tempX = pieceX;
	int tempY = pieceY;
	tempY = cordChangeForPawn(pieceY, 2, color, false);

	tempX = cordChangeForPawn(pieceX, 1, color, true);//RIGHT---------
	index = getIndexByCalculator(tempX, tempY);
	checkTargetCellForEnpassant(index, pieceX, pieceY, color);

	tempX = cordChangeForPawn(pieceX, -1, color, true);//LEFT--------
	index = getIndexByCalculator(tempX, tempY);
	checkTargetCellForEnpassant(index, pieceX, pieceY, color);
}

void getCheckAndPinningLines(vector<int>& checkGivingIndexes)
{
	bool kingIn = false;
	int pieceIn = 0 ,targetCell;
	char enemyColor = colorInverze(convertTurnToColor());
	int opposingKingIndex = getOpposingKingIndex(enemyColor);
	resetIsPinnedValue(enemyColor);
	int pinnedIndex = 100;
	vector<Indicator> tempLine;
	for (int index : checkGivingIndexes)
	{
		calculateMoves(index, true, true, true);
		for (int direction = 0; direction < board[index].legalMoves.size(); direction++)
		{
			pieceIn = 0;
			kingIn = false;
			tempLine.clear();
			tempLine.push_back(createIndicator(board[index].x, board[index].y, 0, 0, attackAction));
			for (int ind = 0; ind < board[index].legalMoves[direction].size(); ind++)
			{
				targetCell = getIndexByCalculator(board[index].legalMoves[direction][ind].x, board[index].legalMoves[direction][ind].y);
				if (board[targetCell].color == noneColor)
				{
					tempLine.push_back(board[index].legalMoves[direction][ind]);
				}
				if (board[targetCell].color != enemyColor && board[targetCell].color != noneColor && board[targetCell].name != kingName)
				{
					if (pieceIn >= 1)
					{
						pieceIn++;
						pinnedIndex = 100;
					}
					else
					{
						pieceIn++;
						pinnedIndex = targetCell;
					}
				}
				if (board[targetCell].x == board[opposingKingIndex].x && board[targetCell].y == board[opposingKingIndex].y)
				{
					kingIn = true;
					break;
				}
			}
			if (kingIn && pieceIn == 0)
			{
				check_line.push_back(tempLine);
				check_line_for_king.push_back(board[index].legalMoves[direction]);
				check = true;
			}
			if (kingIn && pieceIn == 1)
			{
				pinning_line.push_back(tempLine);
				board[pinnedIndex].isPinned = true;
			}
		}
		clearLegalMoves(index);
	}
}

void resetIsPinnedValue(char color)
{
	int startingIndex, finishingIndex;
	defineIndexesForEnemySide(color, startingIndex, finishingIndex);
	for (int index = startingIndex; index < finishingIndex; index++)
	{
		board[index].isPinned = false;
	}
}

bool checkForCheckmate()
{
	char allyColor = colorInverze(convertTurnToColor());
	int startingIndex, finsihingIndex;
	defineIndexesForEnemySide(allyColor, startingIndex, finsihingIndex);
	for (int index = startingIndex; index < finsihingIndex; index++)
	{
		calculateMovesForPinnedOrBlocking(index, check_line, true);
		if (!board[index].legalMoves.empty())
		{
			return false;
		}
	}
	return true;
}

bool isLegalMovesEmpty(int index)
{
	for (auto line : board[index].legalMoves)
	{
		cout << "line size " << line.size() << endl;
		if (!line.empty())
		{
			return false;
		}
	}
	return true;
}

bool checkForStalemate()
{
	char turnInColor = convertTurnToColor();
	int startingIndex, finishingIndex;
	defineIndexesForEnemySide(turnInColor, startingIndex, finishingIndex);
	for (int index = startingIndex; index < finishingIndex; index++)
	{
		calculateMoves(index, true, true);
		if (!isLegalMovesEmpty(index))
		{
			cout << "false" << endl;
			return false;
		}
		clearLegalMoves(index);
	}
	cout << "true" << endl;
	return true;
}

void checkmateScreen(char winningColor)
{
	if (winningColor == whiteColor)
	{
		sWhiteWin.setTexture(texture.white_win);
		sWhiteWin.setOrigin(endScreenX/2, endScreenY / 2);
		sWhiteWin.setPosition((float)width/2, (float)height/2);
		window.draw(sWhiteWin);
	}
	else if (winningColor == blackColor)
	{
		sBlackWin.setTexture(texture.black_win);
		sBlackWin.setOrigin(endScreenX / 2, endScreenY / 2);
		sBlackWin.setPosition((float)width / 2, (float)height / 2);
		window.draw(sBlackWin);
	}
	window.display();
}

void stalemateScreen()
{
	sStalemate.setTexture(texture.stalemate);
	sStalemate.setOrigin(endScreenX / 2, endScreenY / 2);
	sStalemate.setPosition((float)width / 2, (float)height / 2);
	window.draw(sStalemate);
	window.display();
}

void checkPromotion()
{
	if (board[activePieceIndex].name == pawnName && (board[activePieceIndex].y == 7 || board[activePieceIndex].y == 0))
	{
		board[activePieceIndex].name = queenName;
		if (board[activePieceIndex].color == whiteColor)
		{
			board[activePieceIndex].piece.setTexture(texture.wqueen);
		}
		else
		{
			board[activePieceIndex].piece.setTexture(texture.bqueen);
		}
	}
}
//FUNCTIONS-------------------------------------------------------------



//MOVES-----------------------------------------------------------------
void calculateMoves(int index, bool onlyAttackForPawn, bool noLegalmoveCreation, bool noCollisonBishopRook)
{
	if (board[index].name == rookName)
	{
		board[index].legalMoves.resize(4);
		moveCalcForRookBishopOrQueen(index, noCollisonBishopRook, rookName);
		createLegalMoves(noLegalmoveCreation);
	}
	else if (board[index].name == bishopName)
	{
		board[index].legalMoves.resize(4);
		moveCalcForRookBishopOrQueen(index, noCollisonBishopRook, bishopName);
		createLegalMoves(noLegalmoveCreation);
	}
	else if (board[index].name == queenName)
	{
		board[index].legalMoves.resize(8);
		moveCalcForRookBishopOrQueen(index, noCollisonBishopRook, queenName);
		createLegalMoves(noLegalmoveCreation);
	}
	else if (board[index].name == knightName)
	{
		board[index].legalMoves.resize(4);
		knightMoves(index);
		createLegalMoves(noLegalmoveCreation);
	}
	else if (board[index].name == pawnName)
	{
		board[index].legalMoves.resize(1);
		pawnMoves(index, onlyAttackForPawn);
		createLegalMoves(noLegalmoveCreation);
	}
	else if (board[index].name == kingName)
	{
		board[index].legalMoves.resize(8);
		kingMoves(index, onlyAttackForPawn);
		createLegalMoves(noLegalmoveCreation);
	}
}

void calculateMovesForPinnedOrBlocking(int index, vector<vector<Indicator>> targetVector, bool noMoves)
{
	calculateMoves(index, false, noMoves, false);
	vector<Indicator> tempMoves;
	if (board[index].legalMoves.empty())
	{
		return;
	}
	if (board[index].name == kingName)
	{
		return;
	}
	for (int line = 0; line < targetVector.size(); line++)
	{
		for (auto ind : targetVector[line])
		{
			for (int direction = 0; direction < board[index].legalMoves.size(); direction++)
			{
				for (auto legalInd : board[index].legalMoves[direction])
				{
					if (ind.x == legalInd.x && ind.y == legalInd.y && ind.action == attackAction && legalInd.action == attackAction)
					{
						tempMoves.push_back(createIndicator(ind.x, ind.y, 0, 0, attackAction));
					}
					else if (ind.x == legalInd.x && ind.y == legalInd.y)
					{
						tempMoves.push_back(createIndicator(ind.x, ind.y, 0, 0, regularAction));
					}
				}
			}
		}
	}
	clearLegalMoves(index);
	if (!tempMoves.empty())
	{
		board[index].legalMoves.resize(1);
		board[index].legalMoves[0] = tempMoves;
		createLegalMoves(noMoves);
	}
}

void knightMoves(int customIndex)
{
	int xMultiplier = 0;
	int yMultiplier = 0;
	int direction = 0; // 0 - N, 1 - W, 2 - S, 3 - E
	int index = 0;
	while (direction != 4)
	{
		int piecex = board[customIndex].x;
		int piecey = board[customIndex].y;
		switch (direction)
		{
		case 0:
			xMultiplier = 0;
			yMultiplier = -2;
			break;
		case 1:
			xMultiplier = -2;
			yMultiplier = 0;
			break;
		case 2:
			xMultiplier = 0;
			yMultiplier = 2;
			break;
		case 3:
			xMultiplier = 2;
			yMultiplier = 0;
			break;
		}
		if (xMultiplier == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				int m = 0;
				if (i == 0)
				{
					m = 1;
				}
				else
				{
					m = -2;
				}
				piecex += m;
				piecey = board[customIndex].y + yMultiplier;
				int index = getIndexByCalculator(piecex, piecey);
				if (index != 100)
				{
					if (board[index].color == noneColor)
					{
						board[customIndex].legalMoves[direction].push_back(createIndicator(board[index].x, board[index].y, 0, 0, regularAction));
					}
					if (board[index].color != board[customIndex].color && board[index].color != noneColor)
					{
						board[customIndex].legalMoves[direction].push_back(createIndicator(board[index].x, board[index].y, 0, 0, attackAction));
					}
				}
			}
		}
		else if (yMultiplier == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				int m = 0;
				if (i == 0)
				{
					m = 1;
				}
				else
				{
					m = -2;
				}
				piecex = board[customIndex].x + xMultiplier;
				piecey += m;
				int index = getIndexByCalculator(piecex, piecey);
				if (index != 100)
				{
					if (board[index].color == noneColor)
					{
						board[customIndex].legalMoves[direction].push_back(createIndicator(board[index].x, board[index].y, 0, 0, regularAction));
					}
					if (board[index].color != board[customIndex].color && board[index].color != noneColor)
					{
						board[customIndex].legalMoves[direction].push_back(createIndicator(board[index].x, board[index].y, 0, 0, attackAction));
					}
				}
			}
		}
		direction++;
	}
}

void theCellInFrontOfPawn(int customIndex, int pieceX, int pieceY, char color, int ammount)
{
	if (ammount == 0)
	{
		return;
	}
	pieceY = cordChangeForPawn(pieceY, 1, color, false); //adds +1 to the y cord
	int index = getIndexByCalculator(pieceX, pieceY);
	if (index != 100)
	{
		if (board[index].color == noneColor)
		{
			board[customIndex].legalMoves[0].push_back(createIndicator(board[index].x, board[index].y, 0, 0, regularAction));
			theCellInFrontOfPawn(customIndex, pieceX, pieceY, color, ammount - 1);
		}
	}
	return;
}

void theCellAdjacentToPawn(int pieceX, int pieceY, char color, char side, bool custom, int customIndex)
{
	if (customIndex == 100)
	{
		customIndex = activePieceIndex;
	}
	int index;
	int sideInNumber = 1;
	if (side == 'l')
	{
		sideInNumber = -1;
	}
	else if (side == 'r')
	{
		sideInNumber = 1;
	}
	pieceY = cordChangeForPawn(pieceY, 1, color, false);
	pieceX = cordChangeForPawn(pieceX, sideInNumber, color, true);
	index = getIndexByCalculator(pieceX, pieceY);
	if (index != 100)
	{
		if (board[index].color != color && board[index].color != noneColor && !custom)
		{
			board[activePieceIndex].legalMoves[0].push_back(createIndicator(board[index].x, board[index].y, 0, 0, attackAction));
		}
		else if (board[index].color != color && custom)
		{
			board[customIndex].legalMoves[0].push_back(createIndicator(board[index].x, board[index].y, 0, 0, attackAction));
		}
	}
}

void pawnMoves(int customIndex, bool custom)
{
	int pieceX = board[customIndex].x;
	int pieceY = board[customIndex].y;
	char color = board[customIndex].color;
	if (custom)
	{
		theCellAdjacentToPawn(pieceX, pieceY, color, 'l', custom, customIndex);
		theCellAdjacentToPawn(pieceX, pieceY, color, 'r', custom, customIndex);
	}
	else if (board[customIndex].firstMove)
	{
		theCellInFrontOfPawn(customIndex, pieceX, pieceY, color, 2);
		theCellAdjacentToPawn(pieceX, pieceY, color, 'l');
		theCellAdjacentToPawn(pieceX, pieceY, color, 'r');
		findEnpassant(pieceX, pieceY, color);
	}
	else if(!board[customIndex].firstMove)
	{
		theCellInFrontOfPawn(customIndex, pieceX, pieceY, color, 1);
		theCellAdjacentToPawn(pieceX, pieceY, color, 'l');
		theCellAdjacentToPawn(pieceX, pieceY, color, 'r');
	}
}

void createCastlingSides(int customIndex)
{
	vector<char> castle = getCastlingSides(activePieceIndex);
	if (!(castle.empty()))
	{
		for (int i = 0; i < castle.size(); i++)
		{
			if (castle[i] == 'q')
			{
				board[customIndex].legalMoves[3].push_back(createIndicator(board[customIndex].x, board[customIndex].y, -2, 0, castlingAction));
			}
			if (castle[i] == 'k')
			{
				board[customIndex].legalMoves[7].push_back(createIndicator(board[customIndex].x, board[customIndex].y, 2, 0, castlingAction));
			}
		}
	}
}

void gettingUnsortedCords(int customIndex, vector<vector<int>>& cords)
{
	vector<int> multipliers;
	int x, y;
	for (int direction = 0; direction < 8; direction++)
	{
		multipliers = getMultiplier(direction);
		x = board[customIndex].x + multipliers[0];
		y = board[customIndex].y + multipliers[1];
		if ((x < 0 || x > 7) || (y < 0 || y > 7))
		{
			continue;
		}
		else
		{
			cords.push_back({ x,y });
		}
	}
}

void generatingLegalMovesForKing(int customIndex, vector<vector<int>>& cords)
{
	int index;
	for (auto cord : cords)
	{
		for (int direction = 0; direction < board[customIndex].legalMoves.size(); direction++)
		{
			index = getIndexByCalculator(cord[0], cord[1]);
			if (index != 100)
			{
				if (board[index].color != board[customIndex].color && board[index].color != noneColor)
				{
					board[customIndex].legalMoves[direction].push_back(createIndicator(cord[0], cord[1], 0, 0, attackAction));
				}
				else if (board[index].color == noneColor)
				{
					board[customIndex].legalMoves[direction].push_back(createIndicator(cord[0], cord[1], 0, 0, regularAction));
				}
				break;
			}
		}
	}
}

void kingMoves(int customIndex, bool skipCordSorting)
{
	int startingIndex = 0;
	int finishingIndex = 0;
	vector<vector<int>> cords;
	defineIndexesForEnemySide(board[customIndex].color, startingIndex, finishingIndex);
	gettingUnsortedCords(customIndex, cords);
	if (!check)
	{
		createCastlingSides(customIndex);
	}
	if (!skipCordSorting)
	{
		inCheckLineCordDelete(cords);
		sortCordsForKing(startingIndex, finishingIndex, cords); 
	}
	generatingLegalMovesForKing(customIndex, cords);
}

void changePiecePosition()
{
	//CHANGING CORDS---------------------
	int tempx, tempy;
	tempx = board[activePieceIndex].x;
	tempy = board[activePieceIndex].y;
	board[activePieceIndex].x = board[selectedCellIndex].x;
	board[activePieceIndex].y = board[selectedCellIndex].y;
	board[selectedCellIndex].x = tempx;
	board[selectedCellIndex].y = tempy;
	board[selectedCellIndex].firstMove = false;

	//CHANGING POSITION------------------------
	board[activePieceIndex].piece.setPosition(board[activePieceIndex].x * pieceSize, board[activePieceIndex].y * pieceSize);
	board[selectedCellIndex].piece.setPosition(board[selectedCellIndex].x * pieceSize, board[selectedCellIndex].y * pieceSize);

	//SPECIAL ACTIONS-------------------------
	if (enpassantTurn && activeAction == regularAction)
	{
		enpassantTurn = false;
		for (int index : enpassantIndexes)
		{
			board[index].legalMoves.clear();
		}
	}
	if (activeAction == attackAction)
	{
		board[selectedCellIndex].name = cellName;
		board[selectedCellIndex].color = noneColor;
		board[selectedCellIndex].piece.setTexture(texture.cell);
	}
	else if (activeAction == enpassantAction)
	{
		enpassantTurn = false;
		int tempY = cordChangeForPawn(board[activePieceIndex].y, -1, board[activePieceIndex].color, false);
		int index = getIndexByCalculator(board[activePieceIndex].x,tempY);
		board[index].color = noneColor;
		board[index].name = cellName;
		board[index].piece.setTexture(texture.cell);
	}

	//PROMOTION------------------
	checkPromotion();

	//CASTLING------------------------
	if (board[activePieceIndex].name == kingName && isSelectedCellCastling())
	{
		int rookIndex = 0, rookNewPosX = 0;
		int emptyCellX = 0, emptyCellY = 0;
		int kingX = board[activePieceIndex].x;
		int kingY = board[activePieceIndex].y;
		if (kingX == 6 && kingY == 7)
		{
			rookIndex = 63;
			rookNewPosX = 5;
			emptyCellX = 5;
			emptyCellY = 7;
		}
		else if (kingX == 2 && kingY == 7)
		{
			rookIndex = 56;
			rookNewPosX = 3;
			emptyCellX = 3;
			emptyCellY = 7;
		}
		else if (kingX == 6 && kingY == 0)
		{
			rookIndex = 7;
			rookNewPosX = 5;
			emptyCellX = 5;
			emptyCellY = 0;
		}
		else if (kingX == 2 && kingY == 0)
		{
			rookIndex = 0;
			rookNewPosX = 3;
			emptyCellX = 3;
			emptyCellY = 0;
		}
		int emptyCellIndex = getIndexByCalculator(emptyCellX, emptyCellY);
		board[emptyCellIndex].x = board[rookIndex].x;
		board[emptyCellIndex].y = board[rookIndex].y;
		board[rookIndex].x = rookNewPosX;
		board[rookIndex].piece.setPosition(board[rookIndex].x * pieceSize, board[rookIndex].y * pieceSize);
		board[emptyCellIndex].piece.setPosition(board[emptyCellIndex].x * pieceSize, board[emptyCellIndex].y * pieceSize);
	}
	clearLegalMoves(activePieceIndex);
	board[activePieceIndex].firstMove = false;
	sMark.setPosition(10 * pieceSize, 10 * pieceSize);
	turn = turn * -1;

	//CHECKING FOR CHECK--------------------
	vector<int> checkGivingIndexes = getPossibleCheckGivingPieceIndex(board[activePieceIndex].color);
	getCheckAndPinningLines(checkGivingIndexes);
	checkmateInspection = false;
	stalemateInspection = false;
}
//MOVES-----------------------------------------------------------------


//Collapse all functions shortcut: ctrl+m, ctrl+o


//TODO:
//stalemate not working
//the last pawn didnt die for the first time