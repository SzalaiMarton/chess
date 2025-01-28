#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

class LoadAssets {
public:

    Texture wpawn;
    Texture wrook;
    Texture wknight;
    Texture wbishop;
    Texture wqueen;
    Texture wking;

    Texture bpawn;
    Texture brook;
    Texture bknight;
    Texture bbishop;
    Texture bqueen;
    Texture bking;
    Texture cell;
    Texture legalMoveInd;

    Texture board;
    Texture mark;

    Texture white_win;
    Texture black_win;
    Texture stalemate;

    string location = "textures\\";
    string fileType = ".png";

    int loadTextures() {
        if (!wpawn.loadFromFile(location + "whitepawn" + fileType))
        {
            return 1;
        }
        if (!wrook.loadFromFile(location + "whiteRook" + fileType))
        {
            return 1;
        }
        if (!wbishop.loadFromFile(location + "whiteBishop" + fileType))
        {
            return 1;
        }
        if (!wknight.loadFromFile(location + "whiteKnight" + fileType))
        {
            return 1;
        }
        if (!wqueen.loadFromFile(location + "whiteQueen" + fileType))
        {
            return 1;
        }
        if (!wking.loadFromFile(location + "whiteKing" + fileType))
        {
            return 1;
        }

        if (!bpawn.loadFromFile(location + "blackpawn" + fileType))
        {
            return 1;
        }
        if (!brook.loadFromFile(location + "blackRook" + fileType))
        {
            return 1;
        }
        if (!bknight.loadFromFile(location + "blackKnight" + fileType))
        {
            return 1;
        }
        if (!bbishop.loadFromFile(location + "blackBishop" + fileType))
        {
            return 1;
        }
        if (!bqueen.loadFromFile(location + "blackQueen" + fileType))
        {
            return 1;
        }
        if (!bking.loadFromFile(location + "blackKing" + fileType))
        {
            return 1;
        }

        if (!board.loadFromFile(location + "board" + fileType))
        {
            return 1;
        }
        if (!cell.loadFromFile(location + "cell" + fileType))
        {
            return 1;
        }
        if (!legalMoveInd.loadFromFile(location + "legalMoveIndicator" + fileType))
        {
            return 1;
        }
        if (!mark.loadFromFile(location + "mark" + fileType))
        {
            return 1;
        }
        if (!white_win.loadFromFile(location + "white_win" + fileType))
        {
            return 1;
        }
        if (!black_win.loadFromFile(location + "black_win" + fileType))
        {
            return 1;
        }
        if (!stalemate.loadFromFile(location + "stalemate" + fileType))
        {
            return 1;
        }
        return 0;
    }
};