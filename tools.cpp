#include <sys/types.h>
#include <sys/stat.h>

#include <cstring>
#include <ctime>
#include <iostream>

#include "tools.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#ifdef _MSC_VER
#  ifndef NOMINMAX
#     define NOMINMAX
#  endif
#  include <windows.h>
#  include <direct.h>
#  include <limits.h>
#  include <shlobj.h>
#  ifndef S_ISDIR
#      define S_ISDIR(mode)  ((mode & _S_IFMT) == _S_IFDIR)
#  endif
#  ifndef S_ISREG
#     define S_ISREG(mode)  ((mode & _S_IFMT) == _S_IFREG)
#  endif
#elif __linux__
#include <cstdlib>
#include <unistd.h>
#endif

cv::Point FRACTAL::putTexts(
      cv::Mat& out,
      const std::vector<std::string> texts,
      const cv::Point& org,
      const int verticalStep,
      const int horizontalStep,
      const cv::Scalar& clr,
      const double scale
      )
  {
    auto n_org = org;
    for(const auto& text: texts)
    {
      if(n_org.x > out.size().width ||
         n_org.y > out.size().height)
        break;
      cv::putText(
            out,
            text,
            n_org,
            cv::FONT_HERSHEY_PLAIN,
            scale,
            clr,
            2
            );
      n_org += cv::Point(horizontalStep, verticalStep);
    }
    return n_org;
  }

template <typename T>
void FRACTAL::addIfNotExist(std::vector<T>& vec, const T& el)
{
  if(std::find(vec.begin(), vec.end(), el) == vec.end())
    vec.push_back(el);
}

std::string FRACTAL::currentDateTime() 
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%H.%M.%S..%Y.%m.%d", &tstruct);
	return buf;
}

std::string FRACTAL::moduleDirPath()
{
  char libraryPathBuf[256] = {0};

#ifdef _MSC_VER
  HMODULE hModule = 0;
  BOOL ret = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&__FUNCTION__, &hModule);
  DWORD nLen = GetModuleFileNameA(hModule, libraryPathBuf, sizeof(libraryPathBuf));
  if (nLen == sizeof(libraryPathBuf))
    _getcwd(libraryPathBuf, sizeof(libraryPathBuf));

  char drive[256] = {0}, directory[1024] = {0};
  _splitpath(libraryPathBuf, drive, directory, 0, 0);
  _makepath(libraryPathBuf, drive, directory, 0, 0);
#elif __linux__
  if (getcwd(libraryPathBuf, sizeof(libraryPathBuf)) == NULL)
      return std::string();
#endif

  std::string libraryPath(libraryPathBuf);
  const char lastChar = libraryPath[libraryPath.length() - 1];
  if (lastChar != '\\' && lastChar != '/')
    libraryPath.append("/");

  return canonizePath(libraryPath);
}

bool FRACTAL::isPathRelative(const std::string &path)
{
#ifdef _MSC_VER
  bool withoutDriveLetter = path.empty() || ( path.size() > 0 && path[0] != '/' && path[0] != '\\');
  char driveBuf[256] = {0};
  _splitpath(path.c_str(), driveBuf, 0, 0, 0);
  return strlen(driveBuf) <= 0 && withoutDriveLetter;
#else
  return path.empty() || ( path.size() > 0 && path[0] != '/' );
#endif
}

std::string FRACTAL::absolutePath(const std::string &path)
{
  return canonizePath(isPathRelative(path) ? join(moduleDirPath(), path) : path);
}

std::string FRACTAL::canonizePath(const std::string &path)
{
  std::string canonizedPath = path;
  for (size_t i = 0; i < canonizedPath.length(); i++)
    if (canonizedPath[i] == '\\')
      canonizedPath[i] = '/';

  return canonizedPath;
}

// std::string fileName(const std::string &path)
// {
//   size_t pos = canonizePath(path).rfind("/");
//   return (pos == std::string::npos) ? path : path.substr(pos + 1);
// }

// std::string fileBaseName(const std::string &path)
// {
//   size_t pos = canonizePath(path).rfind(".");
//   return fileName(pos == std::string::npos ? path : path.substr(0, pos));
// }

