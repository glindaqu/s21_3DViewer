#ifndef _PARSER_H

#define _PARSER_H
#define _GNU_SOURCE

/**
 * @brief Вершина с координатами x, y, z.
 * v - эквивалент в файле формата .obj
 *
 * @param x - координата x
 * @param y - координата y
 * @param z - координата z
 *
 * @author glindaqu
 */
typedef struct {
  float x;
  float y;
  float z;
} Vertex_t;

/**
 * @brief Поверхность, состоящая из вершин (обыно из 3). Хранит индексы этих
 * вершин в массиве ObjFile.verticies f - эквивалент в файле формата .obj
 *
 * @param verticesIndices - массив индексов вершин из массива ObjFile.vertices
 *
 * @author glindaqu
 */
typedef struct {
  int* verticesIndices;
} Surface_t;

/**
 * @brief Структура-представление .obj файла. Содержит все необходимы для
 * рендеринга данные
 *
 * @param fileName - название файла (в том числе нужно для загрузки файла в
 * парсер)
 * @param vertices - массив вершин
 * @param surfaces - массив поверхностей
 * @param verticesCount - кол-во вершин
 * @param surfacesCount - кол-во поверхностей
 *
 * @author glindaqu
 */
typedef struct {
  char* fileName;
  Vertex_t** vertices;
  Surface_t** surfaces;
  int verticesCount;
  int surfacesCount;
} ObjFile_t;

/**
 * @brief Перечисление кодов-состояний, которые может вернуть парсер
 *
 * @param FILE_DOES_NOT_EXISTS - файл не был открыт, ошибка
 * @param OK - ошибок в ходе парсинга не возникло
 *
 * @author glindaqu
 */
typedef enum { FILE_DOES_NOT_EXISTS, OK } ParserReturnCode;

/**
 * @brief Функция для очистки памяти после работы с файлом
 *
 * @param file - указатель на переменную типа ObjFile_t
 *
 * @author glindaqu
 */
void removeObjFile(ObjFile_t* file);

/**
 * @brief Функция парсинга. Принимает на вход структуру типа ObjFile_t (С УЖЕ
 * УКАЗАННЫМ ИМЕНЕМ ФАЙЛА В НЕЙ) и заполняет структуру в соответствие с данными
 * в файле
 *
 * @param file - указатель на структуру файла типа ObjFile_t
 *
 * @author glindaqu
 */
ParserReturnCode parse(ObjFile_t* file);

#endif  //_PARSER_H