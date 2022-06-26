#pragma once

#include <float.h>
#include "Graph_Math.h"
#pragma warning(disable:4756)


namespace Graph
{
	#pragma pack(push, 1)
	template<class T>
	struct DRECTT
	{
		T left;
		T top;
		T right;
		T bottom;
	};
	#pragma pack(pop)

	#pragma pack(push,1)
	template<class T, bool t_bFloat>
	class CdRectT : public DRECTT<T>
	{
		//因为模板不支持用浮点数作为参数
		//不能定义成template<class T, double t_dValue> class CdRectT
		
		typedef DRECTT<T> baseClass;
		typedef CdRectT<T, t_bFloat> thisClass;
	public:
		CdRectT()
		{
			SetRectEmpty();
		}

		CdRectT(baseClass rect)
		{
			SetRect(rect.left,rect.top,rect.right,rect.bottom);
		}
		
		CdRectT(RECT rect)
		{
			SetRect((T)rect.left, (T)rect.top, (T)rect.right, (T)rect.bottom);
		}

		CdRectT(const CdPointT<T>& ptTopLeft,const CdPointT<T>& ptBottomRight)
		{
			SetRect(ptTopLeft, ptBottomRight);
		}

		CdRectT(const CdPointT<T>& point,const CdSizeT<T>& size)
		{
			SetRect(point, size);
		}

		CdRectT(T left,T top,T right,T bottom)
		{
			SetRect(left,top,right,bottom);
		}
		
		~CdRectT()
		{
		}
		
	public:
		// convert to RECT
		RECT ToRECT() const
		{
			RECT rect={ToNearestInt(left), ToNearestInt(top),
				ToNearestInt(right), ToNearestInt(bottom)};
			return rect;
		}

	public:
		inline CdSizeT<T> Size() const
		{
			return BottomRight() - TopLeft();
		}

		inline T Width() const
		{
			return right - left;
		}
		
		inline T Height() const
		{
			return bottom-top;
		}
		
	public:
		inline CdPointT<T>& TopLeft()
		{
			return *(CdPointT<T>*)(&left);
		}
		
		inline const CdPointT<T>& TopLeft() const
		{
			return *(const CdPointT<T>*)(&left);
		}

		inline CdPointT<T>& BottomRight()
		{
			return *(CdPointT<T>*)(&right);
		}

		inline const CdPointT<T>& BottomRight() const
		{
			return *(const CdPointT<T>*)(&right);
		}
		
		inline CdPointT<T> TopRight() const
		{
			return CdPointT<T>(right, top);
		}

		inline CdPointT<T> BottomLeft() const
		{
			return CdPointT<T>(left,bottom);
		}

	public:
		// set rectangle
		void SetRect(T _left,T _top,T _right,T _bottom)
		{
			left = _left;
			top = _top;
			right = _right;
			bottom = _bottom;
		}

		inline void SetRect(const CdPointT<T>& ptTopLeft,const CdPointT<T>& ptBottomRight)
		{
			SetRect(ptTopLeft.x, ptTopLeft.y, ptBottomRight.x, ptBottomRight.y);
		}

		inline void SetRect(const CdPointT<T>& point,const CdSizeT<T>& size)
		{
			SetRect(point.x, point.y, point.x+size.cx, point.y+size.cy);
		}
		
		inline void CopyRect(const thisClass& rect)
		{
			SetRect(rect.left, rect.top, rect.right, rect.bottom);
		}
		
	public:
		// adjust whether two rectangles are equal
		bool EqualRect(const thisClass& rect) const
		{
			return D_Equal(left, rect.left) && D_Equal(top, rect.top)
				&& D_Equal(right, rect.right) && D_Equal(bottom, rect.bottom);
		}
		
	public:
		// NOTE: The empty property of a rectangle depends on the math-rect flag
		// set rectangle to empty rect in non-math-rect mode
		// set rectangle to absolute null rect in non-math-rect mode
		inline void SetRectEmpty()
		{
			SetRectNull();
		}

		// adjust whether the rect contains some point
		//   adjust whether left>=right or top>=bottom in non-math-rect mode
		//   adjust whether left>right and top>bottom in math-rect mode
		inline bool IsRectEmpty() const
		{
			return !IsRectPositive();
		}
		
		// set rectangle to { 0,0,0,0 }
		// NOTE: Null Rect is not an absolute empty one,
		//       may contains a point (0,0) (in math-rect mode)
		inline void SetRectNull()
		{
			left=top=right=bottom=0;
		}

