#pragma once
#include "Effect.h"

#define ANIMATIONNUM	85

// 动画的内部种类区分，顺序不能调整
//enum AnimationEffect
//{
//	EFFECT_FLIPLEFT = 2,					//	向左翻转
//	EFFECT_FLIPRIGHT,						//	向右翻转
//	EFFECT_FLIPBOTTOM,						//	向下翻转
//	EFFECT_FLIPUP = 5,						//	向上翻转
//	EFFECT_MOSAIC,							//	马赛克
//	EFFECT_SLIDELEFT,						//	向左滑动
//	EFFECT_SLIDERIGHT,						//	向右滑动
//	EFFECT_SLIDEUP,							//	向上滑动
//	EFFECT_SLIDEBOTTOM = 10,				//	向下滑动
//	EFFECT_BANDSLIDEH,						//	水平带状滑动
//	EFFECT_BANDSLIDEV,						//	垂直带状滑动
//	EFFECT_BLINDSH,							//	水平百叶
//	EFFECT_BLINDSV ,						//	垂直百叶
//	EFFECT_SEPCIALBLINDSH= 15, 				//  不等高水平百叶
//	EFFECT_SEPCIALBLINDSV,					//  不等宽垂直百叶
//	EFFECT_SCANLEFT,						//	向左扫描
//	EFFECT_SCANRIGHT,						//	向右扫描
//	EFFECT_SCANUP,							//	向上扫描
//	EFFECT_SCANBOTTOM = 20,					//	向下扫描
//	EFFECT_INERTIASLIDELEFT,				//  向左惯性滑动
//  EFFECT_INERTIASLIDERIGHT,				//  向右惯性滑动
//	EFFECT_INERTIASLIDEUP,					//  向上惯性滑动
//	EFFECT_INERTIASLIDEBOTTOM,				//  向下惯性滑动
//	EFFECT_FADE = 25,						//	淡出/淡入
//	EFFECT_ZOOM,							//	放大
//	EFFECT_SHRINK,							//	缩小
//	EFFECT_SWING,							//	摆出/摆入
//	EFFECT_FUNNEL,							//	漏斗
//	EFFECT_NOISE = 30,						//	噪声
//	EFFECT_CUBELEFT,						//	左立方体
//	EFFECT_CUBERIGHT,						//	右立方体
//	EFFECT_CUBEUP,							//	上立方体
//	EFFECT_CUBEBOTTOM ,						//	下立方体
//	EFFECT_GRADUALERASELEFT = 35,			//	向左渐变擦除
//	EFFECT_GRADUALERASERIGHT,				//	向右渐变擦除
//	EFFECT_GRADUALERASEUP,					//	向上渐变擦除
//	EFFECT_GRADUALERASEBOTTOM,				//	向下渐变擦除
//	EFFECT_PUSHLEFT ,						//	向左推挤
//	EFFECT_PUSHRIGHT = 40,					//	向右推挤
//	EFFECT_PUSHUP,							//	向上推挤
//	EFFECT_PUSHBOTTOM,						//	向下推挤
//	EFFECT_DISSOLVE,						//  溶解
//	EFFECT_CURTAIN,							//  幕
//	EFFECT_TRIANGULARPRISMRIGHT = 45,		//  右三棱柱
//	EFFECT_TRIANGULARPRISMLEFT,				//  左三棱柱
//	EFFECT_TRIANGULARPRISMUP,				//  上三棱柱
//	EFFECT_TRIANGULARPRISMBOTTOM,			//  下三棱柱
//	EFFECT_CUBEINSIDELEFT ,					//  左内立方体
//	EFFECT_CUBEINSIDERIGHT = 50,			//  右内立方体
//	EFFECT_CUBEINSIDEUP,					//  上内立方体
//	EFFECT_CUBEINSIDEBOTTOM,				//  下内立方体
//	EFFECT_SPATIALMOTIONLEFT,				//  空间左移	
//	EFFECT_SPATIALMOTIONRIGHT,				//  空间右移
//	EFFECT_SPATIALMOTIONUP= 55,				//  空间上移
//	EFFECT_SPATIALMOTIONBOTTOM,				//  空间下移
//	EFFECT_PATCHRANDOMFLIP,					//  分块翻转
//	EFFECT_SPATIALROTATELEFT,				//  空间向左旋转
//	EFFECT_SPATIALROTATERIGHT ,				//  空间向右旋转
//	EFFECT_SPATIALROTATEUP = 60,			//  空间向上旋转
//	EFFECT_SPATIALROTATEBOTTOM,				//  空间向下旋转
//	EFFECT_DOOR,							//  开/关门
//	EFFECT_WHIRLPOOL,						//	漩涡
//	EFFECT_SCATTER,							//	打散
//	EFFECT_COLORDADE= 65 ,					//	褪色
//	EFFECT_DAMPSLIDE_BOTTOMLEFT,			//  左下角阻尼滑动
//	EFFECT_DAMPSLIDE_BOTTOMRIGHT,			//  右下角阻尼滑动
//	EFFECT_DAMPSLIDE_TOPLEFT,				//  左上角阻尼滑动
//	EFFECT_DAMPSLIDE_TOPRIGHT,				//  右上角阻尼滑动
//	EFFECT_DAMPSLIDELEFT = 70,				//	向左阻尼滑动
//	EFFECT_DAMPSLIDERIGHT,					//	向右阻尼滑动
//	EFFECT_DAMPSLIDEUP,						//	向上阻尼滑动
//	EFFECT_DAMPSLIDEBOTTOM,					//	向下阻尼滑动
//	EFFECT_CLOCKWIPE,						//	环形擦除
//	EFFECT_PINWHEEL = 75,					//	风车
//	EFFECT_PAGEPEEL,						//	卷页
//	EFFECT_GAUSSBLUR,						//	模糊淡入/出
//	EFFECT_WAVE,							//	波浪化
//	EFFECT_ZOOMBLUR ,						//	拖尾
//  EFFECT_FOLDV = 80,						//  垂直折叠
//	EFFECT_FOLDH,							//	水平折叠
//	EFFECT_STACKLEFT,						//	向左堆积
//	EFFECT_STACKRIGHT,						//	向右堆积
//	EFFECT_STACKUP,							//	向上堆积
//	EFFECT_STACKBOTTOM = 85,				//	向下堆积
//};

