#ifndef _PARSER_H

#define _PARSER_H

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
 * @brief Поверхность, состоящая из вершин (обыно из 3)
 * f - эквивалент в файле формата .obj
 */
typedef struct {
    Vertex_t* vertices;
} Surface_t;

typedef struct {
    char* fileName;
    Vertex_t** vertices;
    Surface_t* surfaces;
    int verticesCount;
} ObjFile_t;

typedef enum {
    FILE_DOES_NOT_EXISTS,
    OK
} ParserReturnCode;


#endif //_PARSER_H