std::string FRACTAL::fileExtension(const std::string &path)
{
  size_t pos = canonizePath(path).rfind(".");
  return (pos == std::string::npos) ? std::string() : path.substr(pos + 1);
}

bool FRACTAL::isFileExist(const std::string &path)
{
  int res(-1);
#ifdef _WINDOWS
  struct _stat64 info = {0};
  std::vector<WCHAR> pathW(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, 0, 0));
  if (0 < MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, &pathW[0],
    static_cast<int>(pathW.size())))
  {
    res = _wstat64(pathW.data(), &info);
  }
#else
  struct stat info = {0};
  res = stat(path.c_str(), &info);
#endif
  return (0 == res) && S_ISREG(info.st_mode);
}

bool FRACTAL::isDirExist(const std::string &path)
{
  int res(-1);
#ifdef _WINDOWS
  struct _stat info = {0};
  std::vector<WCHAR> pathW(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, 0, 0));
  if (0 < MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, &pathW[0],
    static_cast<int>(pathW.size())))
  {
    if (2 < static_cast<int>(pathW.size()))
      if (pathW[static_cast<int>(pathW.size()) - 2] == '/' ||
          pathW[static_cast<int>(pathW.size()) - 2] == '\\') {
      pathW.erase(pathW.end() - 2);
    }
    res = _wstat(pathW.data(), &info);
  }
#else
  struct stat info = {0};
  res = stat(path.c_str(), &info);
#endif
  return (0 == res) && S_ISDIR(info.st_mode);
}

// std::string getFileDir(const std::string &path)
// {
//   std::string cpath = canonizePath(path);
//   auto p = path;
//   std::string fileName = fileName(p);
//   std::string fileDir = cpath.substr(0, cpath.size() - fileName.size());
//   while (fileDir.size() > 1 && fileDir[fileDir.size() - 1] == '/')
//     fileDir.erase(fileDir.size() - 1, 1);
//   return fileDir;
// }


bool FRACTAL::mkdir(const std::string &path)
{
  if (0 < path.size())
  {
#ifdef _WINDOWS
    std::vector<WCHAR> pathW(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, 0, 0));
    if (0 < MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, &pathW[0],
      static_cast<int>(pathW.size())))
    {
      std::vector<WCHAR> pathFull(GetFullPathNameW(&pathW[0], 0, 0, 0));
      if (0 < GetFullPathNameW(&pathW[0], static_cast<DWORD>(pathFull.size()), &pathFull[0], 0))
      {
        if (ERROR_SUCCESS == SHCreateDirectoryExW(0, &pathFull[0], 0) || 
            ERROR_ALREADY_EXISTS == GetLastError())
        return true;
      }
    }
#else
    struct stat st = {0};
    if (!isDirExist(path))
      int res = system(("mkdir -p " + path).c_str());

    if (stat(path.c_str(), &st) == 0)
      return true;
#endif
  }
  else
  {
    std::cerr << "mkdir: empty path " << std::endl;
  }

  return false;
}

std::string FRACTAL::join(const std::string &path1, const std::string &path2)
{
  if (!isPathRelative(path2))
    return canonizePath(path2);
  std::string canonizedPath1 = canonizePath(path1);
  if (canonizedPath1.length() > 0 && canonizedPath1[canonizedPath1.length() - 1] != '/')
    canonizedPath1 += "/";
  return canonizedPath1 + canonizePath(path2);
}

std::string FRACTAL::join(const std::vector<std::string> &path_parts)
{
  if (path_parts.size() == 0)
    return std::string();
  if (path_parts.size() == 1)
    return canonizePath(path_parts[0]);
  std::string result = canonizePath(path_parts[0]);
  if (result.empty()) // first path part was empty. means filesystem root
    result += "/";
  for (int i = 1; i < static_cast<int> (path_parts.size()); ++i)
    result = join(result, path_parts[i]);
  return result;
}

