#include "StdAfx.h"
#include "ImageProcess.h"
#include <math.h>
//#include <mmintrin.h>
//#include <xmmintrin.h>
					
/*
mmintrin.h           MMX
xmmintrin.h          SSE
emmintrin.h          SSE2
pmmintrin.h          SSE3
tmmintrin.h          SSSE3
intrin.h             SSE4A
smmintrin.h          SSE4.1
nmmintrin.h          SSE4.2

*/
#pragma warning(disable : 4244)

BOOL CWarp::WarpImage(BYTE* pData,RECT &rcSrc,WarpRect &rcDes,RECT &rcBound,BYTE* imgDes,Color color/* = Color::Transparent*/,BOOL bFill/* = TRUE*/)
{
	CWarpMatrix matrix;
	if(!matrix.InitMatrix(rcDes,rcSrc))
	{
		return false;
	}

	
	int desWidth = rcBound.right - rcBound.left;
	int desHeight =  rcBound.bottom - rcBound.top;
	int desStride = desWidth*4;

	int srcWidth = rcSrc.right - rcSrc.left+1;
	int srcHeight = rcSrc.bottom - rcSrc.top+1;
	int srcStride = srcWidth*4;

	RECT rcWarpBounder = rcDes.GetBoundRect().ToRECT();
	RECT rcScan = {max(rcBound.left,rcWarpBounder.left),
		max(rcBound.top,rcWarpBounder.top),
		min(rcBound.right-1,rcWarpBounder.right),
		min(rcBound.bottom-1,rcWarpBounder.bottom)
	};

	int x,y;

	if (bFill)
	{
		CMemery::SetPixels(imgDes,desWidth,desHeight,color.GetValue());
	}
	
	__declspec(align(16)) real Vec[4] = {0.0f,0.0f,0.0f,1.0f};

	real *mtx = (real*)(void*)&matrix.m_FastWarpMatrix,*pVec = Vec;

	for (int row = rcScan.top; row <= rcScan.bottom; row++)
	{
		BYTE *pDes = imgDes + (row-rcBound.top)*desStride+(rcScan.left-rcBound.left)*4;
		for (int col = rcScan.left; col <= rcScan.right; col++)
		{
			__asm
			{
				mov esi, [pVec]
				fild col
				fstp [esi]						
				fild row
				fstp [esi+4]					


				mov ecx, esi;
				mov edx, mtx;

				movss xmm0, [ecx];
				shufps xmm0, xmm0, 0; 

				movss xmm1, [ecx+4];
				mulps xmm0, [edx];
				shufps xmm1, xmm1, 0;

				movaps xmm2, [edx+48];
				mulps xmm1, [edx+16];
				addps xmm0, xmm2;
				addps xmm0, xmm1;

				movaps xmm3, xmm0
				shufps xmm3,xmm3,0xaa					
				divps  xmm0,xmm3				

				movlps [ecx], xmm0

				fld [esi]
				fistp x
				fld [esi+4]
				fistp y

				mov eax, x						
				cmp eax, 0						
				jl PixelEnd						
				cmp eax, srcWidth				
				jge  PixelEnd					
				mov ecx, y						
				cmp ecx, 0						
				jl PixelEnd						
				cmp ecx, srcHeight				
				jge PixelEnd					
												
				imul ecx,srcStride				
				add ecx,[pData]					
				lea esi,[ecx+eax*4]				
				mov edi, pDes					
				movsd
PixelEnd:

			}

			pDes+=4;

		}
	}

	return TRUE;
}

BOOL CWarp::WarpImage(BYTE*pData,int srcDataWidth,int srcDataHeight, RECT &rcSrc,WarpRect &rcDes,RECT &rcBound,BYTE* imgDes,Color color/* = Color::Transparent*/,BOOL bFill/* = TRUE*/)
{
	CWarpMatrix matrix;
	if(!matrix.InitMatrix(rcDes,rcSrc))
	{
		return false;
	}

	int desWidth = rcBound.right - rcBound.left;
	int desHeight =  rcBound.bottom - rcBound.top;
	int desStride = desWidth*4;
	int srcStride = srcDataWidth*4;

	RECT rcWarpBounder = rcDes.GetBoundRect().ToRECT();
	RECT rcScan = {max(rcBound.left,rcWarpBounder.left),
		max(rcBound.top,rcWarpBounder.top),
		min(rcBound.right-1,rcWarpBounder.right),
		min(rcBound.bottom-1,rcWarpBounder.bottom)
	};

	int x,y;

	if (bFill)
	{
		CMemery::SetPixels(imgDes,desWidth,desHeight,color.GetValue());
	}

	__declspec(align(16)) real Vec[4] = {0.0f,0.0f,0.0f,1.0f};

	real *mtx = (real*)(void*)&matrix.m_FastWarpMatrix,*pVec = Vec;

	for (int row = rcScan.top; row <= rcScan.bottom; row++)
	{
		BYTE *pDes = imgDes + (row-rcBound.top)*desStride+(rcScan.left-rcBound.left)*4;
		for (int col = rcScan.left; col <= rcScan.right; col++)
		{
			__asm
			{
				mov esi, [pVec]
				fild col
				fstp [esi]						
				fild row
				fstp [esi+4]					


				mov ecx, esi;
				mov edx, mtx;

				movss xmm0, [ecx];
				shufps xmm0, xmm0, 0; 

				movss xmm1, [ecx+4];
				mulps xmm0, [edx];
				shufps xmm1, xmm1, 0;

				movaps xmm2, [edx+48];
				mulps xmm1, [edx+16];
				addps xmm0, xmm2;
				addps xmm0, xmm1;

				movaps xmm3, xmm0
				shufps xmm3,xmm3,0xaa			
				divps  xmm0,xmm3				

				movlps [ecx], xmm0

				fld [esi]
				fistp x
				fld [esi+4]
				fistp y

				mov	esi, [rcSrc]
				mov eax, x						
				cmp eax, [esi]					
				jl PixelEnd						
				cmp eax, [esi+8]				
				jg  PixelEnd					
				mov ecx, y						
				cmp ecx, [esi+4]				
				jl PixelEnd						
				cmp ecx, [esi+12]				
				jg PixelEnd						
				imul ecx,srcStride				
				add ecx,[pData]					
				lea esi,[ecx+eax*4]				
				mov edi, pDes					
				movsd
PixelEnd:

			}

			pDes+=4;

		}
	}

	return TRUE;
}

BOOL CWarp::WarpImage(BYTE *pData, int width, int height, BYTE* &imgDes, real param1/* = 0*/)
{
	int SrcStride = width * 4;
	//不缩放
	real scale = 1.0f;

	BYTE *pixelsDes = (BYTE*)imgDes;

	//int antialias = 1;
	real halfWidth = width/2.0f;
	real halfHeight = height/2.0f;
	real stepWidth = 2.0f/width;
	real stepHeight = 2.0f/height;
	real xnew,ynew,x,y = -1;
	int i2,j2;

	for (int row = 0; row < height; row++)
	{
		x = -1;
		for (int col = 0; col < width; col++)
		{			
			Evaluate(x,y,xnew,ynew,param1);
			
			++xnew *= halfWidth;
			++ynew *= halfHeight;
		
			REAL2INT(xnew,i2);
			REAL2INT(ynew,j2);

			if (i2 >= 0 && i2 < width && j2 >= 0 && j2 < height) 
			{
				memcpy(pixelsDes,pData+j2 * SrcStride + i2*4,4);
			}
			
			pixelsDes += 4;
			x += stepWidth;
			//x /= scale;	
		}

		y += stepHeight;
		//y /= scale;
	}

	return TRUE;
}

