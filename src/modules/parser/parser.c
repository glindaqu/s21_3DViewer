#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

static void readVertex(ObjFile_t* file, char* line);

ParserReturnCode parse(ObjFile_t* file) {
    FILE* objFilePtr = fopen(file->fileName, "r");
    if (objFilePtr == NULL) {
        return FILE_DOES_NOT_EXISTS;
    }
    char* line = NULL;
    size_t readChars = 0;
    while (getline(&line, &readChars, objFilePtr) != EOF) {
        printf("%s", line);
        readVertex(file, line);
    }
    if (line != NULL) free(line);
    fclose(objFilePtr);
    return OK;
}

static void readVertex(ObjFile_t* file, char* line) {
    if (line[0] != 'v' || line[1] != ' ') return;
    file->verticesCount++;
    file->vertices = realloc(file->vertices, sizeof(Vertex_t) * file->verticesCount);
    file->vertices[file->verticesCount - 1] = calloc(1, sizeof(Vertex_t));
    Vertex_t* vertex = file->vertices[file->verticesCount - 1];
    sscanf(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
}

void printVertices(ObjFile_t file) {
    for (int i = 0; i < file.verticesCount; i++) {
        printf("%g %g %g\n", file.vertices[i]->x, file.vertices[i]->y, file.vertices[i]->z);
    }
}

void removeObjFile(ObjFile_t* file) {
    for (int i = 0; i < file->verticesCount; i++) {
        free(file->vertices[i]);
    }
    free(file->vertices);
}

int main(void) {
    ObjFile_t file = {0};
    file.fileName = "cube.obj";
    parse(&file);
    printVertices(file);
    removeObjFile(&file);
    return 0;
}