		// adjust whether rectangle is null (see SetRectNull())
		inline bool IsRectNull() const
		{
			return (left == 0) && (top == 0)
				&& (right == 0) && (bottom == 0);
		}

		// adjust whether left<=right and top<=bottom
		inline bool IsRectNormal() const
		{
			return (left <= right) && (top <= bottom);
		}

		// adjust whether left<right and top<bottom
		inline bool IsRectPositive() const
		{
			return (left < right) && (top < bottom);
		}

		// adjust whether left>right or top>bottom
		inline bool IsRectAbsoluteEmpty() const
		{
			return (left > right) || (top > bottom);
		}

		// set rectangle to { real_MAX,real_MAX,-real_MAX,-real_MAX }
		inline void SetRectAbsoluteNull()
		{
			//#pragma warning(disable:4127)
			#pragma warning(disable:4056)
			if (t_bFloat)
			{
				left = top = (T)FLT_MAX;   
				right = bottom = -(T)FLT_MAX;   
			}								
			else								
			{									
				left = top = (T)DBL_MAX;		
				right = bottom = -(T)DBL_MAX;	
			}
			//#pragma warning(default:4127)
			#pragma warning(default:4056)
		}

		// adjust whether left>right or top>bottom
		inline bool IsRectAbsoluteNull() const
		{
			//#pragma warning(disable:4127)
			#pragma warning(disable:4056)
			if (t_bFloat)
			{
				return (left == (T)FLT_MAX) && (top == (T)FLT_MAX)
					&& (right == -(T)FLT_MAX) && (bottom == -(T)FLT_MAX);
			}
			else
			{
				return (left == (T)DBL_MAX) && (top == (T)DBL_MAX)
					&& (right == -(T)DBL_MAX) && (bottom == -(T)DBL_MAX);
			}
			//#pragma warning(default:4127)
			#pragma warning(default:4056)
		}

		// adjust whether left==right or top==bottom
		inline bool IsLineFigure()const
		{
			return (left == right || top == bottom);
		}
		
	public:
		// make normal by exchanging left and right,top and bottom if necessary
		// Note: an absolute null rect cannot be normalized
		void NormalizeRect()
		{
			if(IsRectAbsoluteNull())
			{
				assert(false);
				return;
			}
			
			if(left>right)
			{
				T tmp = left;
				left = right;
				right = tmp;
			}
			if(top>bottom)
			{
				T tmp = top;
				top = bottom;
				bottom = tmp;
			}
		}
		
		// make rectangle absolute null if it is not empty
		void AdjustRect()
		{
			if(IsRectEmpty())
			{
				SetRectAbsoluteNull();
			}
		}

	public:
		// inflate rectangle
		inline void InflateRect(T cx,T cy)
		{
			InflateRect(cx, cy, cx, cy);
		}

		inline void InflateRect(const CdSizeT<T>& size)
		{
			InflateRect(size.cx, size.cy);
		}

		void InflateRect(T _left, T _top, T _right, T _bottom)
		{
			left -= _left;
			top -= _top;
			right += _right;
			bottom += _bottom;
		}

		inline void InflateRect(const thisClass& rectInflate)
		{
			InflateRect(rectInflate.left, rectInflate.top,
				rectInflate.right, rectInflate.bottom);
		}

		inline void DeflateRect(T cx,T cy)
		{
			DeflateRect(cx, cy, cx, cy);
		}

		inline void DeflateRect(const CdSizeT<T>& size)
		{
			DeflateRect(size.cx, size.cy);
		}

		void DeflateRect(T _left, T _top, T _right, T _bottom)
		{
			left += _left;
			top += _top;
			right -= _right;
			bottom -= _bottom;
		}

		inline void DeflateRect(const thisClass& rectInflate)
		{
			DeflateRect(rectInflate.left, rectInflate.top,
				rectInflate.right, rectInflate.bottom);
		}
		
	public:
		// offset rectangle
		void OffsetRect(T cx,T cy)
		{
			left += cx;
			top += cy;
			right += cx;
			bottom += cy;
		}

		inline void OffsetRect(const CdSizeT<T>& size)
		{
			OffsetRect(size.cx, size.cy);
		}

		inline void OffsetRect(const CdPointT<T>& point)
		{
			OffsetRect(point.x, point.y);
		}

