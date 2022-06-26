
#include "StdAfx.h"
#include "shlwapi.h"
#include "..\MacroDefine.h"
#include "IUiEffectManagerImpl.h"
#pragma comment(lib, "shlwapi.lib")

IUiEffectManagerImpl::IUiEffectManagerImpl()
{
	m_hEffectDll = NULL;
	m_funGetAnimation = NULL;
	m_funReleaseAnimation = NULL;
	m_funGetSurportAnimationType = NULL;

	LoadEffectDll();
}

IUiEffectManagerImpl::~IUiEffectManagerImpl()
{
	m_funGetAnimation = NULL;
	m_funReleaseAnimation = NULL;
	m_funGetSurportAnimationType = NULL;
	SAFE_FREE_LIBRARY(m_hEffectDll);
}

IUiEffectManagerImpl* IUiEffectManagerImpl::GetInstance()
{
	static IUiEffectManagerImpl _UiEffectManagerImplInstance;
	return &_UiEffectManagerImplInstance;
}

bool IUiEffectManagerImpl::LoadEffectDll()
{
	if (m_hEffectDll != NULL)
		return true;
	
	LPCWSTR pszFileName = L"UiFeatureEffect.dll";

	static WCHAR _sszDir[MAX_PATH + 1];
	memset(_sszDir, 0, sizeof(_sszDir));
	::GetModuleFileNameW(NULL, _sszDir, MAX_PATH);
	::PathRemoveFileSpecW(_sszDir);

	wcscat_s(_sszDir, MAX_PATH, L"\\");
	if (pszFileName != NULL && wcslen(pszFileName) > 0)
		wcscat_s(_sszDir, MAX_PATH, pszFileName);


	std::wstring strPath = _sszDir;
	if (strPath.length() <= 0)
		return false;

	m_hEffectDll = ::LoadLibraryW(strPath.c_str());
	if (m_hEffectDll == NULL)
		return false;

	m_funGetAnimation = (GETANIMATION)::GetProcAddress(m_hEffectDll, "GetAnimation");
	if (m_funGetAnimation == NULL)
	{
		m_funGetAnimation = NULL;
		m_funReleaseAnimation = NULL;
		m_funGetSurportAnimationType = NULL;
		SAFE_FREE_LIBRARY(m_hEffectDll);
		return false;
	}

	m_funReleaseAnimation = (RELEASEANIMATION)::GetProcAddress(m_hEffectDll, "ReleaseAnimation");
	if (m_funReleaseAnimation == NULL)
	{
		m_funGetAnimation = NULL;
		m_funReleaseAnimation = NULL;
		m_funGetSurportAnimationType = NULL;
		SAFE_FREE_LIBRARY(m_hEffectDll);
		return false;
	}

	m_funGetSurportAnimationType = (GETSURPORTANIMATIONTYPE)::GetProcAddress(m_hEffectDll, "GetSurportAnimationType");
	if (m_funGetSurportAnimationType == NULL)
	{
		m_funGetAnimation = NULL;
		m_funReleaseAnimation = NULL;
		m_funGetSurportAnimationType = NULL;
		SAFE_FREE_LIBRARY(m_hEffectDll);
		return false;
	}

	return true;
}

// 得到一个动画对象
IUIEffect* IUiEffectManagerImpl::GetAnimation()
{
	if (m_hEffectDll == NULL || m_funGetAnimation == NULL)
		return NULL;

	return m_funGetAnimation();
}

// 删除一个动画对象
bool IUiEffectManagerImpl::ReleaseAnimation(IUIEffect* &pUIEffect)
{
	if (m_hEffectDll == NULL || m_funReleaseAnimation == NULL)
		return false;

	return (m_funReleaseAnimation(pUIEffect) == TRUE);
}

DWORD IUiEffectManagerImpl::GetSurportAnimationType(const char* &strType)
{
	if (m_hEffectDll == NULL || m_funGetSurportAnimationType == NULL)
		return 0;

	return m_funGetSurportAnimationType(strType);
}
