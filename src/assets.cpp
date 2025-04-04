#include "assets.h"

static std::vector<Assets::ObjectTexture> pieceTextures;
static std::vector<Assets::ObjectTexture> otherTextures;


Assets::ObjectTexture Assets::getObjectTexture(const std::string& name)
{
    for(auto &texture : Assets::pieceTextures)
    {
        if(texture.name == name)
        {
            return texture;
        }
    }
    for (auto& texture : Assets::otherTextures)
    {
        if (texture.name == name)
        {
            return texture;
        }
    }
    return *new Assets::ObjectTexture();
}

bool Assets::loadImage(const std::string& path, const std::string& name, sf::Texture& texture)
{
    return texture.loadFromFile(name + fileType);
}

void Assets::loadDirectoryElements(const std::string& path)
{
    std::vector<std::string> contents = Assets::getDirectoryContents(path);
    for (const auto& text : contents)
    {
        sf::Texture temp;
        if(!Assets::loadImage(path, text.substr(0, text.size()-4), temp))
        {
            std::cerr << "Couldn't load " + text.substr(0, text.size()-4) << std::endl;
            continue;
        }
        if (path.find("other") != 0)
        {
            Assets::otherTextures.emplace_back(new Assets::ObjectTexture(text.substr(path.length() + 1, text.find(".") - path.length() - 1), temp));
        }
        else
        {
            Assets::pieceTextures.emplace_back(new Assets::ObjectTexture(text.substr(path.length() + 1, text.find(".") - path.length() - 1), temp));
        }
    }
}

std::vector<std::string> Assets::getDirectoryContents(const std::string& path) {
    std::vector<std::string> contents;
    try 
    {
        for (const auto& entry : std::filesystem::directory_iterator(path)) 
        {
            contents.emplace_back(entry.path().string());
        }
    }
    catch (const std::filesystem::filesystem_error& e) 
    {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    return contents;
}

Assets::ObjectTexture::ObjectTexture()
{
    this->name = noName;
}

Assets::ObjectTexture::ObjectTexture(const std::string& name, sf::Texture& texture)
{
    this->name = name;
    this->texture = texture;
}