class CWndEffect : public IUIEffect
{
public:
	CWndEffect();
	~CWndEffect();	
public:
	//IUIEffect
	BOOL AppendAnimation(AnimationParam &aParam);
	BOOL DependAnimation(WPARAM effectKey);
	BOOL ClearAllAnimation();
	BOOL Animation(IUIEffectCallBack *iDrawEffect,DWORD frameSpin);
	// 初始化支持的动画
	static DWORD InitSurportAnimationType(const char *& strAnimationType);
private:
	// 计算动画过程
	void ComputeAnimation(IUIEffectCallBack *iDrawEffect,DWORD timeElapse);
	// 清理执行结束的动画
	void CleanFinishedAnimation(IUIEffectCallBack *iDrawEffect);
private:
	std::vector<InternalAnimationParam>				m_animationContainer;				// 动画参数容器
	std::vector<InternalAnimationParam>::iterator	m_itAnimation;						// 枚举器
	static const char *								m_animation;						// 动画名称，以\0分割,\0\0结束
	ULONG_PTR m_gdiplusToken;															// gdiplus 
};
 
// 所有支持的动画
const char* CWndEffect::m_animation =  "向左翻转\0向右翻转\0向下翻转\0向上翻转\0马赛克\0向左滑动\0向右滑动\0向上滑动\0向下滑动\0水平带状滑动\0垂直带状滑动\0水平百叶\0垂直百叶\0不等高水平百叶\0不等宽垂直百叶\0向左擦除\0向右擦除\0向上擦除\0向下擦除\0向左惯性滑动\0向右惯性滑动\0向上惯性滑动\0向下惯性滑动\0淡出/淡入\0放大\0缩小\0摆出/摆入\0漏斗\0噪声\0左立方体\0右立方体\0上立方体\0下立方体\0向左渐变擦除\0向右渐变擦除\0向上渐变擦除\0向下渐变擦除\0向左推挤\0向右推挤\0向上推挤\0向下推挤\0溶解\0幕\0右三棱柱\0左三棱柱\0上三棱柱\0下三棱柱\0左内立方体\0右内立方体\0上内立方体\0下内立方体\0空间左移\0空间右移\0空间上移\0空间下移\0分块翻转\0空间向左旋转\0空间向右旋转\0空间向上旋转\0空间向下旋转\0开/关门\0漩涡\0打散\0褪色\0左下角阻尼滑动\0右下角阻尼滑动\0左上角阻尼滑动\0右上角阻尼滑动\0向左阻尼滑动\0向右阻尼滑动\0向上阻尼滑动\0向下阻尼滑动\0环形擦除\0风车\0卷页\0模糊淡入/出\0水波\0拖尾\0垂直折叠\0水平折叠\0\0";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CAlphaBlendImpl : public IImageProcess
{
	// alpha 混合
	BOOL AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha = 255, int interpolationMode = 0);
	// 图片缩放
	BOOL ImageScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode = 0);
};