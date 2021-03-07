
#pragma once
#ifndef FRACTTOOLS_H
#define FRACTTOOLS_H

#include <string>
#include <vector>

std::string moduleDirPath();                         //!< Возвращает путь к исполняемому модулю (в конце - '/')
bool isPathRelative(const std::string &path);        //!< Возвращает @c true в случае относительно пути
std::string absolutePath(const std::string &path);   //!< Возвращает абсолютный путь для относительного
std::string canonizePath(const std::string &path);   //!< Возвращает путь в канонизированном виде (прямой слеш) 
// std::string fileName(const std::string &path);       //!< Возвращает имя файла (включая расширение)
// std::string fileBaseName(const std::string &path);   //!< Возвращает базовое имя файла (без расширения)
std::string fileExtension(const std::string &path);  //!< Возвращает расширение файла
bool isFileExist(const std::string &path);           //!< Возвращает true, если файл существует
bool isDirExist(const std::string &path);            //!< Возвращает true, если директория существует
// std::string getFileDir(const std::string &path);     //!< Возвращает путь к директории по имени файла.
bool ensureFolder(const std::string &path);          //!< Обеспечивает существование папки по заданному пути.
std::string join(const std::string &path1,           //!< Возвращает путь, соединенный из двух
const std::string &path2);
std::string join(const std::vector<std::string> &path_parts); //!< Возвращает путь, соединенный из частей пути, лежащих в векторе.
std::vector<std::string> split(const std::string &path); //!< Возвращает вектор со всеми папками пути
std::pair<std::string, std::string> splitLast(const std::string &path); //!< отделаяет последний элемент пути, возвращает пару внешний путь и последний элемент
std::vector<std::string> ls(const std::string &path = "."); //!< list dir
//! @brief a proxy to stlplus::folder_wildcard; lists folder contents that satisfy wildcard
std::vector<std::string> ls_wc(const std::string &folder, const std::string &wildcard, bool subfolders = true, bool files = true);
//! true, если удалось удалить указаный файл или папку
bool remove(const std::string &path);
//! true, если удалось переименовать указаный файл или папку
bool rename(const std::string &pathFrom, const std::string &pathTo);
//! true, если удалось скопировать указаный файл (для папок не реализовано)
bool copy(const std::string &pathFrom, const std::string &pathTo);                
//! @brief отделяет extention от остального имени файла
std::pair<std::string, std::string> splitExt(const std::string &file);
std::string currentDateTime();

#endif  // FRACTTOOLS_H
