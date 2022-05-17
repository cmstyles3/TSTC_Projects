#ifndef XFILELOAD_H
#define XFILELOAD_H

#include <d3dx9xof.h>
#include <dxfile.h>
#include <rmxfguid.h>
#include <initguid.h>

class CDataObject
{
	
	private:
		LPD3DXFILEDATA m_fileData;
		char *m_name;
		GUID m_ID;
		void *m_data;
		BOOL m_isReference;

	public:
		CDataObject(LPD3DXFILEDATA data);
		~CDataObject();

		void Kill();

		BOOL IsReference() const { return m_isReference; }
		char* GetName() const { return m_name; }
		GUID GetType() const { return m_ID; }
		LPD3DXFILEDATA GetDataInterface() const { return m_fileData; }

		SIZE_T GetChildCount();
		
		CDataObject* GetChild(SIZE_T object);

		HRESULT GetData();

		HRESULT Lock(SIZE_T *Data, const void **ppData);

		HRESULT Unlock();

};

class CXFileLoad
{
	private:
		LPD3DXFILE m_xFile;
		LPD3DXFILEENUMOBJECT m_enumObject;
	public:
		CXFileLoad();
		~CXFileLoad();
		void Kill();

		SIZE_T GetChildCount();
		CDataObject* GetChild(SIZE_T Object);
		HRESULT LoadFromFile(char *file);
		
		HRESULT RegisterTemplates(const void* data, SIZE_T size);
	

};
#endif
