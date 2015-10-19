#pragma once
namespace Graph
{
	#pragma pack(push,1)
	class CdLine
	{
	public:
		CdLine()
		{
		}
		
		CdLine(real x1, real y1, real x2, real y2)
		{
			m_point.x = x1;
			m_point.y = y1;
			m_vector.cx = x2 - x1;
			m_vector.cy = y2 - y1;
		}

		CdLine(const DPOINT& point1, const DPOINT& point2)
		{
			m_point.x = point1.x;
			m_point.y = point1.y;
			m_vector.cx = point2.x - point1.x;
			m_vector.cy = point2.y - point1.y;
		}

		CdLine(const CdPoint& point1, const CdPoint& point2)
		{
			m_point = point1;
			m_vector = point2 - point1;	
		}
		
		inline void operator = (const CdLine & line)
		{
			m_point = line.m_point;
			m_vector = line.m_vector;
		}

		// 获得直线上的点
		inline CdPoint GetLambdaPoint(real lambda) const
		{
			return m_point + m_vector * lambda;
		}

		// 点到线段的最短距离
		real DistFromLine(const CdPoint& point0) const
		{
			real lambda = 0;
			if (!(m_vector.cx == 0 && m_vector.cy == 0))
			{
				lambda = GetInfiniteLambdaOfPoint(point0);
			}

			if (lambda <= 0)
			{
				return PointDist(point0, m_point);
			}
			if (lambda >= 1)
			{
				return PointDist(point0, m_point + m_vector);
			}
			
			return DistFromInfiniteLine(point0);
		}
		
		inline real GetInfiniteLambdaOfPoint(const CdPoint& point0) const
		{
			return GetVectorProjectLambda(point0 - m_point, m_vector);
		}	

		inline real DistFromInfiniteLine(const CdPoint& point0) const
		{
			real dist = SignedDistFromInfiniteLine(point0);
			return (dist >= 0 ? dist : -dist);
		}

		real SignedDistFromInfiniteLine(const CdPoint& point0) const
		{
			real tmp = m_vector.GetLength();
			if (tmp == 0.0f)
			{
				return PointDist(point0, m_point);
			}
			else
			{
				return (m_vector ^ (point0 - m_point)) / tmp;
			}
		}

		inline void SetStartPoint(const DPOINT& ptStart)
		{
			m_point.x = ptStart.x;
			m_point.y = ptStart.y;
		}

		inline void SetStopPoint(const DPOINT& ptStop)
		{
			m_vector.cx = ptStop.x - m_point.x;
			m_vector.cy = ptStop.y - m_point.y;
		}

		inline void SetStartPoint(const CdPoint& ptStart)
		{
			m_point = ptStart;
		}

		inline void SetStopPoint(const CdPoint& ptStop)
		{
			m_vector = ptStop - m_point;
		}

		void InitPoints(const CdPoint& ptStart, const CdPoint& ptStop)
		{
			SetStartPoint(ptStart);
			SetStopPoint(ptStop);
		}

		inline CdPoint GetStartPoint() const
		{
			return m_point;
		}

		inline CdPoint GetStopPoint() const
		{
			return m_point + m_vector;
		}

		// 判断点与线段的相对位置
		// 0：左侧 1：右侧
		real GetInsideLambdaOfPoint(const CdPoint& point0) const
		{
			real lambda = GetInfiniteLambdaOfPoint(point0);
			if (lambda <= 0)
			{
				return 0;
			}
			if (lambda >= 1)
			{
				return 1;
			}

			return lambda;
		}

		inline bool PointInLine(const CdPoint& point0, real delta)
		{
			return DistFromLine(point0) <= delta;
		}

		// 获得直线向量
		inline const CdSize& GetVector() const
		{
			return m_vector;
		}

		// 获得直线的角度
		inline real GetPolarAngle() const
		{
			return m_vector.GetPolarAngle();
		}

	public:
		bool GetSlope(real& dSlope) const
		{
			if (! D_Equal(m_vector.cx, 0))
			{
				dSlope = m_vector.cy / m_vector.cx;
			}			
			return !D_Equal(m_vector.cx, 0);
		}
		
		inline real GetLength() const
		{
			return m_vector.GetLength();
		}
		
		// 沿法线正方向移动
		bool OffsetNormal(real dOffset)
		{
			real dLength = m_vector.GetLength();
			if (dLength)
			{
				m_point.x += dOffset * m_vector.cy / dLength;
				m_point.y -= dOffset * m_vector.cx / dLength;
			}
			return !D_Equal(dLength, 0);
		}
		
