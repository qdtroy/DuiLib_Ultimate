#ifndef UI_ANIM_BOX_H_
#define UI_ANIM_BOX_H_
#pragma once

#include "../UIEffect/IUiEffectManagerImpl.h"
#include "memory"

class AnimLayout : public CVerticalLayoutUI, public IUIEffectCallBack
{
public:
	AnimLayout();
	~AnimLayout();

	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) override;
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

	bool StartEffect();
	void OnTimer(int iCurFrame);

public:
	// 每一 个 动画开始时回调
	virtual void OnUiEffectBegin(WPARAM effectKey, DWORD animaType) override;
	// 每一 个 动画结束时回调
	virtual void OnUiEffectEnd(WPARAM effectKey, DWORD animaType) override;
	// 每一 帧 动画绘制时回调
	virtual void OnUiEffectDraw() override;

private:

	CDuiRect			m_rcCustomShadow;		//保存窗体反转前的阴影九宫格

	bool				m_bPlaying;
	HDC					m_hMemDc;
	HBITMAP				m_hOldBitmap;
	HBITMAP				m_hTransBitmap;

	DWORD						m_dwEffectNum;
	IUiEffectManagerImpl*		m_pEffectManager;
	IUIEffect*					m_pEffect;

public:
	static const LPCTSTR kAnimLayoutClass;
	static const LPCTSTR kAnimLayoutInterface;
};

#endif // UI_ANIM_BOX_H_