void CWarp::Evaluate(real x, real y, real &xnew, real &ynew, real param1)
{
	real phi,radius,radius2;

	__asm
	{
		fld [y]
		fld [x]
		fpatan
		fstp [phi]			//phi

		fld [x]
		fld st
		fmul				//x^2

		fld [y]
		fld st
		fmul				//y^2
		fadd				
		fst [radius2]		//radius2

		fmul [param1]		
		fld1
		fsubrp st(1),st		//st=1-param1 * radius2

		fld1
		fsub [param1]		//st = 1-parm1 st(1) = 1-param1 * radius2
		fdiv				//radius = st

		fld st
		fmul [x]
		fstp [x]
		fmul [y]

		fld st
		fmul
		fld [x]
		fld st
		fmul
		fadd

		fsqrt
		fst [radius]	//st= radius

		fdivr [param1]
		fadd  [phi]		//st = param1

		fsincos			// st = cos st(1) = sin
		fmul  [radius]
		fstp  [phi]		//xnew
		fmul  [radius]
		fstp  [radius2]	//ynew
	}

	xnew = phi;
	ynew = radius2;
}

void CMemery::SetPixels(BYTE *Data,int width, int height, DWORD iValue)
{
	__asm
	{
		mov		eax, iValue
		mov     edi, Data
		mov		ecx, width
		imul	ecx, height
		rep     stosd
	}
}

void CMemery::SetPixels(BYTE *Data,int stride, int left, int top, int right, int bottom, DWORD iValue)
{
	int strideTemp, widTrans = right - left;

	__asm
	{		
		mov		eax, top
		mov     edx, eax
		imul 	eax, stride
		add  	eax, [Data] 
		mov		edi, eax
		mov		eax, left
		lea     edi, [edi+eax*4]	
		mov     eax, widTrans
		shl     eax, 2
		mov     ecx, stride
		sub     ecx, eax
		mov     strideTemp, ecx
		mov		eax, iValue

		jmp     rRowNowJudge 
rRowNowLoop:
		inc     edx
rRowNowJudge:
		cmp     bottom, edx
		jle     rRowNowLoopEnd 

		mov  	ecx, widTrans
		rep  	stosd

		add     edi, strideTemp

		jmp  	rRowNowLoop
rRowNowLoopEnd:

	}
}

void CMemery::CopyPixels(BYTE*DataDes,int strideDes,int leftDes,int topDes,int rightDes,int bottomDes,BYTE *DataSrc,int strideSrc,int leftSrc,int topSrc)
{
	int strideTemp, num = rightDes - leftDes;

	__asm
	{	
		mov		edx, topDes
		mov     eax, edx

		mov		edi, [DataDes]
		mov		ecx, strideDes
		imul	ecx, edx
		add		edi, ecx
		mov		ecx, leftDes
		lea		edi, [edi+ecx*4]	

		mov		esi, [DataSrc]
		mov		ecx, edx
		add		ecx, topSrc
		sub     ecx, topDes
		imul    ecx, strideSrc
		add		esi, ecx
		mov		ecx, leftSrc
		lea		esi, [esi+ecx*4]

		mov     ecx, num
		shl     ecx, 2
		mov     edx, strideSrc
		sub     edx, ecx
		mov     strideTemp, edx
		mov     edx, strideDes
		sub     edx, ecx

		jmp     rRowNowJudge 
rRowNowLoop:
		inc     eax
rRowNowJudge:
		cmp     eax, bottomDes
		jge     rRowNowLoopEnd

		mov     ecx, num
		rep		movsd

		add		esi, strideTemp
		add		edi, edx

		jmp      rRowNowLoop
rRowNowLoopEnd:

	}
}

BOOL CBlinds::BlindsH(BYTE *Data,int width, int height,int iBlindHeight,int iBlindNum,int offsetDistance,DWORD color/* = 0*/)
{
	int stride = width*4;
	int rowNow;
	__asm
	{
		mov		eax, color

		xor     edx, edx		
		mov     rowNow, edx 
		jmp     rRowNowJudge 
rRowNowLoop:
		mov     edx,rowNow 
		add     edx,1 
		mov     rowNow,edx 
rRowNowJudge:
		mov     edx, iBlindNum
		cmp     rowNow, edx 
		jge     rRowNowLoopEnd 

		mov		edx, rowNow
		imul	edx, iBlindHeight
		add		edx, offsetDistance
		cmp		height, edx
		jle		rRowNowLoopEnd

		imul 	edx, stride
		add  	edx, [Data] 
		mov		edi, edx

		mov  	ecx, width
		rep  	stosd

		jmp  	rRowNowLoop
rRowNowLoopEnd:
	}

	return TRUE;
}

BOOL CBlinds::BlindsV(BYTE *Data,int width, int height,int iBlindWidth,int iBlindNum,int offsetDistance,DWORD color/* = 0*/)
{
	int stride = width*4-4;
	int colNow;

	__asm
	{
		mov		eax, color

		mov     edx, 0		
		mov     colNow, edx 
		jmp     rColNowJudge 
rColNowLoop:
		mov     edx,colNow 
		add     edx,1 
		mov     colNow,edx 
rColNowJudge:
		mov     edx, iBlindNum
		cmp     colNow, edx 
		jge     rColNowLoopEnd 

		mov		edx, iBlindWidth
		imul	edx, colNow
		add		edx, offsetDistance
		cmp		width, edx
		jle		rColNowLoopEnd

		lea     edi, [edx*4]
		add  	edi, [Data] 
		mov		ecx, height
setData:
		stosd
		add     edi, stride
		loop    setData

		jmp     rColNowLoop
rColNowLoopEnd:
	}

	return TRUE;
}

BOOL CBlinds::BlindsH(BYTE *DataDes,BYTE *DataSrc,int width, int height,int iBlindHeight,int iBlindNum,int offsetDistance)
{
	int bytes = 4;
	int rowNow,stride = width*bytes;

	__asm
	{
		mov     eax, 0		
		mov     rowNow, eax 
		jmp     rRowNowJudge 
rRowNowLoop:
		mov     eax,rowNow 
		add     eax,1 
		mov     rowNow,eax 
rRowNowJudge:
		mov     eax, iBlindNum
		cmp     rowNow, eax 
		jge     rRowNowLoopEnd

		mov		eax, rowNow
		imul	eax, iBlindHeight
		add		eax, offsetDistance
		cmp		height, eax
		jle		rRowNowLoopEnd

		imul    eax, stride
		mov     edi, eax
		add     edi, DataDes
		mov     esi, eax
		add     esi, DataSrc

		mov     ecx, width
//		cld
		rep		movsd

		jmp      rRowNowLoop
rRowNowLoopEnd:

	}
	return TRUE;
}

BOOL CBlinds::BlindsV(BYTE *DataDes,BYTE *DataSrc,int width, int height,int iBlindWidth,int iBlindNum,int offsetDistance)
{
	int bytes = 4;
	int  colNow,stride = width*bytes-4;

	__asm
	{
		mov     eax, 0		
		mov     colNow, eax 
		jmp     rColNowJudge 
rColNowLoop:
		mov     eax,colNow 
		add     eax,1 
		mov     colNow,eax 
rColNowJudge:
		mov     eax, iBlindNum
		cmp     colNow, eax 
		jge     rColNowLoopEnd 

		mov		eax, iBlindWidth
		imul	eax, colNow
		add		eax, offsetDistance
		cmp		width, eax
		jle		rColNowLoop;

		imul    eax, bytes
		mov		esi, eax
		add     esi, [DataSrc] 
		mov		edi, eax
		add     edi, [DataDes]

		mov     ecx, height
copyData:		
//		cld
		movsd	
		add     esi, stride
		add     edi, stride
		loop	copyData

		jmp     rColNowLoop
rColNowLoopEnd:
	}

	return TRUE;
}

void CAlpha::SetAlpha(BYTE *Data,int width, int height,real alphaChange)
{
	// bgra 
	int bytes = 4;
	int stride = width*bytes;
	int alphaInt = (int)(alphaChange*(1<<16));

	__asm
	{	
		mov		eax, width
		imul	eax, height
		mov		esi, [Data]

		xor		edx, edx
		jmp		pixelLoopJudge
pixelLoop:
		inc		edx
pixelLoopJudge:
		cmp		edx, eax
		jge		pixelLoopEnd

		movzx	ecx,  [esi+3]
		imul	ecx,  alphaInt
		shr		ecx,  16
		mov		[esi+3], cl
		lea		esi,	[esi+4]
		jmp		pixelLoop
pixelLoopEnd:
	}
}

