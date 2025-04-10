#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

const int windowWidth = 800;
const int windowHeight = windowWidth * (16 / 9);
const std::string windowTitle = "chess";
const float boardScale = 1.5f;
const int cellWidth = (windowWidth - 36) / 8;
const int cellHeight = (windowHeight - 36) / 8;
const int fps = 60;
const int noIndex = -1;

extern std::vector<std::string> whitePieceOrder;
extern std::vector<std::string> blackPieceOrder;
extern std::vector<char> colorOrder;

const std::string noName = "null";
const std::string pathToPieceTextures = "res\\textures\\piece_textures";
const std::string pathToOtherTextures = "res\\textures\\other_textures";
const std::string fileType = ".png";

#endif