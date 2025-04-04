#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

const int windowWidth = 800;
const int windowHeight = windowWidth * (16 / 9);
const std::string windowTitle = "chess";
const float boardScale = 1.5f;
const int cellWidth = (windowWidth - 36) / 8;
const int cellHeight = (windowHeight - 36) / 8;
const int fps = 60;
const int noIndex = -1;

std::vector<std::string> whitePieceOrder = { "pawn", "rook", "knight", "bishop", "queen", "king", "bishop", "kinght", "rook" };
std::vector<std::string> blackPieceOrder = { "rook", "knight", "bishop", "king", "queen", "bishop", "kinght", "rook", "pawn" };
const std::vector<char> colorOrder = { "b", "w" };

const std::string noName = "null";
const std::string pathToPieceTextures = "res\\textures\\piece_textures";
const std::string pathToOtherTextures = "res\\textures\\other_textures";
const std::string fileType = ".png";

#endif