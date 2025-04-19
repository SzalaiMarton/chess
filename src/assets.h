#ifndef ASSETS_H
#define ASSETS_H

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "settings.h"


class Assets
{
public:
    
    class ObjectTexture
    {
    public:
        std::string name;
        sf::Texture texture;

        ObjectTexture(const std::string& name, sf::Texture& texture);
    };

    static ObjectTexture* getObjectTexture(const std::string& name);
    static bool loadImage(const std::string& path, const std::string& name, sf::Texture& texture);
    static void loadDirectoryElements(const std::string& path);
    static std::vector<std::string> getDirectoryContents(const std::string &path);
    

    static std::vector<ObjectTexture*> pieceTextures;
    static std::vector<ObjectTexture*> otherTextures;


};

#endif