#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include <vector>
#include <string>

class Directory
{
public:
    Directory(const char*);
    Directory(std::string);
    ~Directory();
    const char                      *getPath() const;
    size_t                          objectsNumber() const;
    std::vector<std::string> const  &getFileList() const;
    std::vector<std::string> const  &getDirectoryList() const;
    static std::string              getCurrentDirName();

private:
    std::string                 m_path;
    std::vector<std::string>    m_directorys;
    std::vector<std::string>    m_files;
};

#endif // DIRECTORY_HPP
