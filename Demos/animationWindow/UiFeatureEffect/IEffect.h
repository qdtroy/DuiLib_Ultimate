#pragma once
#include "Interface\IUIEffect.h"

class IEffect;
struct InternalAnimationParam
{
	AnimationParam  param;				// 接口定义的动画参数
	BYTE*			bmpDataCopy;		// 外部动画图片资源备份
	DWORD			frameNow;			// 当前运行帧
	BOOL			bLastFrame;			// 是否是最后一帧
	IEffect*		pEffect;			// 指向特效函数接口指针
};

//特效接口
class IEffect
{
public:
	// 计算一帧
	virtual void ComputeOneFrame(InternalAnimationParam *internalParam) = 0;
	// 初始化特效参数
	virtual void InitEffectParam(InternalAnimationParam *internalParam) = 0;
	// 清理特效参数
	virtual void ReleaseEffectParam() = 0;
};