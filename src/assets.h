#pragma once

#include "settings.h"

namespace Assets
{
    class ObjectTexture
    {
    public:
        std::string name;
        sf::Texture texture;

        ObjectTexture(const std::string& name, sf::Texture& texture);
    };

    std::shared_ptr<Assets::ObjectTexture> getObjectTexture(const std::string& name);
    bool loadImage(const std::string& name, sf::Texture& texture);
    void loadDirectoryElements(const std::string& path);
    std::vector<std::string> getDirectoryContents(const std::string &path);
    
    extern std::vector<std::shared_ptr<Assets::ObjectTexture>> pieceTextures;
    extern std::vector<std::shared_ptr<Assets::ObjectTexture>> otherTextures;
};
