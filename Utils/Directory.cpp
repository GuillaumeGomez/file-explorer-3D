#include "Directory.hpp"
#include "MyException.hpp"
#include "Utils.hpp"

Directory::Directory(const char *s)
{
    if (!s)
        throw MyException("invalid null path");
    m_path = s;

    std::string lastError;
    std::vector<std::pair<std::string, int> > v = Utils::getFolderDatas(s, &lastError);

    if (v.size() < 1 && lastError != ""){
        throw MyException(lastError);
      }
    for (std::vector<std::pair<std::string, int> >::iterator it = v.begin(); it != v.end(); ++it)
    {
        if ((*it).second == Utils::DIRECTORY) {
            if ((*it).first != "." && (*it).first != "..")
                m_directorys.push_back((*it).first);
        }
        else
            m_files.push_back((*it).first);
    }
}

Directory::~Directory()
{
}

const char                      *Directory::getPath() const
{
    return m_path.c_str();
}

size_t                    Directory::objectsNumber() const
{
    return m_directorys.size() + m_files.size();
}

std::vector<std::string> const  &Directory::getFileList() const
{
    return m_files;
}


std::vector<std::string> const  &Directory::getDirectoryList() const
{
    return m_directorys;
}
