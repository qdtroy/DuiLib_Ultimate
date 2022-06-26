#include "StdAfx.h"
#include "AnimLayout.h"
#include "../UiFeatureEffect/Interface/IUIEffect.h"

const LPCTSTR AnimLayout::kAnimLayoutClass		= L"AnimLayoutUI";
const LPCTSTR AnimLayout::kAnimLayoutInterface  = L"AnimLayout";

AnimLayout::AnimLayout()
	: m_pEffect( NULL )
	, m_bPlaying( false )
	, m_dwEffectNum(2)
{
	m_pEffectManager = IUiEffectManagerImpl::GetInstance();
	m_pEffect = m_pEffectManager->GetAnimation();
	ASSERT(m_pEffect);
}


AnimLayout::~AnimLayout()
{
	m_pEffect->ClearAllAnimation();
	m_pEffectManager->ReleaseAnimation(m_pEffect);
}


LPCTSTR AnimLayout::GetClass() const
{
	return kAnimLayoutClass;
}

LPVOID AnimLayout::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, kAnimLayoutInterface) == 0) return static_cast<AnimLayout*>(this);
	return CVerticalLayoutUI::GetInterface(pstrName);
}

bool AnimLayout::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	if (!m_bPlaying)
	{
		return __super::DoPaint(hDC, rcPaint, pStopControl);
	}

	typedef BOOL(WINAPI *LPALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	lpAlphaBlend(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, m_hMemDc,
		0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, bf);

	return true;

}
void RestoreAlphaColor(LPBYTE pBits, int bitsWidth, PRECT rc)
{
	for(int i = rc->top; i < rc->bottom; ++i)
	{
		for(int j = rc->left; j < rc->right; ++j)
		{
			int x = (i*bitsWidth + j) * 4;
			if((pBits[x + 3] == 0)&& (pBits[x + 0] != 0 || pBits[x + 1] != 0|| pBits[x + 2] != 0))
				pBits[x + 3] = 255;	
		}
	}
}

bool AnimLayout::StartEffect()
{
	if (m_bPlaying)
		return false;
	if (m_dwEffectNum > 80)
	{
		m_dwEffectNum = 2;
		return false;
	}
		
	//LPDWORD pBmpBits = NULL;
	m_hMemDc = ::CreateCompatibleDC(m_pManager->GetPaintDC());
	m_hTransBitmap = CRenderEngine::GenerateBitmap(m_pManager, this, m_rcItem, 0); 
	if (m_hTransBitmap == NULL)
		return false;
	m_hOldBitmap = (HBITMAP) ::SelectObject(m_hMemDc, m_hTransBitmap);

	BITMAP bmDst;
	GetObject(m_hTransBitmap, sizeof(bmDst), &bmDst);
	SIZE szMemDc = { bmDst.bmWidth, bmDst.bmHeight };

	//修补一下Alpha通道,一些控件(Richedit)会让Alpha为0
	RECT rcRestore = m_rcItem;
	RestoreAlphaColor((LPBYTE)bmDst.bmBits, bmDst.bmWidth, &rcRestore);

	// 填充动画参数
	AnimationParam animParam;
	animParam.effectKey = (WPARAM)this;				//控件指针
	animParam.animationEffect = m_dwEffectNum++;	//动画类型，从2-80，1为自定义动画，并没有移植过来
	animParam.animationFrequency = 20;				//动画间隔
	animParam.bShow = TRUE;							//动画顺序
	animParam.hBitmap = m_hTransBitmap;
	animParam.pBmpData = (BYTE*)bmDst.bmBits;
	animParam.bmpSize = szMemDc;
	animParam.hdc = m_hMemDc;

	BOOL bRet = m_pEffect->AppendAnimation(animParam);
	ASSERT(bRet);

	m_bPlaying = true;

	// 这里是同步执行的，Animation函数在动画完毕后返回
	bRet = m_pEffect->Animation(dynamic_cast<IUIEffectCallBack*>(this), 0);
	ASSERT(bRet);

	// 递归演示所有动画效果,这只是为了演示效果,实际开发不要这样做!
	StartEffect();

	return true;
}


void AnimLayout::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	__super::SetAttribute(pstrName, pstrValue);
}

void AnimLayout::OnUiEffectBegin(WPARAM effectKey, DWORD animaType)
{
	CControlUI* pCtrl = (CControlUI*)effectKey;
	if (pCtrl == NULL)
		return;
}

void AnimLayout::OnUiEffectEnd(WPARAM effectKey, DWORD animaType)
{
	CControlUI* pCtrl = (CControlUI*)effectKey;
	if (pCtrl == NULL)
		return;

	m_bPlaying = false;
	SelectObject(m_hMemDc, m_hOldBitmap);
	DeleteObject(m_hTransBitmap);
	DeleteObject(m_hMemDc);

	Invalidate();

}

void AnimLayout::OnUiEffectDraw()
{
	// 这里必须强制重绘窗体，否则无法显示出动画，很关键
	RedrawWindow(m_pManager->GetPaintWindow(), NULL, NULL,  RDW_INVALIDATE | RDW_UPDATENOW);
}
