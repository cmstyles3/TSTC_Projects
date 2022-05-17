#include "xFileLoad.h"

#define KILL(x) if (x) { x->Release(); x = 0; }


CDataObject::CDataObject(LPD3DXFILEDATA data)
{
	m_fileData = data;
	m_name = 0;
}

CDataObject::~CDataObject()
{
	Kill();
}

void CDataObject::Kill()
{
	KILL(m_fileData);
}

SIZE_T CDataObject::GetChildCount()
{
	if (!m_fileData)
		return 0;

	SIZE_T size = 0;

	if (FAILED(m_fileData->GetChildren(&size)))
		return 0;

	return size;
}

CDataObject* CDataObject::GetChild(SIZE_T object)
{
	if(!m_fileData)
		return 0;

	LPD3DXFILEDATA data = 0;

	if (FAILED(m_fileData->GetChild(object, &data)))
		return 0;

	return new CDataObject(data);
}

HRESULT CDataObject::GetData()
{

	if(!m_fileData)
		return E_FAIL;

	SIZE_T size;

	HRESULT hr = m_fileData->GetName(NULL, &size);

	if (FAILED(hr))
		return hr;

	if (m_name)
		delete [] m_name;

	m_name = new char[size];

	hr = m_fileData->GetName(m_name, &size);

	if (FAILED(hr))
		return hr;

	hr = m_fileData->GetType(&m_ID);

	if (FAILED(hr))
		return hr;

	m_isReference = m_fileData->IsReference();

	return S_OK;
}

HRESULT CDataObject::Lock(SIZE_T *size, const void **data)
{
	if (!m_fileData)
		return E_FAIL;

	return m_fileData->Lock(size, data);
}

HRESULT CDataObject::Unlock()
{
	if (!m_fileData)
		return false;

	return m_fileData->Unlock();
}

CXFileLoad::CXFileLoad()
{
	m_xFile = 0;
	m_enumObject = 0;
}

CXFileLoad::~CXFileLoad()
{
	Kill();
}

void CXFileLoad::Kill()
{
	KILL(m_enumObject);
	KILL(m_xFile);
}

SIZE_T CXFileLoad::GetChildCount()
{
	if (!m_enumObject)
		return 0;

	SIZE_T count = 0;

	if (FAILED(m_enumObject->GetChildren(&count)))
		return 0;

	return count;
}

CDataObject* CXFileLoad::GetChild(SIZE_T object)
{
	if (!m_enumObject)
		return 0;

	LPD3DXFILEDATA data = 0;

	if (FAILED(m_enumObject->GetChild(object, &data)))
		return 0;

	return new CDataObject(data);
}

HRESULT CXFileLoad::LoadFromFile(char *file)
{
	HRESULT hr = D3DXFileCreate(&m_xFile);

	if(FAILED(hr))
		return hr;
#include <rmxftmpl.h>
	hr = m_xFile->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES);

	if (FAILED(hr))
	{
		Kill();
		return hr;
	}

	hr = m_xFile->CreateEnumObject(file, D3DXF_FILELOAD_FROMFILE, &m_enumObject);

	if (FAILED(hr))
	{
		Kill();
		return hr;
	}

	return S_OK;
}

HRESULT CXFileLoad::RegisterTemplates(LPCVOID pvData, SIZE_T cbSize)
{
	if(!m_xFile)
		return E_FAIL;

	return m_xFile->RegisterTemplates(pvData, cbSize);
}