void CAlpha::SetAlphaMMX(BYTE *Data,int width, int height,BYTE alphaNew)
{
	UINT32 num = height*width;
	UINT32 loopNum8 = num >> 3;
	UINT32 loopNum4 = (num - (loopNum8 << 3)) >> 2;
	UINT32 loopNum2 = (num -(loopNum8 << 3) - (loopNum4 << 2)) >> 1;
	UINT32 loopNum1 = num & 1;

	const UINT64 alphaMask = ((UINT64)alphaNew) << 56 | (UINT64)alphaNew << 24;

	__asm
	{
		xor  ecx, ecx
		cmp  ecx, loopNum8
		je   pixel4
		movq mm0, alphaMask
		mov  esi, Data
		mov ecx, loopNum8
pixelLoop8:
		movq  mm1, [esi]
		movq  mm2, [esi+8]
		movq  mm3, [esi+16]
		movq  mm4, [esi+24]

		por   mm1,mm0
		por   mm2,mm0
		por   mm3,mm0
		por   mm4,mm0

		movq  [esi],mm1
		movq  [esi+8],mm2
		movq  [esi+16],mm3
		movq  [esi+24],mm4

		lea   esi, [esi+32]
		loop pixelLoop8

pixel4:
		cmp ecx, loopNum4
		je   pixel2
		mov ecx, loopNum4
pixelLoop4:
		movq  mm1, [esi]
		movq  mm2, [esi+8]

		por   mm1,mm0
		por   mm2,mm0

		movq  [esi],mm1
		movq  [esi+8],mm2

		lea   esi, [esi+16]
		loop pixelLoop4

		lea   esi, [esi+32]
		loop pixelLoop4
pixel2:
		cmp ecx, loopNum2
		je   Piexl1
		mov ecx, loopNum2
pixelLoop2:
		movq  mm1, [esi]
		por   mm1,mm0
		movq  [esi],mm1
		lea   esi, [esi+8]
		loop pixelLoop2

Piexl1:
		cmp  ecx,loopNum1
		je   PiexlEnd

		mov  eax,255
		mov  [esi+3],al
PiexlEnd:	
		emms
	}
}

void CAlpha::SetAlpha(BYTE *Data,int stride, int left ,int top, int right, int bottom, real alphaChange)
{
	int alphaInt = (int)(alphaChange*(1<<16));
	int strideTemp, width = right - left;

	__asm
	{	
		mov		eax, top
		mov     edx, eax
		imul    edx, stride
		add     edx, [Data] 
		mov     ecx,left
		lea     esi,[edx+ecx*4]
		mov     ecx,width
		shl     ecx,2
		mov     edx, stride
		sub		edx, ecx
		mov     strideTemp, edx


		jmp     cRowJudge 
cRowLoop:
		add     eax,1 
cRowJudge:
		cmp     bottom, eax 
		jle     cRowLoopEnd 

		mov		ecx,width
rColLoop:
		movzx	edx,  [esi+3]
		imul	edx,  alphaInt
		shr		edx,  16
		mov		[esi+3], dl
		add		esi,  4
		loop	rColLoop
		add     esi,  strideTemp
		jmp     cRowLoop
cRowLoopEnd:

	}
}

BOOL CWater::Create(int iWidth,int iHeight)
{
	// Create our height fields and clear it
	m_iParticleAmplitudeField1 = new int[iWidth*iHeight];
	m_iParticleAmplitudeField2 = new int[iWidth*iHeight];
	memset(m_iParticleAmplitudeField1,0,(iWidth*iHeight)*sizeof(int));
	memset(m_iParticleAmplitudeField2,0,(iWidth*iHeight)*sizeof(int));

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	// Set our page to 0
	m_iHpage = 0;

	m_density = 6;

	return TRUE;
}

BOOL CWater::Destory()
{
	if(m_iParticleAmplitudeField1)
	{
		delete []m_iParticleAmplitudeField1;
		m_iParticleAmplitudeField1 = NULL;
	}

	if(m_iParticleAmplitudeField2)
	{
		delete []m_iParticleAmplitudeField2;
		m_iParticleAmplitudeField2 = NULL;
	}

	return TRUE;
}

BOOL CWater::SetDensity(int idensity)
{
	m_density = idensity;

	return TRUE;
}

BOOL CWater::ResetAmplitude()
{
	memset(m_iParticleAmplitudeField1,0,(m_iWidth*m_iHeight)*sizeof(int));
	memset(m_iParticleAmplitudeField2,0,(m_iWidth*m_iHeight)*sizeof(int));

	return TRUE;
}

BOOL CWater::AmplitudeBlob(int x, int y, int radius, int amplitude)
{
	int *newptr;
	int *oldptr;

	// 设置指针
	if(m_iHpage == 0)
	{
		newptr = m_iParticleAmplitudeField1;
		oldptr = m_iParticleAmplitudeField2;
	}
	else
	{
		newptr = m_iParticleAmplitudeField2;
		oldptr = m_iParticleAmplitudeField1;
	}

	// 随机
	if(x < 0) 
	{
		x = 1 + radius + rand()%(m_iWidth - 2 * radius - 1);
	}

	if(y < 0)
	{
		y = 1 + radius + rand()%(m_iHeight - 2 * radius - 1);
	}

	int left,top,right,bottom;
	left = top = -radius; 
	right = bottom = radius;

	// 边界
	if(x - radius < 1) 
	{
		left -= (x - radius - 1);
	}

	if(y - radius < 1)
	{
		top  -= (y - radius - 1);
	}

	if(x + radius > m_iWidth - 1)
	{
		right -= (x + radius - m_iWidth + 1);
	}

	if(y + radius > m_iHeight - 1) 
	{
		bottom-= (y + radius - m_iHeight + 1);
	}

	int rquad = radius * radius;
	for(int cy = top; cy < bottom; cy++)
	{
		int cyq = cy*cy;
		for(int cx = left; cx < right; cx++)
		{
			int cxq = cx*cx;
			if(cxq + cyq < rquad)
			{
				newptr[m_iWidth * (cy + y) + (cx + x)] += (int)(amplitude*(1.0f-(real)(cxq + cyq)/rquad));
			}
		}
	}

	return TRUE;
}

BOOL CWater::CalcWaterAmplitude(int npage, int density)
{
	int *newptr;
	int *oldptr;

	// 设置指针
	if(npage == 0)
	{
		newptr = m_iParticleAmplitudeField1;
		oldptr = m_iParticleAmplitudeField2;
	}
	else
	{
		newptr = m_iParticleAmplitudeField2;
		oldptr = m_iParticleAmplitudeField1;
	}

	int count = m_iWidth + 1;
	for (int y = (m_iHeight-1)*m_iWidth; count < y; count += 2)
	{

		for (int x = count+m_iWidth-2; count < x; count++)
		{
			int newh = ((oldptr[count + m_iWidth]
			+ oldptr[count - m_iWidth]
			+ oldptr[count + 1]
			+ oldptr[count - 1]
			+ oldptr[count - m_iWidth - 1]
			+ oldptr[count - m_iWidth + 1]
			+ oldptr[count + m_iWidth - 1]
			+ oldptr[count + m_iWidth + 1]
			) >> 2 ) - newptr[count];

			newptr[count] =  newh - (newh >> density);
		}
	}

	return TRUE;
}

BOOL CWater::WaterImage(BYTE*pSrcImage,BYTE*pTargetImage)
{
	DrawWaterWithLight(m_iHpage,pSrcImage,pTargetImage);

	CalcWaterAmplitude(m_iHpage,m_density);

	m_iHpage ^= 1;

	return TRUE;
}

