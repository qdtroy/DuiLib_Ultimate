#pragma once
#include "IEffect.h"
#include "Graph/Graph_Def.h"
#include "Graph/Graph_Rect.h"
#include "Image/ImageProcess.h"

class CEffect : public IEffect
{
public:
	virtual ~CEffect(){};
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam){};
	void InitEffectParam(InternalAnimationParam *internalParam){};
	void ReleaseEffectParam(){};
protected:
	int		 m_effectFrame;
};

class CFlipEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CMosaicEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int		 m_nWidth;					// 图片宽
	int		 m_nHeight;					// 图片高
	int		 m_nMosaicW;				// 马赛克宽
	int		 m_nMosaicH;				// 马赛克高
	int		 m_nWidthCount;				// 马赛克横向个数
	int		 m_nHeightCount;			// 马赛克纵向个数
	int*	 m_pMosaicArray;			// 每个马赛克是否执行完
};

class CSlideEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
protected:
	void  CalculateFrame(int length);
protected:
	int		 m_step;					// 每次滑动的距离(speed)
};

class CBandSlideEffect : public CSlideEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int		m_bandNum;					// 带状个数
	int		m_bandSize;					// 带状的宽或者高
};

class CBlindsEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
protected:
	int		m_bandNum;					// 百叶个数
	int		m_bandSize;					// 百叶的宽或者高
};

class CSepcialBlindsEffect : public CBlindsEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	void CalculateFrame(int length);
private:
	vector<int> m_specialBandSize;		// 百叶的宽或者高
	int			m_nowSumRowCol;			// 当前计算起始行或者列
};

class CScanEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
protected:
	void  CalculateFrame(int length);
protected:
	int		 m_step;					// 每次扫描的距离(speed)
};

class CInertiaSlideEffect : public CSlideEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	Graph::real	m_tInertiaTime;			// 惯性运动当前时间
	void*	m_inertiaInstance;			// 惯性辅助类
	Graph::real	m_tInertiacoeffecient;	// 惯性运动速率调节参数
};

class CFadeEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CZoomEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	Graph::CdRect				m_rcNow;		// 变换完当前的大小
};

class CShrinkEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	Graph::CdRect				m_rcNow;		// 变换完当前的大小
};

class CSwingEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CFunnelEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CNoiseEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int							m_u;			// 方差
};

class CCubeEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CGradualEraseEffect : public CScanEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	Graph::real		m_alphaStep;			// alpha渐变值
	int				m_lastStart;			// 上一次渐变起始行或者列
	int				m_lastEnd;				// 上一次渐变结束行或者列
};

class CPushEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CDissolveEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	BYTE*			m_pDissolveArray;		// 标示是否已被溶解
	int				m_size;					// 所有像素数量
	int				m_framePixel;			// 每帧溶解的个数
};

class CCurtainEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CTriangularprismEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CCubeInsideEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CSpatialmotionEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int			m_xyOffset;						// x或y轴步长
	int			m_zOffset;						// z轴步长
};

class CPatchFlipEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CSpatialRotateEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CDoorEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CWaveEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	CWater			*m_pWater;						// 水波粒子系统
};

class CScatterEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int				m_vx[4];						// 水平速度
	int				m_vy[4];						// 垂直速度
};

class CColorFadeEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CDampSlideEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	Graph::real	m_tInertiaTime;			// 惯性运动当前时间
	void*		m_inertiaInstance;		// 惯性辅助类
	Graph::real	m_tInertiacoeffecient;	// 惯性运动速率调节参数
	Graph::real m_ratio;				// 图像的长宽比
};

class CWhirlPoolEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
};

class CClockWipeEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:

	Bitmap*		m_pBitmap;					// 数据
	BYTE*		m_pBits;					// 数据
};

class CPinWheelEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:

	Bitmap*		m_pBitmap;					// 数据
	BYTE*		m_pBits;					// 数据
};

class CPagePeelEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:

	Bitmap*		m_pBitmap;					// 数据
	BYTE*		m_pBits;					// 数据
	real		m_nK;						// 系数
	BOOL		m_intersectTop;				// 与顶部相交
};

class CBlurEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
};

class CZoomBlurEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
};

class CFoldEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int					m_foldNum;					// 折叠效果的折数
	vector<CdRect>		m_foldRect;					// 每个折的RECT
	vector<WarpRect>	m_warpRect;					// 扭曲的rect
};

