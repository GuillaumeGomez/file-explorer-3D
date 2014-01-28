#include "VertexBufferObject.hpp"
#include "myGLWidget.hpp"

VertexBufferObject::VertexBufferObject() : m_buffer(0), m_dataUploaded(false), m_bufferType(GL_ARRAY_BUFFER)
{
  glGenBuffers(1, &m_buffer);
}

/*-----------------------------------------------

Name:	DeleteVBO

Params:	none

Result:	Releases VBO and frees all memory.

/*---------------------------------------------*/

VertexBufferObject::~VertexBufferObject()
{
  if (m_dataUploaded)
    glDeleteBuffers(1, &m_buffer);
  m_datas.clear();
}

/*-----------------------------------------------

Name:	mapBufferToMemory

Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...

Result:	Maps whole buffer data to memory and
                returns pointer to data.

/*---------------------------------------------*/

/*void *VertexBufferObject::MapBufferToMemory(int iUsageHint)
{
  if (!m_dataUploaded)
    return 0;
  return glMapBuffer(m_bufferType, iUsageHint);
}*/

/*-----------------------------------------------

Name:	MapSubBufferToMemory

Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
		uiOffset - data offset (from where should
					data be mapped).
		uiLength - length of data

Result:	Maps specified part of buffer to memory.

/*---------------------------------------------*/

/*void* VertexBufferObject::MapSubBufferToMemory(int iUsageHint, GLuint uiOffset, GLuint uiLength)
{
  if (!bDataUploaded)
    return 0;
  return glMapBufferRange(m_bufferType, uiOffset, uiLength, iUsageHint);
}*/

/*-----------------------------------------------

Name:	UnmapBuffer

Params:	none

Result:	Unmaps previously mapped buffer.

/*---------------------------------------------*/

/*void VertexBufferObject::UnmapBuffer()
{
  glUnmapBuffer(iBufferType);
}*/

/*-----------------------------------------------

Name:	BindVBO

Params:	a_iBufferType - buffer type (GL_ARRAY_BUFFER, ...)

Result:	Binds this VBO.

/*---------------------------------------------*/

/*void VertexBufferObject::BindVBO(int a_iBufferType)
{
  if (m_dataUploaded)
    return;
  m_bufferType = a_iBufferType;
  glBindBuffer(m_bufferType, m_buffer);
}*/

/*-----------------------------------------------

Name:	UploadDataToGPU

Params:	iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...

Result:	Sends data to GPU.

/*---------------------------------------------*/

/*bool VertexBufferObject::UploadDataToGPU(int iDrawingHint)
{
  if (!m_datas.size())
    return false;
  glBufferData(m_bufferType, m_datas.size(), &m_datas[0], iDrawingHint);
  bDataUploaded = true;
  m_datas.clear();
  return true;
}*/

/*-----------------------------------------------

Name:	AddData

Params:	ptrData - pointer to arbitrary data
                uiDataSize - data size in bytes

Result:	Adds arbitrary data to VBO.

/*---------------------------------------------*/

/*void VertexBufferObject::AddData(void* ptrData, UINT uiDataSize)
{
  data.insert(data.end(), (BYTE*)ptrData, (BYTE*)ptrData+uiDataSize);
  iCurrentSize += uiDataSize;
}*/

/*-----------------------------------------------

Name:	GetDataPointer

Params:	none

Result:	Returns data pointer (only before uploading).

/*---------------------------------------------*/

/*void *VertexBufferObject::GetDataPointer()
{
  if (m_dataUploaded)
    return 0;
  return (void*)m_datas[0];
}*/

/*-----------------------------------------------

Name:	GetBufferID

Params:	none

Result:	Returns VBO ID.

/*---------------------------------------------*/

/*GLuint VertexBufferObject::GetBufferID()
{
  return m_buffer;
}*/

/*-----------------------------------------------

Name:	GetCurrentSize

Params:	none

Result: Returns size of data that has been added to
                the buffer object.

/*---------------------------------------------*/

/*int VertexBufferObject::GetCurrentSize()
{
  return iCurrentSize;
}*/