BOOL CWater::DrawWaterWithLight(int page,BYTE* pSrcImage,BYTE* pTargetImage)
{
	int *ptr = m_iParticleAmplitudeField1;

	if(m_iHpage == 1)
	{
		ptr = m_iParticleAmplitudeField2;
	}

	int offsetPos = 0;

	int lBreak = m_iWidth*m_iHeight;
	for (int y = (m_iHeight-1)*m_iWidth; offsetPos < y;)
	{
		for (int x = offsetPos + m_iWidth; offsetPos < x; offsetPos++)
		{
			int deltax = ptr[offsetPos] - ptr[offsetPos+1];
			int deltay = ptr[offsetPos] - ptr[offsetPos+m_iWidth];

			int lIndex = offsetPos + m_iWidth*(deltay>>3) + (deltax>>3);

			if(lIndex < lBreak && lIndex > 0)
			{
				__asm
				{
					mov  esi, pSrcImage
					mov  eax, lIndex
					lea  esi, [esi+eax*4]

					mov  edi, pTargetImage
					mov  eax, offsetPos
					lea  edi, [edi+eax*4]

					mov  eax, deltax
					cmp  eax, 0						
					jl   Minus						
					movd  mm1, eax					
					punpcklbw mm1, mm1			
					movd  mm2, eax
					movd mm0, [esi]					
					punpcklbw mm1, mm2					
					psubusb mm0,mm1					
					jmp WarpEnd						
Minus:
					neg   eax
					movd  mm1, eax
					punpcklbw mm1, mm1
					movd mm0, [esi]
					punpcklbw mm1, mm1
					paddusb mm0,mm1
WarpEnd:
					movd  [edi],mm0
				}
			}
		}
	}

	__asm emms

	return TRUE;
}

#define PROCESSBOUNDER		1
real CScale::m_sinxDivxTable[513] = { 0 };
BOOL CScale::m_paramInited = FALSE;
real CScale::SinXDivX(real x,real sharpenCoefficient) 
{	
	real a = sharpenCoefficient; 

	x=abs(x);
	real x2=x*x;
	real x3=x2*x;

	if (x <= 1)
	{
		return (a+2)*x3 - (a+3)*x2 + 1;
	}
	else if (x <= 2) 
	{
		return a*x3 - (5*a)*x2 + (8*a)*x - (4*a);
	}
	else
	{
		return 0;
	}
} 

BOOL CScale::InitParam(real sharpenCoefficient)
{
	if(!m_paramInited)
	{
		sharpenCoefficient = abs(sharpenCoefficient);
		sharpenCoefficient = min(1.0f,sharpenCoefficient);
		real coefficient = -2*sharpenCoefficient;
		for (int i = 0; i <= 512; i++)
		{
			m_sinxDivxTable[i]=SinXDivX(i/256.0f,coefficient);
		}

		m_paramInited = TRUE;
	}

	return TRUE;
}

BOOL CScale::Scale(BYTE *DataDes,int desWidth, int desHeight,BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, real sharpenCoefficient)
{
	InitParam(sharpenCoefficient);

	//bounder
	real left = (real)desWidth / srcWidth;
	real top = (real)desHeight / srcHeight;
	real bottom = top * (srcHeight-3);
	real right = left * (srcWidth-3);

	left = ceil(left);
	top = ceil(top);
	right = floor(right);
	bottom = floor(bottom);

	real ratioH = (real)srcHeight / desHeight;
	real rationW = (real)srcWidth / desWidth;

	// common
	for (int row = top; row <= bottom; row++)
	{
		real srcy = row * ratioH + srcTop;
		for (int col = left; col <= right; col++)
		{
			real srcx = col* rationW + srcLeft;
			ThreeOrder(DataSrc,srcStride,srcx,srcy,DataDes+row*desWidth*4+col*4);			
		}
	}

#ifdef PROCESSBOUNDER  //处理边界
	// up bounder
	for (int row = 0; row < top; row++)
	{
		real srcy = row * ratioH + srcTop;
		for (int col = 0; col < desWidth; col++)
		{
			real srcx = col* rationW + srcLeft;
			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
		}
	}

	// bottom bounder
	for (int row = bottom+1; row < desHeight; row++)
	{
		real srcy = row * ratioH + srcTop;
		for (int col = 0; col < desWidth; col++)
		{
			real srcx = col* rationW + srcLeft;
			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
		}
	}


	for (int row = top; row <= bottom; row++)
	{
		// left bounder
		real srcy = row * ratioH + srcTop;
		for (int col = 0; col < left; col++)
		{
			real srcx = col* rationW + srcLeft;
			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
		}
		// right bounder
		for (int col = right+1; col < desWidth; col++)
		{
			real srcx = col* rationW + srcLeft;
			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
		}
	}

#endif

	return TRUE;
}

void CScale::GetBorderColor(const BYTE *DataSrc, const int srcStride, int row, int col, real* borderColor)
{
	const BYTE* ptr = DataSrc+srcStride*row+(col<<2);
	borderColor[0] = ptr[0];
	borderColor[1] = ptr[1];
	borderColor[2] = ptr[2];
	borderColor[3] = ptr[3];
}

void CScale::ThreeOrder(const BYTE *DataSrc,const int srcStride, const real fx,const real fy,BYTE *resultColor)
{
	int x0 = (int)(fx);    //floor
	int y0 = (int)(fy);	   //floor
	real fu = (fx - x0)*256;
	real fv = (fy - y0)*256;

	int fu_8, fv_8 ;
	REAL2INT(fu,fu_8);
	REAL2INT(fv,fv_8);
	__declspec(align(16)) real afu[4] = {m_sinxDivxTable[fu_8+256],m_sinxDivxTable[fu_8],m_sinxDivxTable[256-fu_8],m_sinxDivxTable[512-fu_8]};
	__declspec(align(16)) real afv[4] = {m_sinxDivxTable[256+fv_8],m_sinxDivxTable[fv_8],m_sinxDivxTable[256-fv_8],m_sinxDivxTable[512-fv_8]};

	// pixel data
	__declspec(align(16)) real pixel[4][16];

	x0--;
	y0--;

	for(int i = 0; i < 4; i++)
	{
		const BYTE *pSrc = DataSrc + (y0+i)*srcStride + (x0<<2);

		pixel[i][0] = pSrc[0];
		pixel[i][1] = pSrc[1];
		pixel[i][2] = pSrc[2];
		pixel[i][3] = pSrc[3];

		pixel[i][4] = pSrc[4];
		pixel[i][5] = pSrc[5];
		pixel[i][6] = pSrc[6];
		pixel[i][7] = pSrc[7];

		pixel[i][8] = pSrc[8];
		pixel[i][9] = pSrc[9];
		pixel[i][10] = pSrc[10];
		pixel[i][11] = pSrc[11];

		pixel[i][12] = pSrc[12];
		pixel[i][13] = pSrc[13];
		pixel[i][14] = pSrc[14];
		pixel[i][15] = pSrc[15];
	}

	__asm
	{
		pxor	xmm5,xmm5
		lea ecx, [afu]

		xor     eax,eax	
		xor     esi,esi
		jmp     cJudge 
cLoop:
		add     eax,64 
		add     esi,4
cJudge:
		cmp     eax,256
		jge     cLoopEnd 

		lea edx, [pixel+eax] 

		movss xmm0, [ecx]
		shufps xmm0, xmm0, 0; 

		movss xmm1, [ecx+4];
		mulps xmm0, [edx];
		shufps xmm1, xmm1, 0; 

		movss xmm2, [ecx+8];
		mulps xmm1, [edx+16];
		shufps xmm2, xmm2, 0; 

		movss xmm3, [ecx+12];
		mulps xmm2, [edx+32];
		shufps xmm3, xmm3, 0; 

		addps xmm0, xmm1;
		mulps xmm3, [edx+48];

		addps xmm0, xmm2;
		addps xmm0, xmm3; // xmm0 = result

		lea   edx, [afv+esi]
		movss xmm4,[edx]
		shufps xmm4, xmm4, 0; 
		mulps xmm0,xmm4
		addps xmm5,xmm0

		jmp     cLoop
cLoopEnd:
		movaps  [ecx],xmm5

		mov     edi,resultColor
		xor     eax,eax	
		jmp     cResJudge 
cResLoop:
		add     eax,1 
cResJudge:
		cmp     eax,4
		jge     cResEnd

		fld   [ecx+eax*4]
		fistp x0
		mov   edx,x0
		cmp   edx,0
		jle	  zero
		cmp   edx, 255
		jg	  over
		mov  [edi+eax],dl
		jmp cResLoop
zero:
		mov  [edi+eax],0
		jmp cResLoop
over:
		mov  [edi+eax],255
		jmp cResLoop
cResEnd:
	}
}