		// 沿切线方向移动
		bool OffsetTangent(real dOffset)
		{
			real dLength = m_vector.GetLength();
			if (dLength)
			{
				m_point.x += (dOffset * m_vector.cx / dLength);
				m_point.y += (dOffset * m_vector.cy / dLength);
			}
			return !D_Equal(dLength, 0);
		}
		
	private:
		CdPoint m_point;
		CdSize  m_vector;
	};
	#pragma pack(pop)

	inline real GetDistFromLine(const CdLine& line, const CdPoint& pt, real dDiff=0.001f)
	{
		if(IsSizeEqual(line.GetStopPoint() - line.GetStartPoint(), CdSize(0,0), dDiff))
		{
			return PointDist(pt, line.GetStopPoint());
		}
		else
		{
			return line.DistFromLine(pt);
		}
	}

	// 计算两直线交点
	#define fabs_(x)	(((x) >= 0) ? (x) : (-(x)))
	inline int IntersectLine(const CdPoint& pt1, const CdSize& v1, const CdPoint pt2, const CdSize& v2,
		real* pLambda1, real* pLambda2, CdPoint* pPt)
	{
		// 入参检查
		if((v1.cx == 0) && (v1.cy == 0) || (v2.cx == 0) && (v2.cy == 0))
		{
			return INTERSECTLINE_NONE;
		}

		const bool bHor1 = (v1.cy == 0.0f);
		const bool bVer1 = (v1.cx == 0.0f);
		const bool bHor2 = (v2.cy == 0.0f);
		const bool bVer2 = (v2.cx == 0.0f);
		if(bHor1 && !bVer1 && bHor2 && !bVer2)
		{
			if (pLambda1 != NULL)
			{
				*pLambda1 = v2.cx / v1.cx;
			}
			if (pLambda2 != NULL)
			{
				*pLambda2 = (pt2.x - pt1.x) / v1.cx;
			}
			if(pt1.y == pt2.y)
			{
				return INTERSECTLINE_COVER;
			}
			else
			{
				return INTERSECTLINE_PARALL;
			}
		}
		else if(!bHor1 && bVer1 && !bHor2 && bVer2)
		{
			if (pLambda1 != NULL)
			{
				*pLambda1 = v2.cy / v1.cy;
			}
			if (pLambda2 != NULL)
			{
				*pLambda2 = (pt2.y - pt1.y) / v1.cy;
			}
			if(pt1.x == pt2.x)
			{
				return INTERSECTLINE_COVER;
			}
			else
			{
				return INTERSECTLINE_PARALL;
			}
		}
		else if(bHor1 && !bVer1 && !bHor2 && bVer2)
		{
			if(pLambda1 != NULL)
			{
				*pLambda1 = (pt2.x - pt1.x) / v1.cx;
			}
			if(pLambda2 != NULL)
			{
				*pLambda2 = (pt1.y - pt2.y) / v2.cy;
			}
			if(pPt != NULL)
			{
				(*pPt).x = pt2.x;
				(*pPt).y = pt1.y;
			}
		}
		else if(!bHor1 && bVer1 && bHor2 && !bVer2)
		{
			if(pLambda1 != NULL)
			{
				*pLambda1 = (pt2.y - pt1.y) / v1.cy;
			}
			if(pLambda2 != NULL)
			{
				*pLambda2 = (pt1.x - pt2.x) / v2.cx;
			}
			if(pPt != NULL)
			{
				(*pPt).x = pt1.x;
				(*pPt).y = pt2.y;
			}
		}
		else
		{
			const CdSize v(pt2 - pt1);
			static const real dIntersectLineLambda1 = 1.0E-5f;

			const real dArea = (v1 ^ v2);
			const real dLenSqr1 = v1 * v1;
			const real dLenSqr2 = v2 * v2;
			const real dLenSqrSum = dLenSqr1 + dLenSqr2;
			if (fabs_(dArea) <= (dLenSqrSum * dIntersectLineLambda1))
			{
				if (pLambda1 != NULL)
				{
					*pLambda1 = (v2 * v1) / dLenSqr1;
				}
				if (pLambda2 != NULL)
				{
					*pLambda2 = (v * v1) / dLenSqr1;
				}
				if (fabs_(v ^ v1) <= (dLenSqr1 * dIntersectLineLambda1))
				{
					return INTERSECTLINE_COVER;
				}
				else
				{
					return INTERSECTLINE_PARALL;
				}
			}

			const real lambda1 = (v ^ v2) / dArea;
			const real lambda2 = (v ^ v1) / dArea;
			if (pLambda1 != NULL)
			{
				*pLambda1 = lambda1;
			}
			if (pLambda2 != NULL)
			{
				*pLambda2 = lambda2;
			}
			if (pPt != NULL)
			{
				*pPt = MiddlePoint(pt1 + v1 * lambda1, pt2 + v2 * lambda2);
			}
		}

		return INTERSECTLINE_NORMAL;

	}
	#undef fabs_

