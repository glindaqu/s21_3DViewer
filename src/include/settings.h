#ifndef _SETTINGS_H

#define _SETTINGS_H

#include <stdio.h>

/**
 * @brief Структура, управляющая конфигурацией настроек работы программы
 *
 * @param projectionType Тип проекции (1, 2)
 * @param edgeType Тип грани (сплошная(1), пунктирная(2))
 * @param edgeColor Цвет грани (массив из 3-х float, представление RGB)
 * @param edgeThickness Толщина ребра
 * @param displayWay Способ отображения (?)
 * @param vertexColor Цвет вершины (аналогично цвету грани)
 * @param vertexSize Размер вершины
 * @param bgColor Цвет фона сцены
 *
 * @author glindaqu
 */
typedef struct {
  unsigned int projectionType;
  unsigned int edgeType;
  float edgeColor[3];
  float edgeThickness;
  unsigned int displayWay;
  float vertexColor[3];
  float vertexSize;
  float bgColor[3];
  char* filePath;
} Settings_t;

/**
 * @brief Коды результатов работы парсера файла настроек
 *
 * @param INVALID_FILE Файл нельзя считать
 * @param OK Успешный парсинг
 * @param PERMISSIONS_DENIDED Нет разрешения на чтение файла
 *
 * @author glindaqu
 */
typedef enum { INVALID_FILE, OK, PERMISSIONS_DENIDED } SettingsParserReturnCode;

/**
 * @brief Загрузка конфигурации в структуру
 *
 * @param settings Структура настроек типа Settings_t
 * @param filePath Путь к файлу конфигурации
 *
 * @author glindaqu
 */
SettingsParserReturnCode loadFile(Settings_t* settings, char* filePath);

/**
 * @brief Запись конфигурации программы в файл
 *
 * @param settings Структура настроек типа Settings_t
 * @param filePath Путь к файлу конфигурации
 *
 * @author glindaqu
 */
SettingsParserReturnCode writeFile(Settings_t* settings, char* filePath);

#endif  //_SETTINGS_H