void CScale::ThreeOrderBorder(const BYTE *DataSrc,const int srcStride, int srcLeft, int srcTop,const int srcWidth, const int srcHeight,const real fx,const real fy,BYTE *resultColor)
{
	int x0 = (int)(fx);    //floor
	int y0 = (int)(fy);	   //floor
	real fu = (fx - x0)*256;
	real fv = (fy - y0)*256;

	int fu_8, fv_8 ;
	REAL2INT(fu,fu_8);
	REAL2INT(fv,fv_8);
	__declspec(align(16)) real afu[4] = {m_sinxDivxTable[fu_8+256],m_sinxDivxTable[fu_8],m_sinxDivxTable[256-fu_8],m_sinxDivxTable[512-fu_8]};
	__declspec(align(16)) real afv[4] = {m_sinxDivxTable[256+fv_8],m_sinxDivxTable[fv_8],m_sinxDivxTable[256-fv_8],m_sinxDivxTable[512-fv_8]};

	// pixel data
	__declspec(align(16)) real pixel[4][16];

	x0--;
	y0--;


	int srcRight = srcLeft + srcWidth - 1;
	int srcBottom = srcTop + srcHeight -1 ;
	for(int i = 0; i < 4; i++)
	{
		int row = y0+i < srcTop ? srcTop : (y0+i > srcBottom ? srcBottom : y0+i);

		GetBorderColor(DataSrc, srcStride, row, x0   < srcLeft ? srcLeft : (x0   > srcRight ? srcRight : x0),   &pixel[i][0]);
		GetBorderColor(DataSrc, srcStride, row, x0+1 < srcLeft ? srcLeft : (x0+1 > srcRight ? srcRight : x0+1), &pixel[i][4]);
		GetBorderColor(DataSrc, srcStride, row, x0+2 < srcLeft ? srcLeft : (x0+2 > srcRight ? srcRight : x0+2), &pixel[i][8]);
		GetBorderColor(DataSrc, srcStride, row, x0+3 < srcLeft ? srcLeft : (x0+3 > srcRight ? srcRight : x0+3), &pixel[i][12]);
	}

	__asm
	{
		pxor	xmm5,xmm5
		lea ecx, [afu]

		xor     eax,eax	
		xor     esi,esi
		jmp     cJudge 
cLoop:
		add     eax,64 
		add     esi,4
cJudge:
		cmp     eax,256
		jge     cLoopEnd 

		lea edx, [pixel+eax] 

		movss xmm0, [ecx]
		shufps xmm0, xmm0, 0; 

		movss xmm1, [ecx+4];
		mulps xmm0, [edx];
		shufps xmm1, xmm1, 0; 

		movss xmm2, [ecx+8];
		mulps xmm1, [edx+16];
		shufps xmm2, xmm2, 0; 

		movss xmm3, [ecx+12];
		mulps xmm2, [edx+32];
		shufps xmm3, xmm3, 0; 

		addps xmm0, xmm1;
		mulps xmm3, [edx+48];

		addps xmm0, xmm2;
		addps xmm0, xmm3; // xmm0 = result

		lea   edx, [afv+esi]
		movss xmm4,[edx]
		shufps xmm4, xmm4, 0; 
		mulps xmm0,xmm4
		addps xmm5,xmm0

		jmp     cLoop
cLoopEnd:
		movaps  [ecx],xmm5

		mov     edi,resultColor
		xor     eax,eax	
		jmp     cResJudge 
cResLoop:
		add     eax,1 
cResJudge:
		cmp     eax,4
		jge     cResEnd

		fld   [ecx+eax*4]
		fistp x0
		mov   edx,x0
		cmp   edx,0
		jle	  zero
		cmp   edx, 255
		jg	  over
		mov  [edi+eax],dl
		jmp cResLoop
zero:
		mov  [edi+eax],0
		jmp cResLoop
over:
		mov  [edi+eax],255
		jmp cResLoop
cResEnd:
	}
}

BOOL CScale::ImageScale(BYTE *DataDes,int desWidth, int desHeight,BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode/* = 0*/)
{
	static CScale s_instance;
	if(interpolationMode == 0)
	{
		return s_instance.Scale(DataDes, desWidth, desHeight, DataSrc, srcStride, srcLeft, srcTop, srcWidth, srcHeight);
	}
	else if(interpolationMode == 1)
	{
		return s_instance.BilinearScale(DataDes, desWidth, desHeight, DataSrc, srcStride, srcLeft, srcTop, srcWidth, srcHeight);
	}
	else if(interpolationMode == 2)
	{
		return s_instance.Scale(DataDes, desWidth, desHeight, DataSrc, srcStride, srcLeft, srcTop, srcWidth, srcHeight, 0.6f);
	}

	return FALSE;
}

BOOL CScale::Scale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight)
{
	int ratioH = (srcHeight<<16)/desHeight + 1;
	int ratioW = (srcWidth<<16)/desWidth + 1;

	__asm
	{	
		pxor mm0, mm0
		movd mm3,ratioW
		movd mm4,ratioH
		mov edi, [DataDes]
		xor eax, eax
		jmp rowJudge
rowLoop:
		inc eax
rowJudge:
		cmp	eax, desHeight
		jge	rowEnd

		movd edx, mm0
		shr edx, 16
		add edx, srcTop
		imul edx, srcStride
		add  edx, [DataSrc]
		mov esi, edx	

		pxor mm1, mm1
		mov ecx, desWidth
		jmp colJudge
colLoop:
		sub ecx, 2
colJudge:
		cmp ecx, 2
		jl	colEnd

		movd edx, mm1
		paddd mm1,mm3
		movd ebx, mm1
		shr ebx, 16
		add ebx, srcLeft
		shr edx, 16
		add edx, srcLeft
		movd mm2, [esi+edx*4]
		punpckldq mm2,[esi+ebx*4]
		movq [edi], mm2	
		add  edi, 8
		paddd mm1,mm3

		jmp colLoop
colEnd:
		paddd mm0, mm4
		test ecx, ecx
		jz rowLoop

		movd ebx, mm1
		shr ebx, 16
		add ebx, srcLeft
		movd mm2, [esi+ebx*4]
		movd [edi], mm2
		add  edi, 4

		jmp rowLoop
rowEnd:
		emms
	}
}

BOOL CScale::BilinearScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcleft, int srctop, int srcWidth, int srcHeight)
{
	//bounder
	real left = (real)desWidth / srcWidth;
	real top = (real)desHeight / srcHeight;
	real bottom = top * (srcHeight-1);
	real right = left * (srcWidth-1);

	left = ceil(left);
	top = ceil(top);
	right = floor(right);
	bottom = floor(bottom);

	real ratioH = (real)srcHeight / desHeight;
	real rationW = (real)srcWidth / desWidth;

	real test = 124.00567f;
	real mod = fmod(test,1.0f);

	// common
	for (int row = top; row <= bottom; row++)
	{
		real srcy = row * ratioH + srctop;
		for (int col = left; col <= right; col++)
		{
			real srcx = col* rationW + srcleft;
			ThreeOrder(DataSrc,srcStride,srcx,srcy,DataDes+row*desWidth*4+col*4);			
		}
	}

//#ifdef PROCESSBOUNDER  //处理边界
//	// up bounder
//	for (int row = 0; row < top; row++)
//	{
//		real srcy = row * ratioH + srcTop;
//		for (int col = 0; col < desWidth; col++)
//		{
//			real srcx = col* rationW + srcLeft;
//			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
//		}
//	}
//
//	// bottom bounder
//	for (int row = bottom+1; row < desHeight; row++)
//	{
//		real srcy = row * ratioH + srcTop;
//		for (int col = 0; col < desWidth; col++)
//		{
//			real srcx = col* rationW + srcLeft;
//			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
//		}
//	}
//
//
//	for (int row = top; row <= bottom; row++)
//	{
//		// left bounder
//		real srcy = row * ratioH + srcTop;
//		for (int col = 0; col < left; col++)
//		{
//			real srcx = col* rationW + srcLeft;
//			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
//		}
//		// right bounder
//		for (int col = right+1; col < desWidth; col++)
//		{
//			real srcx = col* rationW + srcLeft;
//			ThreeOrderBorder(DataSrc,srcStride,srcLeft,srcTop,srcWidth,srcHeight,srcx,srcy,DataDes+row*desWidth*4+col*4);			
//		}
//	}
//
//#endif

	return TRUE;
}

