
#pragma once
#include "..\UiFeatureEffect\Interface\IUIEffect.h"

class IUiEffectManagerImpl
{
public:
	IUiEffectManagerImpl();
	virtual ~IUiEffectManagerImpl();

	static IUiEffectManagerImpl* GetInstance();

	// 得到一个动画对象
	IUIEffect* GetAnimation();
	// 删除一个动画对象
	bool ReleaseAnimation(IUIEffect* &pUIEffect);
	DWORD GetSurportAnimationType(const char* &strType);

private:
	bool LoadEffectDll();

private:
	HMODULE m_hEffectDll;
	GETANIMATION m_funGetAnimation;
	RELEASEANIMATION m_funReleaseAnimation;
	GETSURPORTANIMATIONTYPE m_funGetSurportAnimationType;
};
