#pragma once

#include <math.h>
#include "Graph_Def.h"

namespace Graph
{
	// 参数定义(宏)
	const real M_PI = 3.141592654f;
	
	// 数值操作(宏)
	#define ABS(x) ((x)>0?(x):-(x))
	#define ToInt(value)	((int)(value))
	#define ToLong(value)	((long)(value))	
	#define SIGN(x) ((x)>0?(1):-(1))

	// 把real x 转换成int y 
	#define  REAL2INT(x,y)		__asm fld [x] __asm fistp y 

	// 把int x 转换成real y 
	#define  INT2REAL(x,y)		__asm fild [x] __asm fstp y 
	
	// 数值比较(宏)
	#define max(a,b)    (((a) > (b)) ? (a) : (b))
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
	#define between(a,b,c) (((a)<=(b)) && ((b)<(c)))
	#define isnear(a,b,d)	(((b)-(d)<(a)) && ((a)<(b)+(d)))

	// 数组操作(宏)
	#define ARRAY_BOUND_OF(array)	(sizeof(array)/sizeof(array[0]))

	// 数值比较
	inline bool D_Equal(real d1, real d2, real dPrec = dConst_MinPrecision)
	{
		return (ABS(d1-d2)<dPrec);
	}

	inline bool D_LessThan(real d1, real d2, real dPrec = dConst_MinPrecision)
	{
		return d1 < d2 - dPrec;
	}

	inline bool D_GreaterThan(real d1, real d2, real dPrec = dConst_MinPrecision)
	{
		return d1 > d2 + dPrec;
	}

	inline bool D_LessEqual(real d1, real d2, real dPrec = dConst_MinPrecision)
	{
		return d1 <= d2 + dPrec;
	}

	inline bool D_GreaterEqual(real d1, real d2, real dPrec = dConst_MinPrecision)
	{
		return d1 >= d2 - dPrec;
	}

	inline int D_Compare(real d1, real d2, real dPrec = dConst_MinPrecision)
	{
		if(d1 < d2 - dPrec)
		{
			return -1;
		}
		else if(d1 > d2 + dPrec)
		{
			return 1;
		}

		return 0;
	}

	inline real D_Min(real d1, real d2, real dPrec=dConst_MinPrecision)
	{
		return (d1 < d2-dPrec) ? d1 : d2;
	}

	inline real D_Max(real d1, real d2, real dPrec=dConst_MinPrecision)
	{
		return (d1 > d2+dPrec) ? d1 : d2;
	}

	inline real D_Mid(real d, real inf, real sup, real dPrec=dConst_MinPrecision)
	{
		assert(inf<=sup);
		if (d < inf-dPrec)		return inf;
		if (d > sup+dPrec)		return sup;
		return d;
	}

	//向+∞舍入
	inline real D_Ceil(real d)
	{
		return ceil(d);
	}
	//向0舍入
	inline real D_Fix(real d)
	{
		return (d >= 0) ? floor(d) : ceil(d);
	}
	//向-∞舍入
	inline real D_Floor(real d)
	{
		return floor(d);
	}
	//四舍五入
	inline real D_Round(real d)
	{
		real df = floor(d);
		real dc = ceil(d);

		if (d >= 0)
			return (dc-d <= d-df) ? dc : df;

		return (d-df <= dc-d) ? df : dc;
	}

	inline int ToNearestInt(float value)
	{
		return ToInt(value + (value >= 0 ? 0.5f : -0.5f));
	}

	inline int ToNearestInt(double value)
	{
		return ToInt(value + (value >= 0 ? 0.5f : -0.5f));
	}

	inline bool IsOddInt(int value)
	{
		assert(value >= 0);
		return ((value % 2) == 0) ? false : true;
	}

	inline bool BooleanEq(bool b1, bool b2) 
	{
		return (b1 && b2) || (!b1 && !b2); 
	}

