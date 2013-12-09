#include "GraphicHandler.hpp"
#include "MyWindow.hpp"
#include "HandleError.hpp"
#include "../Utils/Utils.hpp"
#include "../String_utils.hpp"
#include <cstdlib>

#ifdef WIN32
#include <cstdio>
#include <WinDef.h>
#include <windows.h>
#include <Olectl.h>
#include <Ole2.h>

/*Image* Icon2Image(HICON hIcon)
{
  BITMAP bm;
  ICONINFO iconInfo;
  GetIconInfo(hIcon, &iconInfo);
  GetObject(iconInfo.hbmColor, sizeof(BITMAP),&bm);
  int width = bm.bmWidth;
  int height = bm.bmHeight;
  int bytesPerScanLine = (width * 3 + 3) & 0xFFFFFFFC;
  int size = bytesPerScanLine * height;
  BITMAPINFO infoheader;
  infoheader.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  infoheader.bmiHeader.biWidth = width;
  infoheader.bmiHeader.biHeight = height;
  infoheader.bmiHeader.biPlanes = 1;
  infoheader.bmiHeader.biBitCount = 24;
  infoheader.bmiHeader.biCompression = BI_RGB;
  infoheader.bmiHeader.biSizeImage = size;
  // allocate Memory for Icon RGB data plus Icon mask plus ARGB buffer for the resulting image
  MemoryBlock* pMemBlock = new MemoryBlock((size*2+height*width*4)*sizeof(uint8));
  uint8* pixelsIconRGB = (uint8*)pMemBlock->getData();
  uint8* alphaPixels	 = (uint8*)(pixelsIconRGB+size);
  uint32* imagePixels   = (uint32*)(alphaPixels+size);
  HDC hDC = CreateCompatibleDC(NULL);
  // Get Icon RGB data
  HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, (HGDIOBJ)iconInfo.hbmColor);
  if(!GetDIBits(hDC, iconInfo.hbmColor, 0, height, (LPVOID) pixelsIconRGB, &infoheader, DIB_RGB_COLORS))
    return NULL;
  SelectObject(hDC, hBmpOld);
  // now get the mask
  if(!GetDIBits(hDC, iconInfo.hbmMask, 0,height,(LPVOID)alphaPixels, &infoheader, DIB_RGB_COLORS))
    return NULL;
  int x=0;
  int currentSrcPos=0;
  int currentDestPos=0;
  int linePosSrc = 0;
  int linePosDest = 0;
  int lsSrc = width*3;
  int vsDest = height-1;
  for(int y=0; y<height; y++)
    {
      linePosSrc = (vsDest-y)*lsSrc;
      linePosDest = y*width;
      for(x=0; x<width; x++)
        {
          //pixels from Icon are stored in BGR vertical and horizontal flipped order
          currentDestPos = linePosDest+x;
          currentSrcPos  = linePosSrc+x*3;
          // BGR -> ARGB
          imagePixels[currentDestPos]=(((uint32)((((pixelsIconRGB[currentSrcPos+2] << 0x10 /*Red*//*) | (pixelsIconRGB[currentSrcPos+1] << 8 /*Green*//*))
                                                  | pixelsIconRGB[currentSrcPos] /*Blue*//*) | ((alphaPixels[currentSrcPos]?0:255) << 0x18))) & 0xffffffffL);
        }
    }
  Image *pImage = new Image(Image::ARGB, width, height,true);
  pImage->setPixelData (0, 0, width, height, (uint8*)imagePixels, width*4);
  deleteAndNull(pMemBlock);
  DeleteDC(hDC);
  return pImage;
}*/
#endif

TextureHandler  *GraphicHandler::tex_container = 0;

GraphicHandler::GraphicHandler(const std::string &winName, MyWindow *w, unsigned int anti_ali)
    : m_name(winName), m_win(w), fullscreen(false), m_elapsedTime(0), m_anti_ali(anti_ali)
{
  if (!m_win)
    {
      HandleError::showError("GraphicHandler: Invalid null pointer in constructor");
      exit(-1);
    }
  if (!tex_container){
      try {
        tex_container = new TextureHandler;
      } catch (std::bad_alloc &err){
        HandleError::showError(err.what());
        exit(-1);
      }
    }
}

GraphicHandler::~GraphicHandler()
{
  if (tex_container){
      delete tex_container;
      tex_container = 0;
    }
}

bool  GraphicHandler::destroyTexture(GLuint id)
{
  if (!tex_container->destroy(id))
    glDeleteTextures(1, &id);
  return true;
}

int GraphicHandler::height()
{
  return screenHeight;
}

int GraphicHandler::width()
{
  return screenWidth;
}

int GraphicHandler::loadIconFile(std::string s)
{
  if (s.empty())
    return 0;
  return loadIconFile(s.c_str());
}

int GraphicHandler::loadIconFile(const char *s)
{
  if (!s)
    return 0;
  std::string extension = s;
  size_t  p;

  if ((p = extension.find_last_of('.')) != extension.npos)
    extension = extension.substr(p);
  std::vector<std::string> tmp = Utility::split<std::string>(s, "/");

  std::string fullname = "./textures/" + std::string(tmp[tmp.size() - 1]) + ".bmp";

  GLuint  glID(0);
  int     width, height, bitsPerPixel;

  if (extension != ".exe")
   glID = tex_container->get(extension);

  if (glID > 0)
    return glID;

  HBITMAP data = Utils::getIconFromFileType(s, &width, &height, &bitsPerPixel);

  /*PICTDESC desc = { sizeof(PICTDESC) };
  desc.picType = PICTYPE_ICON;
  desc.icon.hicon = data;
  IPicture* pPicture = 0;
  HRESULT hr = OleCreatePictureIndirect(&desc, IID_IPicture, FALSE, (void**)&pPicture);
  if (FAILED(hr))
    return hr;

  // Create a stream and save the image
  IStream* pStream = 0;
  CreateStreamOnHGlobal(0, TRUE, &pStream);
  LONG cbSize = 0;
  hr = pPicture->SaveAsFile(pStream, TRUE, &cbSize);

  // Write the stream content to the file
  if (!FAILED(hr)) {
      HGLOBAL hBuf = 0;
      GetHGlobalFromStream(pStream, &hBuf);
      void* buffer = GlobalLock(hBuf);
      HANDLE hFile = CreateFile(Utils::string2wstring(fullname).c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
      if (!hFile)
        hr = HRESULT_FROM_WIN32(GetLastError());
      else {
          DWORD written = 0;
          WriteFile(hFile, buffer, cbSize, &written, 0);
          CloseHandle(hFile);
        }
      GlobalUnlock(buffer);
    }
  // Cleanup
  pStream->Release();
  pPicture->Release();

  if (!FAILED(hr))
    glID = loadTexture(fullname, false);

  //on supprime cette merde
  remove(fullname.c_str());*/

  if (!data)
    return 0;
  fullname = (extension != ".exe" ? extension : fullname);
  //if (!Utils::saveBMP(fullname.c_str(), data))
    //return 0;
  //DeleteObject(data);
  //glID = loadTexture(fullname, false);
  //remove(fullname.c_str());
  int size;
  char  *datas = Utils::getBitmapDatas(data, &size);
  glID = loadTextureFromMemory(datas, size, extension.c_str());
  delete[] datas;
  return glID;
}
