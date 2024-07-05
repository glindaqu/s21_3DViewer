#include "../../include/settings.h"

SettingsParserReturnCode writeFile(Settings_t* settings, char* filePath) {
  FILE* fp = fopen(filePath, "w+");

  if (fp == NULL || fwrite(settings, sizeof(Settings_t), 1, fp) == 0)
    return INVALID_FILE;

  return OK;
}