		void ZoomRect(T dXScale, T dYScale)
		{
			left *= dXScale;
			top *= dYScale;
			right *= dXScale;
			bottom *= dYScale;
		}

	public:
		// adjust whether the point is inside the rectangle
		//#define between(a,b,c)	(((a)<=(b)) && ((b)<=(c)))
		inline bool PtInRect(const CdPointT<T>& point) const
		{
			return between(left, point.x, right)
				&& between(top, point.y, bottom);
		}
		
		// adjust whether the point is on the edge of rectangl
		inline bool PtOnRect(const CdPointT<T>& point) const
		{
			return (between(left, point.x, right) && ((point.y == top) || (point.y == bottom)))
				|| (between(top, point.y, bottom) && ((point.x == left) || (point.y == right)));
		}
		
		// as above,note: the hit test distance is delta (not include delta)
		bool PtOnRect(const CdPointT<T>& point, T delta) const
		{
			if(IsRectEmpty())
			{
				return false;
			}
			#define isnear(a,b,d)	(((b)-(d)<(a)) && ((a)<(b)+(d)))
			return (between(left - delta, point.x, right + delta)
				&& (isnear(point.y, top, delta) || isnear(point.y, bottom, delta)))
				|| (between(top - delta, point.y, bottom + delta)
				&& (isnear(point.x, left, delta) || isnear(point.x, right, delta)));
			#undef isnear
		}
		#undef between

		bool IsIntersectWith(const thisClass& rect)const
		{
			if (Graph::D_GreaterEqual(left, rect.right))
				return false;
			if (Graph::D_GreaterEqual(top, rect.bottom))
				return false;
			if (Graph::D_LessEqual(right, rect.left))
				return false;
			if (Graph::D_LessEqual(bottom, rect.top))
				return false;
			return true;
		}

		bool IsInternalOf(const thisClass& rect)const
		{
			if (!IsIntersectWith(rect))
				return false;

			if (Graph::D_LessThan(left, rect.left))
				return false;
			if (Graph::D_LessThan(top, rect.top))
				return false;
			if (Graph::D_GreaterThan(right, rect.right))
				return false;
			if (Graph::D_GreaterThan(bottom, rect.bottom))
				return false;
			return true;
		}

		bool IsTangentOutWith(const thisClass& rect)const
		{
			if (Graph::D_GreaterThan(left, rect.right))
				return false;
			if (Graph::D_GreaterThan(top, rect.bottom))
				return false;
			if (Graph::D_LessThan(right, rect.left))
				return false;
			if (Graph::D_LessThan(bottom, rect.top))
				return false;

			if (Graph::D_Equal(left, rect.right))
				return true;
			if (Graph::D_Equal(top, rect.bottom))
				return true;
			if (Graph::D_Equal(right, rect.left))
				return true;
			if (Graph::D_Equal(bottom, rect.top))
				return true;
			return false;
		}

		bool IsTangentInWith(const thisClass& rect)const
		{
			if (Graph::D_LessThan(left, rect.left))
				return false;
			if (Graph::D_LessThan(top, rect.top))
				return false;
			if (Graph::D_GreaterThan(right, rect.right))
				return false;
			if (Graph::D_GreaterThan(bottom, rect.bottom))
				return false;

			if (Graph::D_Equal(left, rect.left))
				return true;
			if (Graph::D_Equal(top, rect.top))
				return true;
			if (Graph::D_Equal(right, rect.right))
				return true;
			if (Graph::D_Equal(bottom, rect.bottom))
				return true;
			return false;
		}

	public:
		// intersect two rectangles,return true if it is not empty rectangle
		//   (adjust whether *pRect1 or *pRect2 is empty)
		bool IntersectRect(const thisClass* pRect1, const thisClass* pRect2)
		{
			CopyRect(*pRect1);
			operator &= (*pRect2);
			return !IsRectEmpty();
		}

		bool Intersect(const thisClass Rect1,const thisClass Rect2)
		{
			if(Rect1.IsRectAbsoluteEmpty())
			{
				return false;
			}

			if(Rect2.IsRectEmpty())
			{
				SetRectAbsoluteNull();
			}
			else
			{
				if(Rect1.left < Rect2.left)
				{
					left = Rect2.left;
				}
				else
				{
					left = Rect1.left;
				}

				if(Rect1.right > Rect2.right)
				{
					right = Rect2.right;
				}
				else
				{
					right = Rect1.right;
				}

				if(Rect1.top < Rect2.top)
				{
					top = Rect2.top;
				}
				else
				{
					top = Rect1.top;
				}

				if(Rect1.bottom > Rect2.bottom)
				{
					bottom = Rect2.bottom;
				}
				else
				{
					bottom = Rect1.bottom;
				}
			}
			return !IsRectAbsoluteEmpty();
		}
		