class CStackEffect : public CEffect
{
public:
	// IEffect
	void ComputeOneFrame(InternalAnimationParam *internalParam);
	void InitEffectParam(InternalAnimationParam *internalParam);
	void ReleaseEffectParam();
private:
	int				m_bandNum;					// 带状的条数
	int				m_bandSize;					// 带状的宽或者高
	vector<int>		m_bandFrameNum;				// 每条的帧数
	int				m_bandAnimating;			// 当前运动的带状序号
	int				m_bandFrameSum;				// 在当前带状前面的带状累积完成的帧数
	int				m_bandSubFrame;				// 当前带状的执行帧数
};

/////////////////////////////////////////////////////////////////////////////////////////////
//特效工厂
class CEffectFactory/* : public IFactory*/
{
public:
	static IEffect* CreateEffect(DWORD effectKey)
	{
		IEffect* pEffect = NULL;
		switch (effectKey)
		{
		case 2:
		case 3:
		case 4:
		case 5:
			pEffect = new CFlipEffect();
			break;
		case 6:
			pEffect = new CMosaicEffect();
			break;
		case 7:
		case 8:
		case 9:
		case 10:
			pEffect = new CSlideEffect();
			break;
		case 11:
		case 12:
			pEffect = new CBandSlideEffect();
			break;
		case 13:
		case 14:
			pEffect = new CBlindsEffect();
			break;
		case 15:
		case 16:
			pEffect = new CSepcialBlindsEffect();
			break;
		case 17:
		case 18:
		case 19:
		case 20:
			pEffect = new CScanEffect();
			break;
		case 21:
		case 22:
		case 23:
		case 24:
			pEffect = new CInertiaSlideEffect();
			break;
		case 25:
			pEffect = new CFadeEffect();
			break;
		case 26:
			pEffect = new CZoomEffect();
			break;
		case 27:
			pEffect = new CShrinkEffect();
			break;
		case 28:
			pEffect = new CSwingEffect();
			break;
		case 29:
			pEffect = new CFunnelEffect();
			break;
		case 30:
			pEffect = new CNoiseEffect();
			break;
		case 31:
		case 32:
		case 33:
		case 34:
			pEffect = new CCubeEffect();
			break;
		case 35:
		case 36:
		case 37:
		case 38:
			pEffect = new CGradualEraseEffect();
			break;
		case 39:
		case 40:
		case 41:
		case 42:
			pEffect = new CPushEffect();
			break;
		case 43:
			pEffect = new CDissolveEffect();
			break;
		case 44:
			pEffect = new CCurtainEffect();
			break;
		case 45:
		case 46:
		case 47:
		case 48:
			pEffect = new CTriangularprismEffect();
			break;
		case 49:
		case 50:
		case 51:
		case 52:
			pEffect = new CCubeInsideEffect();
			break;
		case 53:
		case 54:
		case 55:
		case 56:
			pEffect = new CSpatialmotionEffect();
			break;
		case 57:
			pEffect = new CPatchFlipEffect();
			break;
		case 58:
		case 59:
		case 60:
		case 61:
			pEffect = new CSpatialRotateEffect();
			break;
		case 62:
			pEffect = new CDoorEffect();
			break;
		case 63:
			pEffect = new CWhirlPoolEffect();
			break;
		case 64:
			pEffect = new CScatterEffect();
			break;
		case 65:
			pEffect = new CColorFadeEffect();
			break;
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
			pEffect = new CDampSlideEffect();
			break;
		case 74:
			pEffect = new CClockWipeEffect();
			break;
		case 75:
			pEffect = new CPinWheelEffect();
			break;
		case 76:
			pEffect = new CPagePeelEffect();
			break;
		case 77:
			pEffect = new CBlurEffect();
			break;
		case 78:
			pEffect = new CWaveEffect();
			break;
		case 79:
			pEffect = new CZoomBlurEffect();
			break;
		case 80:
		case 81:
			pEffect = new CFoldEffect();
			break;
		case 82:
		case 83:
		case 84:
		case 85:
			pEffect = new CStackEffect();
			break;
		default:
			break;
		}

		return pEffect;
	}
};
/////////////////////////////////////////////////////////////////////////////////////////////