	inline real Power(real x, real y)
	{
		return (x >= 0.0f)? pow(x, y) : -pow(-x, y);
	}

	inline void swap(real &a,real &b)
	{
		real temp=a;
		a=b;
		b=temp;
	}

	// Point&Size结构
	#pragma pack(push,1)
	template<class T>
	struct DPOINTT
	{
		T x;
		T y;
	};
	template<class T>
	struct DSIZET
	{
		T cx;
		T cy;
	};
	#pragma pack(pop)

	
	inline SIZE POINTtoSIZE(POINT point)
	{
		SIZE size = { point.x, point.y };
		return size;
	}


	// CdPoint&CdSize声明
	template<class T>
	class CdPointT;
	template<class T>
	class CdSizeT;

	// CdPoint类
	#pragma pack(push,1)
	template<class T>
	class CdPointT : public DPOINTT<T>
	{
		typedef DPOINTT<T> baseClass;
		typedef CdPointT<T> thisClass;
	public:
		CdPointT()
		{
			x = 0;
			y = 0;
		}
		CdPointT(const baseClass& point)
		{
			x = point.x;
			y = point.y;
		}
		CdPointT(const POINT& point)
		{
			x = (real)point.x;
			y = (real)point.y;
		}
		CdPointT(T _x, T _y)
		{
			x = _x;
			y = _y;
		}
		~CdPointT()
		{
		}

	public:
		// convert to int type POINT
		POINT ToPOINT() const
		{
			POINT point = {ToNearestInt(x), ToNearestInt(y)};
			return point;
		}

	public:
		// convert to real type vector
		DSIZET<T> ToDSIZE() const
		{
			DSIZET<T> size = {x, y};
			return size;
		}
		// convert to int type vector
		SIZE ToSIZE() const
		{
			SIZE size = {ToNearestInt(x), ToNearestInt(y)};
			return size;
		}

	public:
		// offset the point
		inline void Offset(T cx, T cy)
		{
			x += cx;
			y += cy;
		}
		inline void Offset(CdSizeT<T> szOffset)
		{
			Offset(szOffset.cx, szOffset.cy);
		}

	public:
		// get distance to (0,0)
		inline T GetLength() const
		{
			return sqrt(x*x + y*y);
		}
		// get point polar angle (rad)
		//   returns a value in the range -pi to pi radians
		T GetPolarAngle() const
		{
			if(IsOrigin())
			{
				assert(false);
				return 0;
			}
			return atan2(y,x);
		}

	public:
		// point calculations
		inline const thisClass& operator =(const thisClass& point)
		{
			x = point.x;
			y = point.y;
			return point;
		}
		void operator +=(const CdSizeT<T>& size);
		void operator -=(const CdSizeT<T>& size);
		void operator *=(T alpha)
		{
			x *= alpha;
			y *= alpha;
		}
		void operator /=(T alpha)
		{
			if(alpha == 0)
			{
				assert(false);
				return;
			}
			x /= alpha;
			y /= alpha;
		}
		inline const thisClass& operator +() const
		{
			return *this;
		}
		inline thisClass operator -() const
		{
			return thisClass(-x, -y);
		}
		inline thisClass operator *(T dReal)
		{
			x *= dReal;
			y *= dReal;
			return *this;
		}
		inline thisClass operator +(const thisClass& dPoint)
		{
			x += dPoint.x;
			y += dPoint.y;
			return *this;
		}

	public:
		// set point to (0,0)
		inline void SetToOrigin()
		{
			x = y = 0;
		}

	public:
		// adjust is the origin point (0,0)
		inline bool IsOrigin() const
		{
			return (x == 0) && (y == 0);
		}
		// adjust whether both co-ordinate abs value is not larger than delta
		inline bool IsSmall(T alpha) const
		{
			return (fabs(x) <= alpha) && (fabs(y) <= alpha);
		}
	};
	#pragma pack(pop)