		//  as IntersectRect without adjusting whether *pRect1 or *pRect2 is empty
		void IntersectRectExt(const thisClass* pRect1, const thisClass* pRect2)
		{
			CopyRect(*pRect1);
			IntersectRectExt(*pRect2);
		}

		// union two rectangles,return true if it is not empty rectangle
		//   (adjust whether *pRect1 or *pRect2 is empty)
		bool UnionRect(const thisClass* pRect1, const thisClass* pRect2)
		{
			CopyRect(*pRect1);
			operator |= (*pRect2);
			return !IsRectEmpty();
		}

		//  as UnionRect without adjusting whether *pRect1 or *pRect2 is empty
		void UnionRectExt(const thisClass* pRect1, const thisClass* pRect2)
		{
			CopyRect(*pRect1);
			UnionRectExt(*pRect2);
		}

		// get the minimun rectangle which include the remain part of
		//  *pRect1 outside *pRect2,
		//  return true if it is not empty rectangle
		bool SubtractRect(const thisClass* pRect1, const thisClass* pRect2)
		{
			*this = *pRect1;
			if(IsRectPositive() && pRect2->IsRectPositive())
			{
				if((pRect2->left <= left) && (right <= pRect2->right))
				{
					if((pRect2->top <= top) && (top < pRect2->bottom))
					{
						top = pRect2->bottom;
					}
					else if((pRect2->top < bottom) && (bottom <= pRect2->bottom))
					{
						bottom = pRect2->top;
					}
				}
				if((pRect2->top <= top) && (bottom <= pRect2->bottom))
				{
					if((pRect2->left <= left) && (left < pRect2->right))
					{
						left = pRect2->right;
					}
					else if((pRect2->left < right) && (right <= pRect2->right))
					{
						right = pRect2->left;
					}
				}
			}
			AdjustRect();
			return !IsRectEmpty();
		}

	public:
		// see above
		//   duplicating the argument (rect1 or rect2 can be same as *this)
		inline bool IntersectRect(thisClass rect1, thisClass rect2)
		{
			return IntersectRect(&rect1, &rect2);
		}
		
		inline void IntersectRectExt(thisClass rect1, thisClass rect2)
		{
			IntersectRectExt(&rect1, &rect2);
		}

		inline bool UnionRect(thisClass rect1, thisClass rect2)
		{
			return UnionRect(&rect1, &rect2);
		}

		inline void UnionRectExt(thisClass rect1, thisClass rect2)
		{
			UnionRectExt(&rect1, &rect2);
		}

		inline bool SubtractRect(thisClass rect1, thisClass rect2)
		{
			return SubtractRect(&rect1, &rect2);
		}

	public:	
		inline const thisClass& operator = (const thisClass& rect)
		{
			CopyRect(rect);
			return rect;
		}

	public:
		// get the center point of the rect
		inline CdPointT<T> GetCenter() const
		{
			return MiddlePoint(TopLeft(), BottomRight());
		}

		bool IsLegalRect()const
		{
			if (D_GreaterEqual(Width(), dConst_Rect_MinLen, dConst_MinPrecision)
				&&D_GreaterEqual(Height(), dConst_Rect_MinLen, dConst_MinPrecision)
				&&D_LessEqual(Width(), dConst_Rect_MaxLen, dConst_MinPrecision)
				&&D_LessEqual(Height(), dConst_Rect_MaxLen, dConst_MinPrecision))
			{
				return true;
			}
			return false;
		}

		CdPointT<T> AdjustPointInRect(const CdPointT<T>& point) const
		{
			CdPointT<T> ptRet(point);
			if (D_GreaterEqual(point.x, right, dConst_MinPrecision))
			{
				ptRet.x = right;
			}
			else if (D_LessEqual(point.x, left, dConst_MinPrecision))
			{
				ptRet.x = left;
			}
			
			if (D_GreaterEqual(point.y, bottom, dConst_MinPrecision))
			{
				ptRet.y = bottom;
			}
			else if (D_LessEqual(point.y, top, dConst_MinPrecision))
			{
				ptRet.y = top;
			}
			return ptRet;
		}
		
