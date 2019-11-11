#pragma once
#include "../../dokan/dokan.h"
#include "../../dokan/fileinfo.h"
#include <string>
#include <winbase.h>
#include <mutex>
#include <sstream>
#include <filesystem>

struct FileNode {
  FileNode(std::wstring fileName, bool isDirectory)
      : FileNode(fileName, isDirectory,
                 isDirectory ? FILE_ATTRIBUTE_DIRECTORY
                             : FILE_ATTRIBUTE_ARCHIVE) {}

  FileNode(std::wstring fileName, bool isDirectory, DWORD fileAttr)
      : FileName(fileName), IsDirectory(isDirectory), Attributes(fileAttr) {
    Times.Reset();
    FilePath = std::filesystem::path(FileName);
  }

  FileNode(const FileNode &f) {
    FileName = f.FileName;
    FilePath = f.FilePath;
    IsDirectory = f.IsDirectory;
    Attributes = f.Attributes;
    Times = f.Times;
    _data << f._data.rdbuf();
  }

  struct FileTimes {
    void Reset() {
      GetSystemTimeAsFileTime(&Creation);
      LastAccess = LastWrite = Creation;
    }

    FILETIME Creation;
    FILETIME LastAccess;
    FILETIME LastWrite;
  };

  std::wstring FileName;
  std::filesystem::path FilePath;
  bool IsDirectory = false;
  DWORD Attributes = 0;
  FileTimes Times;

  size_t getFileSize() { return _data.str().length(); }

private:
  FileNode() = default;

  std::mutex _file_mutex;
  std::stringstream _data;
};