	// CdSize类
	#pragma pack(push,1)
	template<class T>
	class CdSizeT : public DSIZET<T>
	{
		typedef DSIZET<T> baseClass;
		typedef CdSizeT<T> thisClass;
	public:
		CdSizeT()
		{
			cx = cy = 0;
		}
		CdSizeT(const baseClass& size)
		{
			cx = size.cx;
			cy = size.cy;
		}
		CdSizeT(const SIZE& size)
		{
			cx = (T)size.cx;
			cy = (T)size.cy;
		}
		CdSizeT(T _cx, T _cy)
		{
			cx = _cx;
			cy = _cy;
		}
		~CdSizeT()
		{
		}

	public:
		// convert to int type SIZE
		SIZE ToSIZE() const
		{
			SIZE size = {ToNearestInt(cx), ToNearestInt(cy)};
			return size;
		}

	public:
		// convert to real type point
		DPOINTT<T> ToDPOINT() const
		{
			DPOINTT<T> point = {cx, cy};
			return point;
		}
		// convert to int type point
		POINT ToPOINT() const
		{
			POINT point = {ToNearestInt(cx), ToNearestInt(cy)};
			return point;
		}

	public:
		// get vector length
		inline T GetLength() const
		{
			return sqrt(cx*cx + cy*cy);
		}
		// get vector polar angle (rad)
		//   returns a value in the range -pi to pi radians
		inline T GetPolarAngle() const
		{
			if(IsNull())
			{
				assert(false);
				return 0;
			}
			return atan2(cy, cx);
		}

	public:
		// vector calculations
		inline const thisClass& operator = (const thisClass& size)
		{
			cx = size.cx;
			cy = size.cy;
			return size;
		}
		inline void operator += (const thisClass& size)
		{
			cx += size.cx;
			cy += size.cy;
		}
		inline void operator -= (const thisClass& size)
		{
			cx -= size.cx;
			cy -= size.cy;
		}
		inline T operator * (const thisClass& size)
		{
			return cx * size.cx + cy * size.cy;
		}	
		inline void operator *= (T alpha)
		{
			cx *= alpha;
			cy *= alpha;
		}
		void operator /= (T alpha)
		{
			if(alpha == 0)
			{
				assert(false);
				return;
			}
			cx /= alpha;
			cy /= alpha;
		}
		inline const thisClass& operator + () const
		{
			return *this;
		}
		inline thisClass operator - () const
		{
			return thisClass(-cx, -cy);
		}


	public:
		// set to (0,0)
		inline void SetToNull()
		{
			cx = cy = 0;
		}

	public:
		// adjust is null vector (0,0)
		inline bool IsNull() const
		{
			return (cx == 0) && (cy == 0);
		}
		// adjust whether both co-ordinate abs value is less than delta
		inline bool IsSmall(T delta) const
		{
			return (fabs(cx) <= delta) && (fabs(cy) <= delta);
		}
	};
	#pragma pack(pop)

	typedef DPOINTT<float> DPOINTFLOAT;
	typedef DPOINTT<real> DPOINTDOUBLE;
	typedef CdPointT<float> CdPointFloat;
	typedef CdPointT<real> CdPointDoulbe;

	typedef DSIZET<float> DSIZEFLOAT;
	typedef DSIZET<real> DSIZEDOUBLE;
	typedef CdSizeT<float> CdSizeFloat;
	typedef CdSizeT<real> CdSizeDoulbe;

#ifdef USING_DOUBLEFLOAT
	typedef DPOINTDOUBLE DPOINT;
	typedef CdPointDoulbe CdPoint;
	typedef DSIZEDOUBLE DSIZE;
	typedef CdSizeDoulbe CdSize;
#else
	typedef DPOINTFLOAT DPOINT;
	typedef CdPointFloat CdPoint;
	typedef DSIZEFLOAT DSIZE;
	typedef CdSizeFloat CdSize;
#endif //USING_DOUBLEFLOAT
	