std::vector<std::string> FRACTAL::split(const std::string &path)
{
  std::string canonizedPath = canonizePath(path);
  std::vector<size_t> forwardSlashIndices;
  std::vector<std::string> result;
  //forwardSlashIndices.push_back(0);
  for (size_t i = 0; i < canonizedPath.length(); ++i)
  {
    if (canonizedPath[i] == '/')
      forwardSlashIndices.push_back(i);
  }
  
  if (forwardSlashIndices.empty())
  {
    result.push_back(path);
    return result;
  }
  
  if (forwardSlashIndices.back() != canonizedPath.length() - 1)
    forwardSlashIndices.push_back(canonizedPath.length());
  if (isPathRelative(canonizedPath))
  {
    result.push_back(canonizedPath.substr(
      0, 
      forwardSlashIndices[0]));
  }
  else
  {
    forwardSlashIndices[0] = -1;
  }

  for (int i = 0; i < static_cast<int> (forwardSlashIndices.size()) - 1; ++i)
  {
    result.push_back(canonizedPath.substr(
      forwardSlashIndices[i] + 1, 
      forwardSlashIndices[i + 1] - forwardSlashIndices[i] - 1));
  }
  return result;
}

std::pair<std::string, std::string> FRACTAL::splitLast(const std::string &path)
{
  auto parts = split(path);
  if (parts.size() == 0)
    return std::make_pair(std::string(), std::string());

  auto last = parts.back();
  parts.pop_back();
  return std::make_pair(join(parts), last);
}



std::vector<std::string> FRACTAL::ls(const std::string &path)
{
  // return stlplus::folder_all(path);
}

std::vector<std::string> FRACTAL::ls_wc(const std::string &folder, const std::string &wildcard, bool subfolders,
  bool files)
{
  // return stlplus::folder_wildcard(folder, wildcard, subfolders, files);
}



bool FRACTAL::remove(const std::string &path)
{
  bool res(false);
  // if (0 < path.size())
  // {
  //   if (isDirExist(path))
  //   {
  //     res = stlplus::folder_delete(path, true);
  //   }
  //   else if (isFileExist(path))
  //   {
  //     res = stlplus::file_delete(path);
  //   }
  //   else
  //   {
  //     std::cerr << "remove can't detect path type for " << path << std::endl;
  //     res = false;
  //   }
  // }
  // else
  // {
  //   std::cerr << "remove empty path" << std::endl;
  // }
  return res;
}

bool FRACTAL::rename(const std::string &pathFrom, const std::string &pathTo)
{
  bool res(false);
  // if ((0 < pathFrom.size()) && (0 < pathTo.size()))
  // // not empty path
  // {
  //   if (isDirExist(pathFrom))
  //   {
  //     res = stlplus::folder_rename(pathFrom, pathTo);
  //   }
  //   else if (isFileExist(pathFrom))
  //   {
  //     res = stlplus::file_rename(pathFrom, pathTo);
  //   }
  //   else
  //   {
  //     std::cerr << "rename can't detect path type for " << pathFrom << std::endl;
  //     res = false;
  //   }
  // }
  // else
  // {
  //   std::cerr << "rename empty path" << std::endl;
  // }
  return res;
}
  
bool FRACTAL::copy(const std::string &pathFrom, const std::string &pathTo)                
{
  bool res(false);
  // if ((0 < pathFrom.size()) && (0 < pathTo.size()))
  // // not empty path
  // {
  //   if (isDirExist(pathFrom))
  //   {
  //     std::cerr << "copy not implimented for folder " << pathFrom << std::endl;
  //     res = false;
  //   }
  //   else if (isFileExist(pathFrom))
  //   {
  //     res = stlplus::file_copy(pathFrom, pathTo);
  //   }
  //   else
  //   {
  //     std::cerr << "copy can't detect path type for " << pathFrom << std::endl;
  //     res = false;
  //   }
  // }
  // else
  // {
  //   std::cerr << "copy empty path" << std::endl;
  // }
  return res;
}

std::pair<std::string, std::string> FRACTAL::splitExt(const std::string &file)
{
  auto pos = file.find_last_of('.');
  if (pos != std::string::npos)
  {
    return std::make_pair(file.substr(0, pos), file.substr(pos));
  }
  else
  {
    return std::make_pair(file, std::string());
  }
}