	// 计算两直线交点
	inline int IntersectLine(const CdLine& line1, const CdLine& line2,
		real* pLambda1, real* pLambda2, CdPoint* pPt)
	{
		return IntersectLine(line1.GetStartPoint(), line1.GetVector(), 
			line2.GetStartPoint(), line2.GetVector(),
			pLambda1, pLambda2, pPt);
	}

	inline void GetTwoLineCross(real A1, real B1, real C1, 
		real A2, real B2, real C2, CdPoint& ptCross)
	{	
		if(D_GreaterThan(fabs(A2*B1 - A1*B2), 0, dConst_MinPrecision ))
		{
			ptCross.y = (A1*C2 - A2*C1)/(A2*B1 - A1*B2);
			ptCross.x = (B1*C2 - B2*C1)/(A1*B2 - A2*B1);
		}
		else
		{
			assert(false);
		}
	}

	inline void LineOffsetOutSide(CdPoint& pt1, CdPoint& pt2, real dOffset)
	{
		real dLength = sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));
		if(!D_Equal(dLength,0))
		{
			real dx = ( dOffset*(pt2.y - pt1.y) / dLength );
			real dy = ( dOffset*(pt1.x - pt2.x) / dLength );
			pt1.x += dx;
			pt1.y += dy;
			pt2.x += dx;
			pt2.y += dy;
		}
	}

	inline bool GetIntersectPoint(const CdPoint& pt1, const CdPoint& pt2, 
		const CdPoint& pt3, const CdPoint& pt4, CdPoint& pt)
	{
		if(PointEqual(pt2, pt3))
		{
			pt = pt2;
			return true;
		}	
		
		real dx1 = pt2.x - pt1.x;
		real dy1 = pt2.y - pt1.y;
		real dx2 = pt4.x - pt3.x;
		real dy2 = pt4.y - pt3.y;
		real z1 = dy1 * pt1.x - dx1 * pt1.y;
		real z2 = dy2 * pt3.x - dx2 * pt3.y;
		real deno = dx1 * dy2 - dx2 * dy1;
		if(!D_Equal(deno, 0.0f))
		{
			pt.x = (dx1 * z2 - dx2 * z1) / deno;
			pt.y = (dy1 * z2 - dy2 * z1) / deno;
			return true;
		}
		return false;
	}

	// 判断点与线的相对位置关系
	// 返回值 －1:右侧 0:线上 1:左侧
	inline int GetRelativePos(const CdLine& line, const CdPoint& pt)
	{
		const real dx0 = line.GetVector().cx;
		const real dy0 = line.GetVector().cy;
		const real dx1 = pt.x - line.GetStartPoint().x;
		const real dy1 = pt.y - line.GetStartPoint().y;

		const real dTemp0 = dx0 * dy1;
		const real dTemp1 = dy0 * dx1;

		if(D_Equal(dTemp0, dTemp1))
		{
			return 0;
		}	
		else if(dTemp0 < dTemp1)
		{
			return -1;
		}
		else 
		{
			return 1;
		}
	}

	// 判断两点是否都处于一条直线的同侧
	inline bool IsSameSide(const CdLine& line, const CdPoint& pt1, const CdPoint& pt2)
	{
		int iPos1 = GetRelativePos(line, pt1);
		int iPos2 = GetRelativePos(line, pt2);
		if(iPos1 == -1 && iPos2 == -1 ||
			iPos1 == 0 || iPos2 == 0 ||
			iPos1 == 1 && iPos2 == 1)
		{
			return true;
		}
		return false;
	}

	// 判断两条线段是否相交
	inline bool IsSegIntersect(const CdLine& segline1, const CdLine& segline2)
	{
		if((segline1.GetVector().cx == 0) && (segline1.GetVector().cy == 0) && (segline2.GetVector().cx == 0) && (segline2.GetVector().cy == 0))
		{
			return false;
		}

		return (!IsSameSide(segline1, segline2.GetStartPoint(), segline2.GetStopPoint()) &&
			!IsSameSide(segline2, segline1.GetStartPoint(), segline1.GetStopPoint()));
	}

	// 计算两条线段的交点
	// 返回值 false:没有交点，出参pt无效; true:有交点，出参pt有效
	inline bool GetSegIntersect(const CdLine& segline1, const CdLine& segline2, CdPoint& pt)
	{
		if(!IsSegIntersect(segline1, segline2))
		{
			return false;
		}

		return GetIntersectPoint(segline1.GetStartPoint(), segline1.GetStopPoint(), 
			segline2.GetStartPoint(), segline2.GetStopPoint(), pt);
	}
}