BOOL CAlphaBlend::AlphaBlend(BYTE *desData, BYTE *srcData, int width, int height, BYTE sourceAlpha/* = 255*/)
{
	if(sourceAlpha == 0)
	{
		return TRUE;
	}

	if (width < 1 || height < 1) 
	{
		return FALSE;
	}

	if(sourceAlpha != 255)
	{
		CAlpha::SetAlpha(srcData, width, height, (real)sourceAlpha/255);
	}

	UINT32 num = width*height; 


	const unsigned __int64 alphaBitMask = 0xffffffffffff;

	__asm 
	{
		mov			edi,desData
		mov			esi,srcData	
		pxor		mm6,mm6
		pxor		mm7,mm7
		mov			eax,num

scan_loop:
		cmp         eax,2
		jl			loopEnd

		cmp    		[esi+3],0
		je			loopOne
		cmp			[esi+3],255
		je			copyOne
		jmp			sseLoop
copyOne:
		movsd
		dec			eax
		jmp			scan_loop
loopOne:
		add			esi,4
		add			edi,4
		dec			eax
		jmp			scan_loop

sseLoop:    
		movq		mm4,[esi]			
		movq		mm5,[edi]			

		movq		mm0,mm4				
		movq		mm1,mm5				
		punpcklbw	mm0,mm6				
		punpcklbw	mm1,mm7				
		pshufw		mm2,mm0,0ffh		
		pand		mm2,alphaBitMask
		psubw		mm0,mm1	
		movq		mm3,mm1				
		pmullw		mm0,mm2					
		psllw		mm3,8
		paddw		mm0,mm3				
		psrlw		mm0,8				

		punpckhbw	mm5,mm7				
		punpckhbw	mm4,mm6				
		movq		mm3,mm5				
		pshufw		mm2,mm4,0ffh
		psubw		mm4,mm5		
		pand		mm2,alphaBitMask	
		pmullw		mm4,mm2	
		psllw		mm3,8							
		paddw		mm4,mm3				
		psrlw		mm4,8				
		packuswb	mm0,mm4				
		movq		[edi],mm0

		add         esi,8
		add			edi,8
		sub			eax,2

		jmp			scan_loop
loopEnd:
		cmp         eax, 0
		je			LastPixelEnd

		cmp    		[esi+3],0
		je			LastPixelEnd
		cmp			[esi+3],255
		je			copyLastOne
		jmp			sseLoopLast
copyLastOne:
		movsd
		jmp			LastPixelEnd
sseLoopLast:

		movd		mm0,[esi]
		movd		mm1,[edi]

		punpcklbw	mm0,mm6			
		punpcklbw	mm1,mm7			
		pshufw		mm2,mm0,0ffh	
		pand		mm2,alphaBitMask
		movq		mm3,mm1			
		psubw		mm0,mm1			
		psllw		mm3,8			
		pmullw		mm0,mm2			
		paddw		mm0,mm3			
		psrlw		mm0,8			
		packuswb	mm0,mm0
		movd        [edi],mm0

LastPixelEnd:
		emms
	}


	return TRUE;
}

BOOL CAlphaBlend::AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, BYTE sourceAlpha/* = 255*/)
{	
	if(sourceAlpha == 0)
	{
		return TRUE;
	}

	if (desWidth < 1 || desHeight < 1) 
	{
		return FALSE;
	}

	if(sourceAlpha != 255)
	{
		CAlpha::SetAlpha(srcData, desWidth, desHeight, (real)sourceAlpha/255);
	}

	const unsigned __int64 alphaBitMask = 0xffffffffffff;

	__asm 
	{
		mov			edi,desData
		mov         eax,desStride
		imul		eax,desTop
		add         edi,eax
		mov			eax,desLeft
		lea			edi,[edi+eax*4]
		mov			esi,srcData
		mov         eax,srcStride
		imul		eax,srcTop
		add         esi,eax
		mov			eax,srcLeft
		lea			esi,[esi+eax*4]

		mov			edx,desHeight
		pxor		mm6,mm6
		pxor		mm7,mm7
		xor			eax,eax
scan_loop:
		cmp         eax, desWidth
		jge			pix_loopEnd

		cmp    		[esi+3], 0
		je			loopOne
		cmp			[esi+3], 255
		je			copyOne
		mov			ecx, desWidth
		sub			ecx, eax
		shr			ecx, 1
		test		ecx, ecx
		jnz			pix_loop
		jmp			one_pixel

copyOne:
		movsd		
		add			eax, 1
		jmp			scan_loop

loopOne:
		add			eax, 1
		add			esi, 4
		add			edi, 4
		jmp			scan_loop

pix_loop:
		movq		mm4,[esi]		
		movq		mm5,[edi]	

		movq		mm0,mm4				
		movq		mm1,mm5				
		punpcklbw	mm0,mm6				
		punpcklbw	mm1,mm7				
		pshufw		mm2,mm0,0ffh		
		pand		mm2,alphaBitMask
		psubw		mm0,mm1	
		movq		mm3,mm1				
		pmullw		mm0,mm2					
		psllw		mm3,8
		paddw		mm0,mm3				
		psrlw		mm0,8				

		punpckhbw	mm5,mm7				
		punpckhbw	mm4,mm6				
		movq		mm3,mm5				
		pshufw		mm2,mm4,0ffh
		psubw		mm4,mm5		
		pand		mm2,alphaBitMask	
		pmullw		mm4,mm2	
		psllw		mm3,8							
		paddw		mm4,mm3				
		psrlw		mm4,8				
		packuswb	mm0,mm4				
		movq		[edi],mm0	

		add			eax, 2
		add			esi, 8
		add			edi, 8
		jmp			scan_loop

one_pixel:

		movd		mm0,[esi]
		movd		mm1,[edi]

		punpcklbw	mm0,mm6			
		punpcklbw	mm1,mm7			
		pshufw		mm2,mm0,0ffh	
		pand		mm2,alphaBitMask
		movq		mm3,mm1			
		psubw		mm0,mm1			
		psllw		mm3,8			
		pmullw		mm0,mm2			
		paddw		mm0,mm3			
		psrlw		mm0,8			
		packuswb	mm0,mm0
		movd        [edi],mm0
		add			esi, 4
		add			edi, 4
pix_loopEnd:
		add			esi, srcStride
		add			edi, desStride
		mov			ecx, desWidth
		shl			ecx, 2
		sub			esi, ecx
		sub			edi, ecx
		xor			eax, eax
		dec			edx
		jnz			scan_loop

		emms
	}

	return TRUE;
}

