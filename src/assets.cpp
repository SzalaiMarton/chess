#include "assets.h"

std::vector<Assets::ObjectTexture> Assets::textureVector;

Assets::ObjectTexture Assets::getObjectTexture(std::string name)
{
    sf::Texture nothing;
    for(auto texture : Assets::textureVector)
    {
        if(texture.name == name)
        {
            return texture;
        }
    }
    return *new Assets::ObjectTexture("null", nothing);
}

bool Assets::loadImage(std::string path, std::string name, sf::Texture& texture)
{
    Assets assets;
    return texture.loadFromFile(path + name + assets.fileType);
}

void Assets::loadDirectoryElements(std::string path)
{
    std::vector<std::string> contents = Assets::getDirectoryContents(path);
    for (const auto& text : contents)
    {
        sf::Texture temp;
        if(Assets::loadImage(path, text.substr(0, text.size()-4), temp))
        {
            std::cerr << "Couldn't load " + text.substr(0, text.size()-4) << std::endl;
            continue;
        }
        Assets::textureVector.push_back(*new Assets::ObjectTexture(text.substr(0, text.size() - 5), temp));
    }
}

std::vector<std::string> Assets::getDirectoryContents(const std::string& path) {
    std::vector<std::string> contents;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            contents.push_back(entry.path().string());
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    return contents;
}

Assets::ObjectTexture::ObjectTexture(std::string name, sf::Texture texture)
{
    this->name = name;
    this->texture = texture;
}