	// CdPoint&CdSize运算
	#define OUGHT(f)           ((void)0)
	inline void CdPoint::operator += (const CdSize& size)
	{
		x += size.cx;
		y += size.cy;
	}
	
	inline void CdPoint::operator -= (const CdSize& size)
	{
		x -= size.cx;
		y -= size.cy;
	}

	inline CdSize operator - (const CdPoint& point1, const CdPoint& point2)
	{
		return CdSize(point1.x - point2.x, point1.y - point2.y);
	}

	inline bool operator == (const CdPoint& point1, const CdPoint& point2)
	{
		return (point1 - point2).IsNull();
	}

	inline bool operator != (const CdPoint& point1, const CdPoint& point2)
	{
		return !operator==(point1, point2);
	}

	inline CdSize operator + (const CdSize& size1, const CdSize& size2)
	{
		return CdSize(size1.cx + size2.cx, size1.cy + size2.cy);
	}

	inline CdSize operator - (const CdSize& size1, const CdSize& size2)
	{
		return CdSize(size1.cx - size2.cx, size1.cy - size2.cy);
	}

	inline real operator * (const CdSize& size1, const CdSize& size2)
	{
		return size1.cx*size2.cx + size1.cy*size2.cy;
	}
	
	inline real operator ^ (const CdSize& size1, const CdSize& size2)
	{
		return size1.cx*size2.cy - size1.cy*size2.cx;
	}

	inline bool operator == (const CdSize& size1, const CdSize& size2)
	{
		return (size1 - size2).IsNull();
	}
	
	inline bool operator != (const CdSize& size1, const CdSize& size2)
	{
		return !operator==(size1, size2);
	}

	inline CdPoint operator + (const CdPoint& point, const CdSize& size)
	{
		return CdPoint(point.x + size.cx, point.y + size.cy);
	}

	inline CdPoint operator - (const CdPoint& point, const CdSize& size)
	{
		return CdPoint(point.x - size.cx, point.y - size.cy);
	}

	inline CdPoint& DPOINTtoCdPoint(DPOINT& point)
	{
		return (CdPoint&)point;
	}

	inline const CdPoint& DPOINTtoCdPoint(const DPOINT& point)
	{
		return (const CdPoint&)point;
	}
	
	inline bool PointEqual(const CdPoint& pt1, const CdPoint& pt2, real dPrec=dConst_MinPrecision)
	{
		if(D_Equal(pt1.x, pt2.x, dPrec) && D_Equal(pt1.y, pt2.y, dPrec))
		{
			return true;
		}
		
		return false;
	}

	inline CdPoint MirrorPoint(const CdPoint& point, const CdPoint& pointRef)
	{
		return pointRef - (point - pointRef);
	}

	inline real PointDist(const CdPoint& point1, const CdPoint& point2)
	{
		if(point1.x == point2.x)
		{
			return ABS(point1.y - point2.y);
		}
		else if(point1.y == point2.y)
		{
			return ABS(point1.x - point2.x);
		}
		else
		{
			return sqrt((point1.x - point2.x) * (point1.x - point2.x) + 
				(point1.y - point2.y) * (point1.y - point2.y));
		}
	}

	inline real AbsPointDist(const CdPoint& point1, const CdPoint& point2)
	{
		CdSize sz = point2 - point1;
		return max(fabs(sz.cx), fabs(sz.cy));
	}

	inline CdSize ScaleSize(const CdSize& size, real alpha)
	{ 
		return CdSize(size.cx * alpha, size.cy * alpha);
	}

	inline CdSize ScaleSize(const CdSize& size, real alphaX, real alphaY)
	{
		return CdSize(size.cx * alphaX, size.cy * alphaY);
	}

	inline CdSize operator * (const CdSize& size, real alpha)
	{ 
		return CdSize(size.cx * alpha, size.cy * alpha);
	}