BOOL CAlphaBlend::AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha/* = 255*/, int interpolationMode/* = 0*/)
{
	if(srcWidth == desWidth || desHeight == srcHeight)
	{
		// 不缩放
		if(srcWidth == (srcStride >> 2) && desWidth == (desStride >> 2))
		{
			// 两张图大小相等
			return AlphaBlend(desData, srcData, srcWidth, srcHeight, sourceAlpha);
		}

		return AlphaBlend(desData, desStride, desLeft, desTop, desWidth, desHeight, srcData, srcStride, srcLeft, srcTop, sourceAlpha);
	}

	BYTE *pDataScale = new BYTE[desWidth * desHeight * 4];
	if(pDataScale == NULL)
	{
		return FALSE;
	}

	if(!CScale::ImageScale(pDataScale, desWidth, desHeight, srcData, srcStride, srcLeft, srcTop, srcWidth, srcHeight, interpolationMode))
	{
		delete [] pDataScale;
		return FALSE;
	}

	if(desWidth != (desStride >> 2))
	{
		if(!AlphaBlend(desData, desStride, desLeft, desTop, desWidth, desHeight, pDataScale, desWidth*4, 0, 0, sourceAlpha))
		{
			delete [] pDataScale;
			return FALSE;
		}
	}
	else
	{
		if(!AlphaBlend(desData, pDataScale, desWidth, desHeight, sourceAlpha))
		{
			delete [] pDataScale;
			return FALSE;
		}
	}

	delete [] pDataScale;
	return TRUE;
}

void CGaussBlur::ImageGaussiabBlur(BYTE *Data,int width, int height,int radius, int *weights)
{		
	// bgra 
	int bytes = 4;
	int stride = width*bytes;
	int templateLength = 2 * radius + 1;
	DWORD num = max(width,height)*bytes;
	BYTE *ptrTemp = new BYTE[num];
	memset(ptrTemp, 0, num*sizeof(BYTE));	

	int r,g,b,a,row,col;

	__asm
	{
			mov     eax, 0				
			mov     col, eax 
			jmp     cColJudge 
cColLoop:
			mov     eax,col 
			add     eax,1 
			mov     col,eax 
cColJudge:
			mov     eax, width
			cmp     col, eax 
			jge     cColLoopEnd 

			mov     eax, 0			
			mov     row, eax 
			jmp     cPreAlphaJudge 
cPreAlphaLoop:
			mov     edx,row 
			add     edx,1 
			mov     row,edx 
cPreAlphaJudge:
			mov     eax, height
			cmp     row, eax 
			jge     cPreAlphaLoopEnd 

			mov         eax, row
			imul        eax, stride
			add         eax, [Data] 
			mov         ecx, col
			imul        ecx, bytes 
			add         eax, ecx 

			mov			ebx, eax   

			movzx       eax, [eax] 
			movzx       edx, [ebx+3] 
			imul        eax, edx 
			cdq              
			mov         ecx, 0FFh 
			idiv        ecx 
			mov         [ebx],al 

			mov			eax,  ebx
			movzx       eax, [eax+1] 
			movzx       edx, [ebx+3] 
			imul        eax, edx 
			cdq              
			mov         ecx, 0FFh 
			idiv        ecx 
			mov         [ebx+1], al 

			mov			eax,  ebx
			movzx       eax, [eax+2] 
			movzx       edx, [ebx+3] 
			imul        eax,edx 
			cdq              
			mov         ecx,0FFh 
			idiv        ecx 
			mov         byte ptr [ebx+2],al 

			jmp         cPreAlphaLoop
cPreAlphaLoopEnd:

			mov     eax, 0					
			mov     row, eax 
			jmp     cRowJudge 
cRowLoop:
			mov     edx,row 
			add     edx,1 
			mov     row,edx 
cRowJudge:
			mov     eax, height
			cmp     row, eax 
			jge     cRowEnd 

			xor     ebx, ebx				
			mov     r,   ebx   
			mov     g,	 ebx 
			mov     b,   ebx   
			mov     a,   ebx 		

			mov     eax, row 
			imul    eax, stride 
			mov     edx, [Data]
			add     edx, eax 
			mov     eax, col 
			imul    eax, bytes 
			add     edx, eax				

			mov     esi, edx				
			mov		edi, weights			

			mov     ecx, templateLength		

			mov     eax, radius				
			cmp		eax, row
			jge		cOffset1
			mov		eax, height
			sub		eax, radius
			dec		eax
			cmp		eax, row
			jl		cOffset2				
			jmp		cOffset3
cOffset1:						
			add		ecx, row                   
			sub		ecx, eax
			sub     eax, row				
			imul    eax, 4					
			add		edi, eax
			mov		eax, stride					// 数据偏移量
			imul	eax, row
			sub		esi, eax	
			jmp		cblurLoop
cOffset2:
			add		ecx, eax				
			sub		ecx, row
cOffset3:
			mov		eax, radius				
			imul	eax, stride
			sub		esi, eax
cblurLoop:								
			movzx   eax, [esi]				
			movzx   edx, [esi+1]			
			imul    eax, [edi]				
			imul    edx, [edi]				
			add     b,   eax				
			add     g,   edx			
			movzx   eax, [esi+2]		
			movzx   edx, [esi+3]			
			imul    eax, [edi]				
			imul    edx, [edi]				
			add     r,   eax				
			add     a,   edx			
			add     edi, 4				
			add     esi, stride			
			loop    cblurLoop			


			mov     eax, row 
			imul    eax, bytes 
			add     eax, [ptrTemp]		
			mov     edi, eax				

			mov		eax, b
			shr		eax, 16
			mov		[edi], al
			mov		eax, g
			shr		eax, 16
			mov		[edi+1], al
			mov		eax, r
			shr		eax, 16
			mov		[edi+2], al
			mov		eax, a
			shr		eax, 16
			mov		[edi+3], al

			jmp     cRowLoop 
cRowEnd:


			mov         eax, Data
			mov         ecx, col
			imul        ecx, bytes 
			add         eax, ecx 
			mov			edi, eax			

			mov         esi, ptrTemp		

			mov     eax, 0
			mov     row, eax 
			jmp     cRowCopyJudge 
cRowCopyLoop:
			mov     edx,row 
			add     edx,1 
			mov     row,edx 
cRowCopyJudge:
			mov     eax, height
			cmp     row, eax 
			jge     cRowCopyLoopEnd 

			mov         dl, [esi]		
			mov         [edi], dl 
			mov         dl, [esi+1] 
			mov         [edi+1], dl 
			mov         dl, [esi+2] 
			mov         [edi+2], dl 
			mov         dl, [esi+3] 
			mov         [edi+3], dl 

			mov         eax, esi		
			add         eax, bytes
			mov         esi, eax 			
			mov         eax, edi		
			add         eax, stride 
			mov         edi, eax 

			jmp         cRowCopyLoop
cRowCopyLoopEnd:
		jmp		cColLoop

cColLoopEnd:

		//// row blur

			mov     eax, 0						
			mov     row, eax 
			jmp     rRowJudge 
rRowLoop:
			mov     eax,row 
			add     eax,1 
			mov     row,eax 
rRowJudge:
			mov     eax, height
			cmp     row, eax 
			jge     rRowLoopEnd 

			mov     eax, 0					
			mov     col, eax 
			jmp     rColJudge 
rColLoop:
			mov     eax, col 
			add     eax, 1 
			mov     col, eax 
rColJudge:
			mov     eax, width 
			cmp     col, eax 
			jge     rColEnd 

			xor     ebx, ebx					
			mov     r,   ebx   
			mov     g,	 ebx 
			mov     b,   ebx   
			mov     a,   ebx 		

			mov     eax, row 
			imul    eax, stride 
			mov     edx, [Data]
			add     edx, eax 
			mov     eax, col 
			imul    eax, bytes 
			add     edx, eax					

			mov     esi, edx					
			mov		edi, weights				

			mov     ecx, templateLength			

			mov     eax, radius					
			cmp		eax, col
			jge		rOffset1
			mov		eax, width
			sub		eax, radius
			dec		eax
			cmp		eax, col
			jl		rOffset2					
			jmp		rOffset3
rOffset1:						
			add		ecx, col                   
			sub		ecx, eax
			sub     eax, col			
			imul    eax, 4				
			add		edi, eax
			mov		eax, bytes			
			imul	eax, col
			sub		esi, eax	
			jmp		rblurLoop
rOffset2:
			add		ecx, eax				
			sub		ecx, col
rOffset3:
			mov		eax, radius				
			imul	eax, bytes
			sub		esi, eax
rblurLoop:								
			movzx   eax, [esi]				
			movzx   edx, [esi+1]			
			imul    eax, [edi]				
			imul    edx, [edi]				
			add     b,   eax				
			add     g,   edx			
			movzx   eax, [esi+2]		
			movzx   edx, [esi+3]			
			imul    eax, [edi]				
			imul    edx, [edi]				
			add     r,   eax				
			add     a,   edx			
			add     edi, 4						
			add     esi, bytes					
			loop    rblurLoop					

			mov     eax, col 
			imul    eax, bytes 
			add     eax, [ptrTemp]				
			mov     edi, eax					

			mov		eax, b						
			shr		eax, 16
			mov		[edi], al
			mov		eax, g
			shr		eax, 16
			mov		[edi+1], al
			mov		eax, r
			shr		eax, 16
			mov		[edi+2], al
			mov		eax, a
			shr		eax, 16
			mov		[edi+3], al

			jmp     rColLoop 
rColEnd:


			mov         esi, ptrTemp					

			mov         eax, row						
			imul        eax, stride
			add         eax, Data
			mov         edi, eax 

			mov     eax, 0								
			mov     col, eax 
			jmp     rSepAlphaJudge 
rSepAlphaLoop:
			mov     eax,col 
			add     eax,1 
			mov     col,eax 
rSepAlphaJudge:
			mov     eax, width
			cmp     col, eax 
			jge     rSepAlphaEnd 


			movzx       ecx, [esi+3]					
			test        ecx, ecx 
			je          rAlphaNullOrOpacity 
			cmp         ecx, 0FFh 
			je          rAlphaNullOrOpacity 


			movzx       eax, [esi]						
			imul        eax, 0FFh 
			cdq              
			idiv        ecx 
			mov         [edi],al 


			movzx       eax, [esi+1]					
			imul        eax, 0FFh 
			cdq              
			idiv        ecx 
			mov         [edi+1],al 

			movzx       eax, [esi+2]					
			imul        eax, 0FFh 
			cdq              
			idiv        ecx 
			mov         [edi+2], al 
			jmp			rAlphaCopy

rAlphaNullOrOpacity:
			mov         dl, [esi]						
			mov         [edi], dl 
			mov         dl, [esi+1] 
			mov         [edi+1], dl 
			mov         dl, [esi+2] 
			mov         [edi+2], dl 
rAlphaCopy:										
			mov         dl, [esi+3]						
			mov         [edi+3], dl 

			mov         eax, esi						
			add         eax, bytes
			mov         esi, eax 

			mov         eax, edi						
			add         eax, bytes 
			mov         edi, eax 

			jmp        rSepAlphaLoop

rSepAlphaEnd:
		jmp			rRowLoop
rRowLoopEnd:
	}

	delete []ptrTemp;
	delete []weights;
} 

