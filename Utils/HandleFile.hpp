#ifndef HANDLEFILE_HPP
#define HANDLEFILE_HPP

#include <string>
#include <fstream>

class HandleFile
{
public:
    HandleFile(std::string name, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary);
    HandleFile(const char *name, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary);
    virtual ~HandleFile();
    std::string const   &getFilename() const;
    std::size_t         read(std::size_t bytesToRead, std::string&);
    char                *read(std::size_t bytesToRead, std::size_t *readBytes = 0);
    void                write(std::size_t bytesToWrite, std::string const&);
    void                write(std::size_t bytesToWrite, const char *);
    void                write(std::string const&);
    bool                isOpen();
    bool                open();
    void                close();
    void                flush();
    std::string const   &getLastError() const;
    static bool         exists(const char*);

protected:
    std::ios_base::openmode     m_openMode;
    std::string                 m_filename;
    std::string                 m_lastError;
    std::fstream                m_stream;
};

#endif // HANDLEFILE_HPP
