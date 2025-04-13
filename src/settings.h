#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

const float boardTextureWidth = 532.f; // board height is the same
const float boardSide = 30.f; // the side that needs to be subtracted from board to get the playing field

const float pieceTextureWidth = 64.f; // piece height is the same

const int windowWidth = 800;
const int windowHeight = windowWidth * (16 / 9);
const std::string windowTitle = "chess";
const float boardScale = windowWidth / boardTextureWidth;
const float cellWidth = (windowWidth - 36) / 8;
const float cellHeight = (windowHeight - 36) / 8;
const float pieceScale = cellWidth / pieceTextureWidth;
const int fps = 60;
const int noIndex = -1;

extern std::vector<std::string> whitePieceOrder;
extern std::vector<std::string> blackPieceOrder;
extern std::vector<char> colorOrder;

const std::string pathToPieceTextures = "res\\textures\\piece_textures";
const std::string pathToOtherTextures = "res\\textures\\other_textures";
const std::string fileType = ".png";

#endif