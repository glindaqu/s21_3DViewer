#ifndef _PARSER_H

#define _PARSER_H
#define _GNU_SOURCE

/**
 * @brief Вершина с координатами x, y, z.
 * v эквивалент в файле формата .obj
 *
 * @param x координата x
 * @param y координата y
 * @param z координата z
 *
 * @author glindaqu
 */
typedef struct {
  float x, y, z;
} Vertex_t;

/**
 * @brief Поверхность, состоящая из вершин (обыно из 3). Хранит индексы этих
 * вершин в массиве ObjFile.verticies f эквивалент в файле формата .obj
 *
 * @param verticesIndices массив индексов вершин из массива ObjFile.vertices
 * @param normalsIndices массив индексов нормалей из массива ObjFile.normals
 *
 * @author glindaqu
 */
typedef struct {
  unsigned int* verticesIndices;
  unsigned int* normalsIndices;
} Surface_t;

/**
 * @brief Структура представления .obj файла. Содержит все необходимы для
 * рендеринга данные
 *
 * @param fileName название файла (в том числе нужно для загрузки файла в
 * парсер)
 * @param vertices массив вершин
 * @param normals массив нормалей
 * @param surfaces массив поверхностей
 * @param verticesCount колво вершин
 * @param surfacesCount колво поверхностей
 * @param normalsCount колво нормалей
 *
 * @author glindaqu
 */
typedef struct {
  const char* fileName;
  Vertex_t** vertices;
  Vertex_t** normals;
  Surface_t** surfaces;
  int verticesCount;
  int surfacesCount;
  int normalsCount;
} ObjFile_t;

/**
 * @brief Инициализация структуры ObjFile_t
 *
 * @param file Указатель на переменную типа ObjFile_t
 *
 * @return void
 *
 * @author yukikoqo
 */
void initParser(ObjFile_t* file);

/**
 * @brief Перечисление кодовсостояний, которые может вернуть парсер
 *
 * @param FILE_DOES_NOT_EXISTS Файл не был открыт, ошибка
 * @param OK Ошибок в ходе парсинга не возникло
 * @param INVALID_FILE_FORMAT Расширение файла отлично от .obj
 * @param FILE_CONTAINS_UNEXPECTED_CONTENT Парсинг файла не дал результата
 * (структура пуста)
 *
 * @author glindaqu
 */
typedef enum {
  FILE_DOES_NOT_EXISTS,
  OK,
  INVALID_FILE_FORMAT,
  FILE_CONTAINS_UNEXPECTED_CONTENT
} ParserReturnCode;

/**
 * @brief Функция для очистки памяти после работы с файлом
 *
 * @param file Указатель на переменную типа ObjFile_t
 *
 * @return void
 *
 * @author glindaqu
 */
void removeObjFile(ObjFile_t* file);

/**
 * @brief Функция парсинга. Принимает на вход структуру типа ObjFile_t (С УЖЕ
 * УКАЗАННЫМ ИМЕНЕМ ФАЙЛА В НЕЙ) и заполняет структуру в соответствие с данными
 * в файле
 *
 * @param file Указатель на структуру файла типа ObjFile_t
 *
 * @return ParserReturnCode
 *
 * @author glindaqu
 */
ParserReturnCode parse(ObjFile_t* file);

#endif  //_PARSER_H
