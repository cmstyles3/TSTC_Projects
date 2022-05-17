



#ifndef XFILESAVE_H
#define XFILESAVE_H

#include <d3dx9xof.h>
#include <dxfile.h>
#include <rmxfguid.h>
//
#include <initguid.h>

class CSaveDataObject
{
	
	private:
		LPD3DXFILESAVEDATA m_saveData;

	public:
		CSaveDataObject(LPD3DXFILESAVEDATA data);
		~CSaveDataObject();

		void Kill();

		LPD3DXFILESAVEDATA GetSaveDataInterface() const {return m_saveData;}

		CSaveDataObject* AddChildSaveDataObject(const GUID rguidTemplate, const char* name, const GUID *id, SIZE_T size, const void* data);

		HRESULT AddDataReference(const char* name, const GUID *id);

};
	

class CXFileSave
{
	private:
		LPD3DXFILE m_xFile;
		LPD3DXFILESAVEOBJECT m_saveObject;
	public:
		CXFileSave();
		~CXFileSave();
		void Kill();

		HRESULT CreateFile(char* File);

		CSaveDataObject* CreateSaveDataObject(const GUID rguidTemplate, const char* name, const GUID *id, SIZE_T size, const void* data);

		HRESULT Save();

		HRESULT RegisterTemplates(const void* data, SIZE_T size);
	

};

#endif
