#include "functions.h"


void Functions::refresFrame(sf::RenderWindow& window, Objects::Board& board)
{
    window.clear();
    window.draw(board.sprite);
    for (auto obj : Objects::allPieces)
    {
        window.draw(obj.sprite);
    }
    window.display();
}