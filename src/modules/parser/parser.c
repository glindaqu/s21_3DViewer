#include "../../include/parser.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

static void readVertex(ObjFile_t* file, char* line);
static void readNormal(ObjFile_t* file, char* line);
static void readSurface(ObjFile_t* file, char* line);
static void initParser(ObjFile_t* file);
static int checkFileFormat(ObjFile_t* file);

ParserReturnCode parse(ObjFile_t* file) {
  FILE* objFilePtr = fopen(file->fileName, "r");
  if (objFilePtr == NULL) {
    return FILE_DOES_NOT_EXISTS;
  } else if (checkFileFormat(file) == -1) {
    return INVALID_FILE_FORMAT;
  }
  setlocale(LC_NUMERIC, "C");
  char* line = NULL;
  size_t readChars = 0;
  while (getline(&line, &readChars, objFilePtr) != EOF) {
    readVertex(file, line);
    readNormal(file, line);
    readSurface(file, line);
  }
  if (line != NULL) free(line);
  fclose(objFilePtr);
  return file->surfacesCount && file->verticesCount
             ? OK
             : FILE_CONTAINS_UNEXPECTED_CONTENT;
}

void removeObjFile(ObjFile_t* file) {
  for (int i = 0; i < file->verticesCount; i++) {
    free(file->vertices[i]);
  }
  free(file->vertices);
  for (int i = 0; i < file->surfacesCount; i++) {
    free(file->surfaces[i]->verticesIndices);
    free(file->surfaces[i]->normalsIndices);
    free(file->surfaces[i]);
  }
  free(file->surfaces);
  for (int i = 0; i < file->normalsCount; i++) {
    free(file->normals[i]);
  }
  free(file->normals);
}

static int checkFileFormat(ObjFile_t* file) {
  if (strstr(file->fileName, ".obj")) return -1;
  return 1;
}

static void initParser(ObjFile_t* file) {
  file->verticesCount = 0;
  file->normalsCount = 0;
  file->surfacesCount = 0;
  file->vertices = NULL;
  file->normals = NULL;
  file->surfaces = NULL;
}

static void readVertex(ObjFile_t* file, char* line) {
  if (line[0] != 'v' || line[1] != ' ') return;
  file->verticesCount++;
  file->vertices =
      realloc(file->vertices, sizeof(Vertex_t) * file->verticesCount);
  file->vertices[file->verticesCount - 1] = calloc(1, sizeof(Vertex_t));
  Vertex_t* vertex = file->vertices[file->verticesCount - 1];
  sscanf(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
}

static void readNormal(ObjFile_t* file, char* line) {
  if (line[0] != 'v' || line[1] != 'n') return;
  file->normalsCount++;
  file->normals = realloc(file->normals, sizeof(Vertex_t) * file->normalsCount);
  file->normals[file->normalsCount - 1] = calloc(1, sizeof(Vertex_t));
  Vertex_t* vertex = file->normals[file->normalsCount - 1];
  sscanf(line, "vn %f %f %f", &vertex->x, &vertex->y, &vertex->z);
}

static void readSurface(ObjFile_t* file, char* line) {
  if (line[0] != 'f' || line[1] != ' ') return;
  file->surfacesCount++;
  file->surfaces =
      realloc(file->surfaces, sizeof(Surface_t) * file->surfacesCount);
  file->surfaces[file->surfacesCount - 1] = calloc(1, sizeof(Surface_t));
  Surface_t* surface = file->surfaces[file->surfacesCount - 1];
  surface->verticesIndices = calloc(3, sizeof(int));
  surface->normalsIndices = calloc(3, sizeof(int));
  if (strstr(line, "//"))
    sscanf(line, "f %d//%d %d//%d %d//%d", &surface->verticesIndices[0],
           &surface->normalsIndices[0], &surface->verticesIndices[1],
           &surface->normalsIndices[1], &surface->verticesIndices[2],
           &surface->normalsIndices[2]);
  else if (strchr(line, '/'))
    sscanf(line, "f %d/%*d/%d %d/%*d/%d %d/%*d/%d",
           &surface->verticesIndices[0], &surface->normalsIndices[0],
           &surface->verticesIndices[1], &surface->normalsIndices[1],
           &surface->verticesIndices[2], &surface->normalsIndices[2]);
  else
    sscanf(line, "f %d %d %d", &surface->verticesIndices[0],
           &surface->verticesIndices[1], &surface->verticesIndices[2]);
  for (int i = 0; i < 3; i++) {
    surface->verticesIndices[i]--;
    surface->normalsIndices[i]--;
  }
}