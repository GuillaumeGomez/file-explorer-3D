#pragma once

#include <vector>
#include <string>
#include <utility>

#ifdef WIN32
#include <windows.h>
#endif

namespace Utils
{
  enum {
    DIRECTORY,
    FILE
  };

#ifdef WIN32
  std::wstring string2wstring(const std::string &s);
  std::wstring string2wstring(const char *s);
  std::string wstring2string(const std::wstring &wstr);
  std::string wstring2string(const wchar_t *s);
#endif

  std::vector<std::pair<std::string, int> > getFolderDatas(const char*, std::string *lastError = 0);
  std::vector<std::pair<std::string, int> > getFolderDatas(std::string const&, std::string *lastError = 0);
  void  sleep(unsigned int ms);
  bool  playSound(const char *sound_name);

#ifdef WIN32
  HBITMAP getIconFromFileType(const char *s, int *width, int *height, int *bitsPerPixel);
  void  destroyIcon(void *i);
  bool  decode(unsigned char* buffer, unsigned int &width, unsigned int &height,
               std::vector<unsigned char> &image);
  /*bool  saveBMP(const char *name, std::vector<unsigned char> const &image, unsigned int width, unsigned int height,
                unsigned int depth);*/
  bool  saveBMP(const char *filename, HBITMAP img);
  char  *getBitmapDatas(HBITMAP bitmap, int *size);
#endif
}

// These next two structs represent how the icon information is stored
// in an ICO file.
typedef struct
{
  unsigned char bWidth;               // Width of the image
  unsigned char bHeight;              // Height of the image (times 2)
  unsigned char bColorCount;          // Number of colors in image (0 if >=8bpp)
  unsigned char bReserved;            // Reserved
  unsigned short wPlanes;              // Color Planes
  unsigned short wBitCount;            // Bits per pixel
  unsigned long dwBytesInRes;         // how many bytes in this resource?
  unsigned long dwImageOffset;        // where in the file is this image
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct
{
  unsigned short      idReserved;   // Reserved
  unsigned short      idType;       // resource type (1 for icons)
  unsigned short      idCount;      // how many images?
  //ICONDIRENTRY  idEntries[1]; // the entries for each image
} ICONDIR, *LPICONDIR;

// 46 bytes
typedef struct{
  BITMAPINFOHEADER   icHeader;      		// DIB header
  unsigned long		icColors[1];   		// Color table (short 4 bytes) //RGBQUAD
  unsigned char            icXOR[1];      // DIB bits for XOR mask
  unsigned char            icAND[1];      // DIB bits for AND mask
} ICONIMAGE, *LPICONIMAGE;