void CGaussBlur::InitWeights(double Q,int radius,int **weights)
{
	double fx = 0;
	int templateLength = radius*2 + 1;
	real *fweights = new real[templateLength];

	for(int i = 1; i <=  radius; i++)
	{
		fx = i / Q;   
		fweights[radius + i] = (real)exp(-fx * fx / 2);   
		fweights[radius - i] = fweights[radius + i];
	}

	fweights[radius] = 1.0;

	fx = 0.0;   
	for(int i = 0; i < templateLength; i++)
	{
		fx = fx + fweights[i];  
	}

	*weights = new int[templateLength];

	// 归一化，整数化
	for(int i = 0; i < templateLength; i++)
	{
		(*weights)[i] = int(fweights[i] / fx * 65536.0 + 0.5);  
	}

	delete []fweights;
}

BOOL CTraileBlur::TraileBlur(BYTE *DataDes, BYTE *DataSrc, int width, int height, int lengthBlur)
{
	if(lengthBlur <= 0)
	{
		memcpy(DataDes, DataSrc, width*height*4);
		return TRUE;
	}
	// bgra 
	int bytes = 4;
	int stride = width*bytes;
	int nCenX = width/2, nCenY = height/2;
	real coefficient = 0.02f;
	int row,col,blurIndex,xx,yy,nSumB, nSumG, nSumR, nSumA;

	real *varResult = new real[lengthBlur];
	int sizefloat = sizeof(real);
	__asm
	{	
			mov		esi, [varResult]
			mov     eax, 0		
			mov     blurIndex, eax 
			jmp     blurCofficientJudge 
blurCofficientLoop:
			mov     eax,blurIndex 
			add     eax,1 
			mov     blurIndex,eax 
blurCofficientJudge:
			mov     eax, lengthBlur
			cmp     blurIndex, eax 
			jge     blurCofficientLoopEnd 

			fild [blurIndex]     
			fld [coefficient]
			fmul

			fld1
			fadd	
			fstp [esi]
	
			add esi , sizefloat
			jmp		blurCofficientLoop
blurCofficientLoopEnd:

			mov     eax, 0					
			mov     row, eax 
			jmp     cRowJudge 
cRowLoop:
			mov     edx,row 
			add     edx,1 
			mov     row,edx 
cRowJudge:
			mov     eax, height
			cmp     row, eax 
			jge     cRowLoopEnd 

			// col blur
			mov     eax, 0				    
			mov     col, eax 
			jmp     cColJudge 
cColLoop:
			mov     eax,col 
			add     eax,1 
			mov     col,eax 
cColJudge:
			mov     eax, width
			cmp     col, eax 
			jge     cColLoopEnd 

			xor eax, eax
			mov nSumB, eax
			mov nSumG, eax
			mov nSumR, eax
			mov nSumA, eax

			//  blur loop

			mov     esi, [varResult]		
			mov     blurIndex, eax 
			jmp     blurJudge 
blurLoop:
			mov     eax,blurIndex 
			add     eax,1 
			mov     blurIndex,eax 
			add     esi, sizefloat
blurJudge:
			mov     eax, lengthBlur
			cmp     blurIndex, eax 
			jge     blurLoopEnd 

			fld  [esi]		
			mov eax, col
			sub eax, nCenX
			mov [xx],eax
			fild [xx]			
			fmul
			FRNDINT				
			fistp xx

			mov eax, nCenX
			add eax, xx
			cmp eax, 0
			jl  blurLoopEnd
			cmp eax, width
			jge blurLoopEnd
			mov [xx], eax
			fld [esi]

			mov eax, row
			sub eax, nCenY
			mov [yy],eax
			fild [yy]			

			fmul
			FRNDINT				
			fistp yy

			mov eax, nCenY
			add eax, yy
			cmp eax, 0
			jl  blurLoopEnd
			cmp eax, height
			jge blurLoopEnd
			mov [yy], eax

			mov         eax, yy
			imul        eax, stride
			add         eax, [DataSrc] 
			mov         ecx, xx
			imul        ecx, bytes 
			add         eax, ecx 

			mov			ecx, eax       

			movzx       eax, [eax]
			add         eax, nSumB
			mov			nSumB,eax

			movzx       eax, [ecx+1]
			add         eax, nSumG
			mov			nSumG,eax

			movzx       eax, [ecx+2] 
			add         eax, nSumR
			mov			nSumR,eax

			movzx       eax, [ecx+3] 
			add         eax, nSumA
			mov			nSumA,eax

			jmp     blurLoop
blurLoopEnd:

			mov         eax, row
			imul        eax, stride
			add         eax, [DataDes] 
			mov         ecx, col
			imul        ecx, bytes 
			add         eax, ecx 

			mov			esi, eax      

			mov			ebx,blurIndex
			mov			eax, nSumA	
			cdq 
			idiv        ebx
			mov			[esi+3], al
			mov			ecx, eax
			imul		ecx, blurIndex

			test        al, al 
			je          cColLoop
	
			cdq              
			mov         eax,nSumB 
			imul		eax, 0xff
			idiv        ecx
			mov			[esi], al

			cdq              
			mov         eax,nSumG 
			imul		eax, 0xff
			idiv        ecx
			mov			[esi+1], al

			cdq              
			mov         eax,nSumR 
			imul		eax, 0xff
			idiv        ecx
			mov			[esi+2], al

			jmp     cColLoop
cColLoopEnd:
			jmp     cRowLoop
cRowLoopEnd:
	}

	delete []varResult;
	return TRUE;
}