	public:	
		// offset rectangle
		inline void operator += (const CdPointT<T>& point)
		{
			OffsetRect(point);
		}

		inline void operator += (const CdSizeT<T>& size)
		{
			OffsetRect(size);
		}

		inline void operator -= (const CdPointT<T>& point)
		{
			OffsetRect(-point);
		}

		inline void operator-=(const CdSizeT<T>& size)
		{
			OffsetRect(-size);
		}

		// inflate rectangle
		inline void operator += (const thisClass& rect)
		{
			InflateRect(rect);
		}

		// deflate rectangle
		inline void operator -= (const thisClass& rect)
		{
			DeflateRect(rect);
		}

		// intersect rectangles
		void operator &= (const thisClass& rect)
		{
			if(IsRectEmpty())
			{
				return;
			}

			if(rect.IsRectEmpty())
			{
				SetRectAbsoluteNull();
			}
			else
			{
				IntersectRectExt(rect);
			}
		}
		
		// union rectangles
		void operator |= (const thisClass& rect)
		{
			if(IsRectEmpty())
			{
				operator=(rect);
			}
			else if(!rect.IsRectEmpty())
			{
				UnionRectExt(rect);
			}
		}
		
	public:
		// intersect rectangles without adjusting whether empty
		void IntersectRectExt(const thisClass& rect)
		{
			if(left < rect.left)
			{
				left = rect.left;
			}

			if(right > rect.right)
			{
				right = rect.right;
			}

			if(top < rect.top)
			{
				top = rect.top;
			}

			if(bottom > rect.bottom)
			{
				bottom = rect.bottom;
			}

			AdjustRect();
		}

		// 判断是否与rect相交，只要rect是normal的就行，无所谓empty
		bool IntersectRectExt(const thisClass& rect)const
		{
			if (!IsRectNormal() || !rect.IsRectNormal())
			{
				return false;
			}
			if (left > rect.right || right < rect.left ||
				top > rect.bottom || bottom < rect.top)
			{
				return false;
			}
			return true;
		}

		// union rectangles without adjusting whether empty
		void UnionRectExt(const thisClass& rect)
		{
			if(left > rect.left)
			{
				left = rect.left;
			}

			if(right < rect.right)
			{
				right = rect.right;
			}

			if(top > rect.top)
			{
				top = rect.top;
			}

			if(bottom < rect.bottom)
			{
				bottom = rect.bottom;
			}
		}

		void UnionRectExt(const CdPointT<T>& pt)
		{
			if(left > pt.x)
			{
				left = pt.x;
			}

			if(right < pt.x)
			{
				right = pt.x;
			}

			if(top > pt.y)
			{
				top = pt.y;
			}

			if(bottom < pt.y)
			{
				bottom = pt.y;
			}
		}
	};
	#pragma pack(pop)

	typedef DRECTT<float> DRECTFLOAT;
	typedef CdRectT<float, true> CdRectFloat;
	typedef DRECTT<double> DRECTDOUBLE;
	typedef CdRectT<double, false> CdRectDouble;

#ifdef USING_DOUBLEFLOAT
	typedef DRECTDOUBLE DRECT;
	typedef CdRectDouble CdRect;
#else
	typedef DRECTFLOAT DRECT;
	typedef CdRectFloat CdRect;
#endif

	inline bool operator == (const CdRect& rect1, const CdRect& rect2)
	{
		return (rect1.left  == rect2.left)
			&& (rect1.top   == rect2.top)
			&& (rect1.right == rect2.right)
			&& (rect1.bottom == rect2.bottom);
	}

	inline bool operator != (const CdRect& rect1, const CdRect& rect2)
	{
		return !operator == (rect1, rect2);
	}

	// offset rectangle
	inline CdRect operator + (CdRect rect, const CdPoint& point)
	{
		rect += point;
		return rect;
	}

	inline CdRect operator - (CdRect rect, const CdPoint& point)
	{
		rect-=point;
		return rect;
	}

	inline CdRect operator + (CdRect rect, const CdSize& size)
	{
		rect += size;
		return rect;
	}

	inline CdRect operator - (CdRect rect, const CdSize& size)
	{
		rect -= size;
		return rect;
	}

	inline CdRect operator + (CdRect rect, const CdRect& rectInflate)
	{
		rect += rectInflate;
		return rect;
	}

	inline CdRect operator - (CdRect rect, const CdRect& rectDeflate)
	{
		rect-=rectDeflate;
		return rect;
	}

