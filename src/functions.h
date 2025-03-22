#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "objects.h"
#include "SFML/Graphics.hpp"

class Functions
{
public:
    static void refresFrame(sf::RenderWindow&, Objects::Board&);
};

#endif