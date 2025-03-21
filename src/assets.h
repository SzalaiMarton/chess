#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include "SFML/Graphics.hpp"

class Assets
{
public:
    const std::string pathToPieceTextures = "res/texture/piece_textures";
    const std::string pathToOtherTextures = "res/texture/other_textures";
    const std::string fileType = "png";

    class ObjectTexture
    {
    public:
        std::string name;
        sf::Texture texture;

        ObjectTexture(std::string name, sf::Texture texture);
    };

    static ObjectTexture getObjectTexture(std::string);
    static bool loadImage(std::string, std::string, sf::Texture&);
    static void loadDirectoryElements(std::string);
    static std::vector<std::string> getDirectoryContents(const std::string &path);

    static std::vector<ObjectTexture> textureVector;
};