	inline CdRect operator & (CdRect rect1, const CdRect& rect2)
	{
		rect1 &= rect2;
		return rect1;
	}

	inline CdRect operator | (CdRect rect1, const CdRect& rect2)
	{
		rect1 |= rect2;
		return rect1;
	}

	inline bool IsRectInside(const CdRect& rect1, const CdRect& rect2, real dComparePrecision = 1.0e-14f)
	{
		if (rect1.IsRectEmpty())
		{
			return true;
		}

		if (rect2.IsRectEmpty())
		{
			return false;
		}

		if ((D_GreaterEqual(rect1.left, rect2.left,dComparePrecision))
				&& D_LessEqual(rect1.right,rect2.right,dComparePrecision)
				&& D_GreaterEqual(rect1.top, rect2.top,dComparePrecision)
				&& D_LessEqual(rect1.bottom , rect2.bottom,dComparePrecision))
		{
			return true;
		}

		return false;
	}

	inline bool IsLineIntersect(const CdPoint& pt1Start, const CdPoint& pt1End, 
		const CdPoint& pt2Start, const CdPoint& pt2End)
	{
		real dTemp1, dTemp2, dLength;
		CdPoint ptInter;

		dTemp1 = (pt2End.y - pt2Start.y) * (pt1End.x - pt1Start.x);
		dTemp2 = (pt2End.x - pt2Start.x) * (pt1End.y - pt1Start.y);

		if(D_Equal(dTemp1, dTemp2, dConst_MinPrecision))
		{
			return false;
		}

		ptInter.x = ((pt1Start.y - pt1End.y) * pt1Start.x * (pt2End.x - pt2Start.x) +
					(pt2End.y - pt2Start.y) * pt2Start.x * (pt1End.x - pt1Start.x) +
					(pt1Start.y - pt2Start.y) * (pt2End.x - pt2Start.x) * (pt1End.x - pt1Start.x)) /
					(dTemp1 - dTemp2);
		ptInter.y = ((pt1Start.x - pt1End.x) * pt1Start.y * (pt2End.y - pt2Start.y) +
					(pt2End.x - pt2Start.x) * pt2Start.y * (pt1End.y - pt1Start.y) +
					(pt1Start.x - pt2Start.x) * (pt2End.y - pt2Start.y) * (pt1End.y - pt1Start.y)) /
					(dTemp2 - dTemp1);
		
		dLength = PointDist(pt1Start, pt1End);
		if(PointDist(ptInter, pt1Start) > dLength||
		   PointDist(ptInter, pt1End) > dLength)
		{
			return false;
		}

		dLength = PointDist(pt2Start, pt2End);
		if(PointDist(ptInter, pt2Start) > dLength||
		   PointDist(ptInter, pt2End) > dLength)
		{
			return false;
		}

		return true;
	}

	inline bool IsLineIntersectRect(const CdPoint& ptStart, const CdPoint& ptEnd, const CdRect& rect)
	{
		if(rect.PtInRect(ptStart) || rect.PtInRect(ptEnd))
		{
			return true;
		}
		
		if(IsLineIntersect(ptStart, ptEnd, rect.TopLeft(), rect.BottomLeft()))
		{
			return true;
		}
		
		if(IsLineIntersect(ptStart, ptEnd, rect.BottomLeft(), rect.BottomRight()))
		{
			return true;
		}
		
		if(IsLineIntersect(ptStart, ptEnd, rect.BottomRight(), rect.TopRight()))
		{
			return true;
		}
		
		if(IsLineIntersect(ptStart, ptEnd, rect.TopRight(), rect.TopLeft()))
		{
			return true;
		}

		return false;
	}

	inline bool IsRectIntersect(const CdRect& rect1, const CdRect& rect2, real dComparePrecision = 1.0e-14f)
	{
		if(rect1.IsRectEmpty() || rect2.IsRectEmpty())
		{
			return false;
		}
		
		assert(rect1.IsRectNormal() && rect2.IsRectNormal());
		if(D_GreaterThan(rect1.left, rect2.right, dComparePrecision) || 
			D_LessThan(rect1.right, rect2.left, dComparePrecision) || 
			D_GreaterThan(rect1.top, rect2.bottom, dComparePrecision) || 
			D_LessThan(rect1.bottom, rect2.top, dComparePrecision))
		{
			return false;
		}
		
		return true;
	}

}
#pragma warning(default:4756)