	inline CdSize operator / (const CdSize& size, real alpha)
	{ 
		assert(alpha != 0.0f);
		return CdSize(size.cx / alpha, size.cy / alpha);
	}

	inline real operator / (const CdSize& vect, const CdSize& vectRef)
	{
		if (vectRef.IsNull())
		{
			OUGHT(false);
			return 0;
		}
		
		return (vect * vectRef) / (vectRef * vectRef);
	}

	inline CdSize ProjectVector(const CdSize& vect, const CdSize& vectRef)
	{
		return vectRef * (vect / vectRef);
	}

	inline CdSize GetUnitVector(const CdSize& vector1)
	{
		real length = vector1.GetLength();
		if (length <= 0)
		{
			OUGHT(false);
			return CdSize(0, 0);
		}

		return vector1 / length;
	}

	inline CdSize VectorWithLength(const CdSize& vectRef, real length)
	{
		return GetUnitVector(vectRef) * length;
	}

	inline CdSize VectorWithLength(const CdSize& vectRef, const CdSize& vLength)
	{
		return VectorWithLength(vectRef, vLength.GetLength());
	}

	inline real GetVectorProjectLambda(const CdSize& vectorIn, const CdSize& vectRef)
	{
		return vectorIn/vectRef;
	}

	inline bool IsDoubleEqual(real dNum1, real dNum2, real dDiff = 0.001f)
	{
		if(ABS(dNum1-dNum2) <= ABS(dDiff))
		{
			return true;
		}

		return false;
	}

	inline bool IsPointEqual(const DPOINT& pt1, const DPOINT& pt2, real dDiff = 0.001f)
	{
		if(IsDoubleEqual(pt1.x, pt2.x, dDiff)&&IsDoubleEqual(pt1.y, pt2.y, dDiff))
		{
			return true;
		}
		
		return false;
	}
	
	inline bool IsSizeEqual(const DSIZE& size1, const DSIZE& size2, real dDiff = 0.001f)
	{
		if(IsDoubleEqual(size1.cx, size2.cx, dDiff)&&
		   IsDoubleEqual(size1.cy, size2.cy, dDiff))
		{
			return true;
		}

		return false;
	}

	inline CdPoint MiddlePoint(const CdPoint& point1, const CdPoint& point2)
	{
		return CdPoint((point1.x + point2.x) * 0.5f, (point1.y + point2.y) * 0.5f);
	}

	inline CdPoint MiddlePoint(const CdPoint* lpPoint, int cnPoint)
	{
		if (cnPoint <= 0)
		{
			OUGHT(false);
			return CdPoint(0, 0);
		}

		CdPoint ptOrg(lpPoint[0]);
		CdSize vectorTemp(0, 0);

		for (int k = 1; k < cnPoint; k++)
		{
			// Same code as "vector += lpPoint[k] - ptOrg;"
			// Write as this way for prevent warning C4762 and C4759 in MSVC 1.5
			vectorTemp += *(lpPoint + k) - ptOrg;
		}
		vectorTemp /= (real)cnPoint;
		
		return ptOrg + vectorTemp;
	}

	inline real GetIntercrossAngle(const CdSize& vector1, const CdSize& vector2)
	{
		if (vector1.IsNull() || vector2.IsNull())
			return 0;

		real angle = vector2.GetPolarAngle() - vector1.GetPolarAngle();
		if (angle <= -M_PI)
			angle += M_PI + M_PI;
		else if (angle > M_PI)
			angle -= M_PI + M_PI;
		
		return angle;
	}

	inline real GetIntercrossAngle(const CdSize& vector1, const CdSize& vector2, real epsilon)
	{
		if (vector1.IsSmall(epsilon) || vector2.IsSmall(epsilon))
		{
			return 0;
		}

		return GetIntercrossAngle(vector1, vector2);
	}

}