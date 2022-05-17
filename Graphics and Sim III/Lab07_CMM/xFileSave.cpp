#include "xFileSave.h"

#define KILL(x) if (x) { x->Release(); x = 0; }


// START CSAVEDATAOBJECT FUNCTION DEFINITIONS -------------------
CSaveDataObject::CSaveDataObject(LPD3DXFILESAVEDATA data)
{
	m_saveData = data;
}

CSaveDataObject::~CSaveDataObject()
{
	Kill();
}

void CSaveDataObject::Kill()
{
	KILL(m_saveData);
}

CSaveDataObject* CSaveDataObject::AddChildSaveDataObject(const GUID rguidTemplate, const char *name, const GUID *id, SIZE_T size, const void *data)
{
	if(!m_saveData)
			return 0;

	LPD3DXFILESAVEDATA saveData = 0;

	if (FAILED(m_saveData->AddDataObject(rguidTemplate, name, id, size, data, &saveData)))
			return 0;

		return new CSaveDataObject(saveData);

}

HRESULT CSaveDataObject::AddDataReference(const char *name, const GUID *id)
{
	if(!m_saveData)
			return NULL;

	return m_saveData->AddDataReference(name, id);
}
// END CSAVEDATAOBJECT FUNCTION DEFINITIONS -------------------


// START CXFILESAVE FUNCTION DEFINITIONS -------------------
CXFileSave::CXFileSave()
{
	m_xFile = 0;
	m_saveObject = 0;
}

CXFileSave::~CXFileSave()
{
	Kill();
}

void CXFileSave::Kill()
{
	KILL(m_saveObject);
	KILL(m_xFile);
}

HRESULT CXFileSave::CreateFile(char *file)
{
	HRESULT hr = D3DXFileCreate(&m_xFile);

	if (FAILED(hr))
		return hr;
#include <rmxftmpl.h>
	hr = m_xFile->RegisterTemplates((void*)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES);

	if (FAILED(hr))
	{
		Kill();
		return hr;
	}

	hr = m_xFile->CreateSaveObject(file, D3DXF_FILESAVE_TOFILE, D3DXF_FILEFORMAT_TEXT, &m_saveObject);

	if (FAILED(hr))
	{
		Kill();
		return hr;
	}

	return S_OK;
}

CSaveDataObject* CXFileSave::CreateSaveDataObject(const GUID rguidTemplate, const char *name, const GUID *id, SIZE_T size, const void *data)
{
	if (!m_saveObject)
		return 0;

	LPD3DXFILESAVEDATA saveData = 0;

	if (FAILED(m_saveObject->AddDataObject(rguidTemplate, name, id, size, data, &saveData)))
		return 0;

	return new CSaveDataObject(saveData);
}

HRESULT CXFileSave::Save()
{
	if (!m_saveObject)
		return E_FAIL;

	return m_saveObject->Save();
}

HRESULT CXFileSave::RegisterTemplates(const void *data, SIZE_T size)
{
	if(!m_xFile)
		return E_FAIL;

	return m_xFile->RegisterTemplates(data, size);
}
// END CXFILESAVE FUNCTION DEFINITIONS -------------------
