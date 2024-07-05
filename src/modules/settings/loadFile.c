#include "../../include/settings.h"

SettingsParserReturnCode loadFile(Settings_t* settings, char* filePath) {
  FILE* fp = fopen(filePath, "r+");

  if (fp == NULL || fread(settings, sizeof(Settings_t), 1, fp) == 0)
    return INVALID_FILE;

  return OK;
}