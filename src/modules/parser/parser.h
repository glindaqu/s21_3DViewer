#ifndef _PARSER_H

#define _PARSER_H
#define _GNU_SOURCE

/**
 * @brief Вершина с координатами x, y, z. 
 * v - эквивалент в файле формата .obj
 */
typedef struct {
    float x;
    float y;
    float z;
} Vertex_t;

/**
 * @brief Поверхность, состоящая из вершин (обыно из 3). Хранит индексы этих вершин в массиве ObjFile.verticies
 * f - эквивалент в файле формата .obj
 */
typedef struct {
    int* verticesIndices;
} Surface_t;

typedef struct {
    char* fileName;
    Vertex_t** vertices;
    Surface_t** surfaces;
    int verticesCount;
    int surfacesCount;
} ObjFile_t;

typedef enum {
    FILE_DOES_NOT_EXISTS,
    OK
} ParserReturnCode;

void removeObjFile(ObjFile_t* file);
ParserReturnCode parse(ObjFile_t* file);


#endif //_PARSER_H