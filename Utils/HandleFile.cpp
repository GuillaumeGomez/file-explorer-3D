#include "HandleFile.hpp"
#include "MyException.hpp"

using namespace std;

HandleFile::HandleFile(string name, ios_base::openmode mode) : m_openMode(mode), m_filename(name)
{
}

HandleFile::HandleFile(const char *name, ios_base::openmode mode) : m_openMode(mode)
{
    if (!name){
        throw MyException("Invalid file name");
    }
    m_filename = name;
}

HandleFile::~HandleFile()
{
    close();
}

std::string const   &HandleFile::getFilename() const
{
    return m_filename;
}

std::string const   &HandleFile::getLastError() const
{
    return m_lastError;
}

std::size_t         HandleFile::read(std::size_t bytesToRead, std::string &s)
{
    if (!this->isOpen()){
        m_lastError = "File isn't open";
        return 0;
    }
    char *tmpBuff = new char[bytesToRead];

    m_stream.read(tmpBuff, bytesToRead);
    string tmp;
    size_t r = m_stream.gcount();

    tmp.copy(tmpBuff, r);
    s += tmp;
    delete[] tmpBuff;
    return r;
}

char                *HandleFile::read(std::size_t bytesToRead, std::size_t *readBytes)
{
    if (!this->isOpen()){
        m_lastError = "File isn't open";
        return 0;
    }
    char *tmpBuff = new char[bytesToRead];

    m_stream.read(tmpBuff, bytesToRead);
    size_t r = m_stream.gcount();

    if (readBytes)
        *readBytes = r;
    return tmpBuff;
}

void    HandleFile::write(const string &s)
{
    this->write(s.length(), s);
}

void         HandleFile::write(std::size_t bytesToWrite, std::string const &s)
{
    if (!this->isOpen()){
        m_lastError = "File isn't open";
        return;
    }
    m_stream.write(s.c_str(), bytesToWrite);
}

void HandleFile::write(std::size_t bytesToWrite, const char *s)
{
    if (!this->isOpen()){
        m_lastError = "File isn't open";
        return;
    }
    m_stream.write(s, bytesToWrite);
}

bool                HandleFile::isOpen()
{
    return m_stream.is_open();
}

bool                HandleFile::open()
{
    if (this->isOpen())
        return true;
    m_stream.open(m_filename.c_str(), m_openMode);
    if (this->isOpen()){
        m_lastError = "Couldn't open: " + m_filename;
        return false;
    }
    return true;
}

void                HandleFile::close()
{
    if (!this->isOpen())
        return;
    m_stream.close();
}
