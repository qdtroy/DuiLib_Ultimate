// UIEffect.cpp : Defines the exported functions for the DLL application.
//
#include "StdAfx.h"
#include <time.h>
#include "UIFeatureEffect.h"

IUIEffect* GetAnimation(void)
{
	// 创建对象实例
	IUIEffect *pEffect =  dynamic_cast<IUIEffect*>(new CWndEffect());
	return pEffect != NULL ? pEffect :NULL;
}

BOOL ReleaseAnimation(IUIEffect* &pUIEffect)
{
	// 释放，置空
	CWndEffect* pWndEffect =  dynamic_cast<CWndEffect*>(pUIEffect);
	delete pWndEffect;
	pUIEffect = NULL;

	return TRUE;
}

DWORD GetSurportAnimationType(const char *& strAnimationType)
{
	return CWndEffect::InitSurportAnimationType(strAnimationType);
}

IImageProcess* GetImageProcess(void)
{
	// 创建对象实例
	IImageProcess *pProcess =  dynamic_cast<IImageProcess*>(new CAlphaBlendImpl());
	return pProcess != NULL ? pProcess : NULL;
}

BOOL ReleaseImageProcess(IImageProcess* &pProcess)
{
	// 释放，置空
	IImageProcess* pImageProcess =  dynamic_cast<CAlphaBlendImpl*>(pProcess);
	delete pImageProcess;
	pImageProcess = NULL;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ClearContainerData(InternalAnimationParam &pElem)
{
	// 删除拷贝的内存
	delete []pElem.bmpDataCopy;
	pElem.bmpDataCopy = NULL;
	// 删除特效类
	pElem.pEffect->ReleaseEffectParam();
	delete dynamic_cast<CEffect*>(pElem.pEffect);
	pElem.pEffect = NULL;
}

CWndEffect::CWndEffect()
{	
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CWndEffect::~CWndEffect()
{
	// 清理资源
	for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();

	if(m_gdiplusToken != 0)
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
}

DWORD CWndEffect::InitSurportAnimationType(const char *& strAnimationType)
{
	strAnimationType = m_animation;
	return ANIMATIONNUM;
}

BOOL CWndEffect::AppendAnimation(AnimationParam &aParam)
{
	if(aParam.animationEffect > ANIMATIONNUM+1 || aParam.animationEffect <= 1)
	{
		return FALSE;
	}

	InternalAnimationParam internalParam;
	memcpy(&internalParam.param,&aParam,sizeof(aParam));
	DWORD dataSize = internalParam.param.bmpSize.cx*internalParam.param.bmpSize.cy*4;
	internalParam.bmpDataCopy = new BYTE[dataSize];

	if(internalParam.bmpDataCopy == NULL)
	{
		return FALSE;
	}

	memcpy(internalParam.bmpDataCopy,aParam.pBmpData,dataSize);
	internalParam.frameNow = 0;
	internalParam.bLastFrame = FALSE;

	IEffect *pEffect = CEffectFactory::CreateEffect(aParam.animationEffect);
	internalParam.pEffect = pEffect;
	pEffect->InitEffectParam(&internalParam);
	
	m_animationContainer.push_back(internalParam);

	return TRUE;
}

BOOL CWndEffect::DependAnimation(WPARAM effectKey)
{
	BOOL bFind = FALSE;
	for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	{
		if(m_itAnimation->param.effectKey == effectKey)
		{
			delete []m_itAnimation->bmpDataCopy;
			CEffect *pEffect = dynamic_cast<CEffect*>(m_itAnimation->pEffect);
			pEffect->ReleaseEffectParam();
			delete pEffect;
			m_animationContainer.erase(m_itAnimation);

			bFind = TRUE;
		}
	}

	return  bFind;
}

BOOL CWndEffect::ClearAllAnimation()
{
	for_each(m_animationContainer.begin(),m_animationContainer.end(),ClearContainerData);
	m_animationContainer.clear();

	return TRUE;
}

BOOL CWndEffect::Animation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin)
{
	if(iDrawEffect == NULL)
	{
		return FALSE;
	}

	DWORD time_start = clock();;
	DWORD delta_time = 0;

	// 队列非空，执行动画
	while (m_animationContainer.size() != 0)
	{
		ComputeAnimation(iDrawEffect,delta_time);

		iDrawEffect->OnUiEffectDraw();

		CleanFinishedAnimation(iDrawEffect);

		Sleep(1);

		delta_time = clock() - time_start;
	}

	return TRUE;
}

void CWndEffect::ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse)
{
	// 此循环过程中不能插入
	for(m_itAnimation = m_animationContainer.begin();m_itAnimation != m_animationContainer.end(); m_itAnimation++)
	{	
		IEffect *pEffect = (IEffect *)m_itAnimation->pEffect;
		if((!m_itAnimation->frameNow))
		{
			// 第一帧
			m_itAnimation->frameNow++;
			pEffect->ComputeOneFrame(&*m_itAnimation);
			// 通知绘制模块
			iDrawEffect->OnUiEffectBegin(m_itAnimation->param.effectKey, m_itAnimation->param.animationEffect);
		}
		else if(timeElapse / m_itAnimation->param.animationFrequency >= m_itAnimation->frameNow)
		{
			// 符合下一帧条件，重新计算
			m_itAnimation->frameNow++;
			pEffect->ComputeOneFrame(&*m_itAnimation);
		}

	}
}

void CWndEffect::CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect)
{
	// 绘制完成后删除完成的动画
	int sizeVec = m_animationContainer.size();
	for(int i = 0; i < sizeVec; i++)
	{
		if(m_animationContainer[i].bLastFrame)
		{
			iDrawEffect->OnUiEffectEnd(m_animationContainer[i].param.effectKey, m_animationContainer[i].param.animationEffect);
			delete []m_animationContainer[i].bmpDataCopy;
			CEffect *pEffect = dynamic_cast<CEffect*>(m_animationContainer[i].pEffect);
			pEffect->ReleaseEffectParam();
			delete pEffect;
			m_animationContainer.erase(m_animationContainer.begin()+i);
			sizeVec = m_animationContainer.size();
		}

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CAlphaBlendImpl::AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha, int interpolationMode)
{
	return CAlphaBlend::AlphaBlend(desData, desStride, desLeft, desTop, desWidth, desHeight, srcData, srcStride, srcLeft, srcTop, srcWidth, srcHeight, sourceAlpha, interpolationMode);
}

BOOL CAlphaBlendImpl::ImageScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode)
{
	return CScale::ImageScale(DataDes, desWidth, desHeight, DataSrc, srcStride, srcLeft, srcTop, srcWidth, srcHeight, interpolationMode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
