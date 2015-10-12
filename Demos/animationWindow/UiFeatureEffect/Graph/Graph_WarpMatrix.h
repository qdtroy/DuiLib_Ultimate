#pragma once

#include <vector>
#include "graph_Rect.h"
#include "graph_Line.h"
#include "graph_Matrix.h"
#include <mmintrin.h>
#include <xmmintrin.h>

namespace Graph
{
	enum kWarpHandle
	{
		kWarpHandle_None,
		kWarpHandle_TopLeft,
		kWarpHandle_TopRight,
		kWarpHandle_BottomRight,
		kWarpHandle_BottomLeft
	};

	enum kVanishHandle{
		kVanishHandle_None,
		kVanishHandle_Left,
		kVanishHandle_Top,
		kVanishHandle_Right,
		kVanishHandle_Bottom,
	};

	#pragma pack(push,1)
	struct WarpRect
	{
		CdPoint topLeft;
		CdPoint topRight;
		CdPoint bottomRight;
		CdPoint bottomLeft;

	public:
		WarpRect()
		{
		}

		WarpRect(const CdRect& rect)
		{
			topLeft = rect.TopLeft();
			topRight = rect.TopRight();
			bottomRight = rect.BottomRight();
			bottomLeft = rect.BottomLeft();
		}

	public:
		// 判断是否是凸四边形
		bool IsConvexRect()const
		{
			CdLine line12(topLeft, topRight);
			CdLine line23(topRight, bottomRight);
			CdLine line34(bottomRight, bottomLeft);
			CdLine line41(bottomLeft, topLeft);

			if (IsSegIntersect(line12, line34) || IsSegIntersect(line23, line41))
			{
				return false;
			}

			if (! IsSameSide(line12, bottomLeft, bottomRight) ||
				! IsSameSide(line34, topRight, topLeft))
			{
				return false;
			}

			return true;
		}
		
		// 判断是否是凹四边形
		bool IsSunkenRect()const
		{
			CdLine line12(topLeft, topRight);
			CdLine line23(topRight, bottomRight);
			CdLine line34(bottomRight, bottomLeft);
			CdLine line41(bottomLeft, topLeft);

			if (IsSegIntersect(line12, line34) || IsSegIntersect(line23, line41))
			{
				return false;
			}

			if (IsSameSide(line12, bottomLeft, bottomRight) &&
				IsSameSide(line34, topRight, topLeft))
			{
				return false;
			}

			return true;
		}

		void GetPoints(std::vector<CdPoint>& arrPoint)const
		{
			arrPoint.clear();
			arrPoint.reserve(4);
			arrPoint.push_back(topLeft);
			arrPoint.push_back(topRight);
			arrPoint.push_back(bottomRight);
			arrPoint.push_back(bottomLeft);
		}

		kWarpHandle PtOnWarpHandle(const CdPoint& ptHit, const CdSize& dSize)const
		{
			Graph::CdRect dHandleRect;

			dHandleRect.SetRect(topLeft.x - dSize.cx,
				topLeft.y - dSize.cy,
				topLeft.x + dSize.cx,
				topLeft.y + dSize.cy);
			if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
			{
				return kWarpHandle_TopLeft;
			}
			dHandleRect.SetRect(topRight.x - dSize.cx,
				topRight.y - dSize.cy,
				topRight.x + dSize.cx,
				topRight.y + dSize.cy);
			if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
			{
				return kWarpHandle_TopRight;
			}
			dHandleRect.SetRect(bottomRight.x - dSize.cx,
				bottomRight.y - dSize.cy,
				bottomRight.x + dSize.cx,
				bottomRight.y + dSize.cy);
			if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
			{
				return kWarpHandle_BottomRight;
			}
			dHandleRect.SetRect(bottomLeft.x - dSize.cx,
				bottomLeft.y - dSize.cy,
				bottomLeft.x + dSize.cx,
				bottomLeft.y + dSize.cy);
			if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
			{
				return kWarpHandle_BottomLeft;
			}

			return kWarpHandle_None;
		}

		bool GetVanishHandle(kVanishHandle kVanish, CdPoint& pt)const
		{
			switch(kVanish) {
			case kVanishHandle_Left:
				return GetVanishHandle(topRight, topLeft, bottomRight, bottomLeft, pt);
				break;
			case kVanishHandle_Top:
				return GetVanishHandle(bottomLeft, topLeft, bottomRight, topRight, pt);
				break;
			case kVanishHandle_Right:
				return GetVanishHandle(topLeft, topRight, bottomLeft, bottomRight, pt);
				break;
			case kVanishHandle_Bottom:
				return GetVanishHandle(topLeft, bottomLeft, topRight, bottomRight, pt);
				break;
			default:
				assert(false);
			}

			return false;
		}

		kVanishHandle PtOnVanishHandle(const CdPoint& ptHit, const CdSize& dSize)const
		{
			Graph::CdRect dHandleRect;
			Graph::CdPoint pt;
			if (GetVanishHandle(kVanishHandle_Left, pt))
			{
				dHandleRect.SetRect(pt.x - dSize.cx,
					pt.y - dSize.cy,
					pt.x + dSize.cx,
					pt.y + dSize.cy);
				if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
				{
					return kVanishHandle_Left;
				}
			}
			if (GetVanishHandle(kVanishHandle_Top, pt))
			{
				dHandleRect.SetRect(pt.x - dSize.cx,
					pt.y - dSize.cy,
					pt.x + dSize.cx,
					pt.y + dSize.cy);
				if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
				{
					return kVanishHandle_Top;
				}
			}
			if (GetVanishHandle(kVanishHandle_Right, pt))
			{
				dHandleRect.SetRect(pt.x - dSize.cx,
					pt.y - dSize.cy,
					pt.x + dSize.cx,
					pt.y + dSize.cy);
				if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
				{
					return kVanishHandle_Right;
				}
			}
			if (GetVanishHandle(kVanishHandle_Bottom, pt))
			{
				dHandleRect.SetRect(pt.x - dSize.cx,
					pt.y - dSize.cy,
					pt.x + dSize.cx,
					pt.y + dSize.cy);
				if (dHandleRect.PtInRect(ptHit) || dHandleRect.PtOnRect(ptHit))
				{
					return kVanishHandle_Bottom;
				}
			}

			return kVanishHandle_None;
		}

		CdRect GetBoundRect()const
		{
			CdRect dBound(topLeft, topLeft);
			dBound.UnionRectExt(dBound, CdRect(topRight, topRight));
			dBound.UnionRectExt(dBound, CdRect(bottomRight, bottomRight));
			dBound.UnionRectExt(dBound, CdRect(bottomLeft, bottomLeft));
			return dBound;
		}

		void Offset(const CdSize& szOffset)
		{
			topLeft.Offset(szOffset);
			topRight.Offset(szOffset);
			bottomRight.Offset(szOffset);
			bottomLeft.Offset(szOffset);
		}

	private:
		bool GetVanishHandle(const CdPoint& pt1, const CdPoint& pt2,
			const CdPoint& pt3, const CdPoint& pt4, CdPoint& pt)const
		{
			real dLambda1, dLambda2;
			if (INTERSECTLINE_NORMAL == IntersectLine(Graph::CdLine(pt1, pt2),
				Graph::CdLine(pt3, pt4),
				&dLambda1,
				&dLambda2,
				&pt))
			{
				if (Graph::D_GreaterEqual(dLambda1, 1) &&
					Graph::D_GreaterEqual(dLambda2, 1))
				{
					return true;
				}
			}
			return false;
		}
	};
	#pragma pack(pop)

	/* Calculates coefficients of perspective transformation
	* which maps (xi,yi) to (ui,vi), (i=1,2,3,4):
	*
	*      c00*xi + c01*yi + c02
	* ui = ---------------------
	*      c20*xi + c21*yi + c22
	*
	*      c10*xi + c11*yi + c12
	* vi = ---------------------
	*      c20*xi + c21*yi + c22
	*
	* Coefficients are calculated by solving linear system:
	* / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
	* | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
	* | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
	* | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|,
	* |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
	* |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
	* |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
	* \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/
	*
	* where:
	*   cij - matrix coefficients, c22 = 1
	*
	*      ct00*ui + ct01*vi + ct02
	* xi = ---------------------
	*      ct20*ui + ct21*vi + ct22
	*
	*      ct10*ui + ct11*vi + ct12
	* yi = ---------------------
	*      ct20*ui + ct21*vi + ct22
	*
	*  ct[0][0] = m_warpMatrix[2][2]*m_warpMatrix[1][1] - m_warpMatrix[1][2]*m_warpMatrix[2][1];
	*  ct[0][1] = m_warpMatrix[2][1]*m_warpMatrix[0][2] - m_warpMatrix[2][2]*m_warpMatrix[0][1];
	*  ct[0][2] = m_warpMatrix[0][1]*m_warpMatrix[1][2] - m_warpMatrix[0][2]*m_warpMatrix[1][1];
	*
	*  ct[1][0] = m_warpMatrix[2][0]*m_warpMatrix[1][2] - m_warpMatrix[2][2]*m_warpMatrix[1][0];
	*  ct[1][1] = m_warpMatrix[2][2]*m_warpMatrix[0][0] - m_warpMatrix[2][0]*m_warpMatrix[0][2];
	*  ct[1][2] = m_warpMatrix[1][0]*m_warpMatrix[0][2] - m_warpMatrix[0][0]*m_warpMatrix[1][2];
	*
	*  ct[2][0] = m_warpMatrix[2][1]*m_warpMatrix[1][0] - m_warpMatrix[2][0]*m_warpMatrix[1][1];
	*  ct[2][1] = m_warpMatrix[2][0]*m_warpMatrix[0][1] - m_warpMatrix[0][0]*m_warpMatrix[2][1];
	*  ct[2][2] = m_warpMatrix[0][0]*m_warpMatrix[1][1] - m_warpMatrix[1][0]*m_warpMatrix[0][1];
	*
	*/


	const int	WARPMATRIX_SIZE = 9;

	#pragma pack(push,1)
	class CWarpMatrix
	{
	public:
		CWarpMatrix()
		{
			ClearMatrix();
		}		
		~CWarpMatrix()
		{
		}
		
	public:
		
		// 初始化扭曲环境，将一个矩形框映射到一个凸四边形中

		bool InitMatrix(const WarpRect& _warpRect, const CdRect& _rectSrc)
		{
			if (! _warpRect.IsConvexRect())
			{
				return false;
			}
	
			WarpRect warpRect = _warpRect;
			CdRect rectSrc = _rectSrc;
			//assert(rectSrc.TopLeft() == CdPoint(0,0));

			real k2, k3, k4;

			real delta = (warpRect.topRight.x -warpRect.bottomRight.x) * (warpRect.bottomLeft.y - warpRect.topRight.y) - (warpRect.topRight.y - warpRect.bottomRight.y) * (warpRect.bottomLeft.x - warpRect.topRight.x);
			if (D_Equal(delta, 0))
			{
				k2 = 0;
				k3 = 0;
				k4 = 0;
			}
			else
			{	
				k2 = ((warpRect.topLeft.x - warpRect.bottomRight.x) * (warpRect.bottomLeft.y - warpRect.topLeft.y) - (warpRect.bottomLeft.x - warpRect.topLeft.x) * (warpRect.topLeft.y - warpRect.bottomRight.y)) / delta;
				k3 = ((warpRect.topLeft.x - warpRect.topRight.x) * (warpRect.bottomLeft.y - warpRect.topRight.y) - (warpRect.bottomLeft.x - warpRect.topRight.x) * (warpRect.topLeft.y - warpRect.topRight.y)) / delta;
				k4 = ((warpRect.topRight.x - warpRect.bottomRight.x) * (warpRect.topLeft.y - warpRect.topRight.y) - (warpRect.topLeft.x - warpRect.topRight.x) * (warpRect.topRight.y - warpRect.bottomRight.y)) / delta;
			}

			if (D_Equal(rectSrc.Width(), 0))
			{
				m_warpMatrix[0][0] = 0;
				m_warpMatrix[1][0] = 0;
				m_warpMatrix[2][0] = 0;
			}
			else
			{	
				m_warpMatrix[0][0] = (k2 * warpRect.topRight.x - warpRect.topLeft.x) / rectSrc.Width();
				m_warpMatrix[1][0] = (k2 * warpRect.topRight.y - warpRect.topLeft.y) / rectSrc.Width();
				m_warpMatrix[2][0] = (k2 - 1) / rectSrc.Width();
			}

			if (D_Equal(rectSrc.Height(),0))
			{
				m_warpMatrix[0][1] = 0;
				m_warpMatrix[1][1] = 0;
				m_warpMatrix[2][1] = 0;
			}
			else
			{	
				m_warpMatrix[0][1] = (k3 * warpRect.bottomRight.x - k2 * warpRect.topRight.x) / rectSrc.Height();
				m_warpMatrix[1][1] = (k3 * warpRect.bottomRight.y - k2 * warpRect.topRight.y) / rectSrc.Height();
				m_warpMatrix[2][1] = (k3 - k2) / rectSrc.Height();
			}

			m_warpMatrix[0][2] = warpRect.topLeft.x - (m_warpMatrix[0][0] * rectSrc.left + m_warpMatrix[0][1] * rectSrc.top);
			m_warpMatrix[1][2] = warpRect.topLeft.y - (m_warpMatrix[1][0] * rectSrc.left + m_warpMatrix[1][1] * rectSrc.top);
			m_warpMatrix[2][2] = 1  - (m_warpMatrix[2][0] * rectSrc.left + m_warpMatrix[2][1] * rectSrc.top);

			ComputeIntWarpBackMatrix();

			return true;
		}

		bool IsIdentityMatrix()const
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (! D_Equal(m_warpMatrix[i][j], 0))
					{
						return false;
					}
				}
			}
			return true;
		}

		void ClearMatrix()
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					m_warpMatrix[i][j] = 0;
				}
			}
		}

		void WarpPoint(CdPoint& point)const
		{
			//if (IsIdentityMatrix())
			//{
			//	return;
			//}

			real k, x, y;

			if (D_LessThan(fabs(m_warpMatrix[2][0]), 0.00001f) &&
				D_LessThan(fabs(m_warpMatrix[2][1]), 0.00001f) &&
				D_LessThan(fabs(m_warpMatrix[2][2]-1), 0.00001f))
			{
				x = (real)(m_warpMatrix[0][0] * point.x + m_warpMatrix[0][1] * point.y + m_warpMatrix[0][2]);
				y = (real)(m_warpMatrix[1][0] * point.x + m_warpMatrix[1][1] * point.y + m_warpMatrix[1][2]);
				point.x = x;
				point.y = y;
			}
			else
			{
				k = (real)(m_warpMatrix[2][0] * point.x + m_warpMatrix[2][1] * point.y + m_warpMatrix[2][2]);
				if (D_LessEqual(fabs(k), 0.000001f))
				{
					return;
				}
				x = (real)((m_warpMatrix[0][0] * point.x + m_warpMatrix[0][1] * point.y + m_warpMatrix[0][2]) / k);
				y = (real)((m_warpMatrix[1][0] * point.x + m_warpMatrix[1][1] * point.y + m_warpMatrix[1][2]) / k);
				point.x = x;
				point.y = y;

			}
		}

		void WarpPoint(WarpRect& warpRect)const
		{
			std::vector<CdPoint> arrPointWarp;
			warpRect.GetPoints(arrPointWarp);

			WarpPoint(arrPointWarp);

			warpRect.topLeft = arrPointWarp[0];
			warpRect.topRight = arrPointWarp[1];
			warpRect.bottomRight = arrPointWarp[2];
			warpRect.bottomLeft = arrPointWarp[3];
		}

		void WarpPoint(std::vector<CdPoint>& arrPointWarp)const
		{
			//if (IsIdentityMatrix())
			//{
			//	return;
			//}

			std::vector<CdPoint>::iterator it;

			real k;
			real x, y;

			if (D_LessThan(fabs(m_warpMatrix[2][0]), 0.00001f) &&
				D_LessThan(fabs(m_warpMatrix[2][1]), 0.00001f) &&
				D_LessThan(fabs(m_warpMatrix[2][2]-1), 0.00001f))
			{
				for (it = arrPointWarp.begin(); it != arrPointWarp.end(); it++)
				{
					x = (real)(m_warpMatrix[0][0] * it->x + m_warpMatrix[0][1] * it->y + m_warpMatrix[0][2]);
					y = (real)(m_warpMatrix[1][0] * it->x + m_warpMatrix[1][1] * it->y + m_warpMatrix[1][2]);
					it->x = x;
					it->y = y;
				}
			}
			else
			{
				for (it = arrPointWarp.begin(); it != arrPointWarp.end(); it++)
				{
					k = (real)(m_warpMatrix[2][0] * it->x + m_warpMatrix[2][1] * it->y + m_warpMatrix[2][2]);
					if (D_LessEqual(fabs(k), 0.000001f))
					{
						continue;
					}
					x = (real)((m_warpMatrix[0][0] * it->x + m_warpMatrix[0][1] * it->y + m_warpMatrix[0][2]) / k);
					y = (real)((m_warpMatrix[1][0] * it->x + m_warpMatrix[1][1] * it->y + m_warpMatrix[1][2]) / k);
					it->x = x;
					it->y = y;
				}
			}
		}

		void WarpPointBack(CdPoint& point)const
		{
			//if (IsIdentityMatrix())
			//{
			//	return;
			//}
	
			real k, x, y;

			k = (real)((m_warpMatrix[2][0] * point.x - m_warpMatrix[0][0]) * (m_warpMatrix[2][1] * point.y - m_warpMatrix[1][1]) -
				(m_warpMatrix[2][0] * point.y - m_warpMatrix[1][0]) * (m_warpMatrix[2][1] * point.x - m_warpMatrix[0][1]));

			if (D_LessEqual(fabs(k), 0.000001f))
			{
				return;
			}

			x = (real)((m_warpMatrix[0][2] - m_warpMatrix[2][2] * point.x) * (m_warpMatrix[2][1] * point.y - m_warpMatrix[1][1]) -
				(m_warpMatrix[1][2] - m_warpMatrix[2][2] * point.y) * (m_warpMatrix[2][1] * point.x - m_warpMatrix[0][1]));

			y = (real)((m_warpMatrix[2][0] * point.x - m_warpMatrix[0][0]) * (m_warpMatrix[1][2] - m_warpMatrix[2][2] * point.y) -
				(m_warpMatrix[2][0] * point.y - m_warpMatrix[1][0]) * (m_warpMatrix[0][2] - m_warpMatrix[2][2] * point.x));

			point.x = x / k;
			point.y = y / k;
		}

		void FastWarpPointBack(CdPoint& point)const
		{
			__declspec(align(16)) real Vec[4] = {point.x,point.y,0,1.0f};

			real *pVec = Vec;

			__asm
			{
				mov ecx, [pVec];
				mov edx, [m_FastWarpMatrix];

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
				shufps xmm3,xmm0,0xaa				
				divps  xmm0,xmm3				
				movaps [ecx], xmm0
			}

			point.x = Vec[0];
			point.y = Vec[1];
		}

		void WarpPointBack(WarpRect& warpRect)const
		{
			std::vector<CdPoint> arrPointWarp;
			warpRect.GetPoints(arrPointWarp);

			WarpPointBack(arrPointWarp);

			warpRect.topLeft = arrPointWarp[0];
			warpRect.topRight = arrPointWarp[1];
			warpRect.bottomRight = arrPointWarp[2];
			warpRect.bottomLeft = arrPointWarp[3];
		}

		void WarpPointBack(std::vector<CdPoint>& arrPointWarp)const
		{
			//if (IsIdentityMatrix())
			//{
			//	return;
			//}

			std::vector<CdPoint>::iterator it;

			real k;
			real x, y;

			for (it = arrPointWarp.begin();	it != arrPointWarp.end(); it++)
			{
				k = (real)((m_warpMatrix[2][0] * it->x - m_warpMatrix[0][0]) * (m_warpMatrix[2][1] * it->y - m_warpMatrix[1][1]) -
					(m_warpMatrix[2][0] * it->y - m_warpMatrix[1][0]) * (m_warpMatrix[2][1] * it->x - m_warpMatrix[0][1]));
				if (D_LessEqual(fabs(k), 0.000001f))
				{
					continue;
				}

				x = (real)((m_warpMatrix[0][2] - m_warpMatrix[2][2] * it->x) * (m_warpMatrix[2][1] * it->y - m_warpMatrix[1][1]) -
					(m_warpMatrix[1][2] - m_warpMatrix[2][2] * it->y) * (m_warpMatrix[2][1] * it->x - m_warpMatrix[0][1]));
				y = (real)((m_warpMatrix[2][0] * it->x - m_warpMatrix[0][0]) * (m_warpMatrix[1][2] - m_warpMatrix[2][2] * it->y) -
					(m_warpMatrix[2][0] * it->y - m_warpMatrix[1][0]) * (m_warpMatrix[0][2] - m_warpMatrix[2][2] * it->x));
				it->x = x / k;
				it->y = y / k;
			}

		}

		void WarpSize(const Graph::CdPoint& pt, Graph::CdSize& dSize)const
		{
			Graph::CdPoint ptStart = pt;
			Graph::CdPoint ptStop = ptStart;
			ptStop.Offset(dSize);
			WarpPoint(ptStart);
			WarpPoint(ptStop);
			dSize = ptStop - ptStart;
		}

		void WarpSizeBack(const Graph::CdPoint& pt, Graph::CdSize& dSize)const
		{
			Graph::CdPoint ptStart = pt;
			Graph::CdPoint ptStop = ptStart;
			ptStop.Offset(dSize);
			WarpPointBack(ptStart);
			WarpPointBack(ptStop);
			dSize = ptStop - ptStart;
		}

		void GetWarpMatrix(real dMatrix[WARPMATRIX_SIZE], CdSize& size)const
		{
			if (dMatrix != NULL)
			{
				dMatrix[0] = m_warpMatrix[0][0];
				dMatrix[1] = m_warpMatrix[0][1];
				dMatrix[2] = m_warpMatrix[0][2];
				dMatrix[3] = m_warpMatrix[1][0];
				dMatrix[4] = m_warpMatrix[1][1];
				dMatrix[5] = m_warpMatrix[1][2];
				dMatrix[6] = m_warpMatrix[2][0];
				dMatrix[7] = m_warpMatrix[2][1];
				dMatrix[8] = m_warpMatrix[2][2];
			}

		}
		void SetWarpMatrix(real dMatrix[WARPMATRIX_SIZE], const CdSize& size)
		{
			if (dMatrix != NULL)
			{
				m_warpMatrix[0][0] = dMatrix[0];
				m_warpMatrix[0][1] = dMatrix[1];
				m_warpMatrix[0][2] = dMatrix[2];
				m_warpMatrix[1][0] = dMatrix[3];
				m_warpMatrix[1][1] = dMatrix[4];
				m_warpMatrix[1][2] = dMatrix[5];
				m_warpMatrix[2][0] = dMatrix[6];
				m_warpMatrix[2][1] = dMatrix[7];
				m_warpMatrix[2][2] = dMatrix[8];
			}
		}
	 
		//函数介绍：求逆矩阵（高斯—约当法） 
		BOOL InverseMatrix(const int &row)
		{
			real *m=new real[row*row];
			memcpy_s(m,sizeof(real)*row*row,m_warpMatrix,sizeof(real)*row*row);

			int i,j,k;
			int *is=new int[row],*js=new int[row];

			for (k=0;k<row;k++)
			{
				real max=0;
				//全选主元
				//寻找最大元素
				for (i=k;i<row;i++)
				{
					for (j=k;j<row;j++)
					{
						if (fabs(*(m+i*row+j))>max)
						{
							max=*(m+i*row+j);
							is[k]=i;
							js[k]=j;
						}
					}
				}

				if (0 == max)
				{
					return FALSE;
				}

				//行交换
				if (is[k]!=k)
				{
					for (i=0;i<row;i++)
					{
						swap(*(m+k*row+i),*(m+is[k]*row+i));
					}
				}

				//列交换
				if (js[k]!=k)
				{
					for (i=0;i<row;i++)
					{
						swap(*(m+i*row+k),*(m+i*row+js[k]));
					}
				}

				*(m+k*row+k)=1/(*(m+k*row+k));

				for (j=0;j<row;j++)
				{
					if (j!=k)
					{
						*(m+k*row+j)*=*((m+k*row+k));
					}
				}

				for (i=0;i<row;i++)
				{
					if (i!=k)
					{
						for (j=0;j<row;j++)
						{
							if(j!=k)
							{
								*(m+i*row+j)-=*(m+i*row+k)**(m+k*row+j);
							}
						}
					}
				}

				for (i=0;i<row;i++)
				{
					if(i!=k)
					{
						*(m+i*row+k)*=-(*(m+k*row+k));
					}
				}
			}

			int r;
			//恢复
			for (r=row-1;r>=0;r--)
			{
				if (js[r]!=r)
				{
					for (j=0;j<row;j++)
					{
						swap(*(m+r*row+j),*(m+js[r]*row+j));
					}
				}
				if (is[r]!=r)
				{
					for (i=0;i<row;i++)
					{
						swap(*(m+i*row+r),*(m+i*row+is[r]));
					}
				}
			}

			memcpy_s(m_warpBackMatrix,sizeof(real)*row*row,m,sizeof(real)*row*row);

			delete []is;
			delete []js;
			delete []m;

			return TRUE;
		}

		void ComputeIntWarpBackMatrix()
		{

			CMatrix mtxTemp(m_warpMatrix[2][2]*m_warpMatrix[1][1] - m_warpMatrix[1][2]*m_warpMatrix[2][1],m_warpMatrix[2][0]*m_warpMatrix[1][2] - m_warpMatrix[2][2]*m_warpMatrix[1][0],m_warpMatrix[2][1]*m_warpMatrix[1][0] - m_warpMatrix[2][0]*m_warpMatrix[1][1],0
			
				,m_warpMatrix[2][1]*m_warpMatrix[0][2] - m_warpMatrix[2][2]*m_warpMatrix[0][1],m_warpMatrix[2][2]*m_warpMatrix[0][0] - m_warpMatrix[2][0]*m_warpMatrix[0][2],m_warpMatrix[2][0]*m_warpMatrix[0][1] - m_warpMatrix[0][0]*m_warpMatrix[2][1],0
				,0,0,0,0
				,m_warpMatrix[0][1]*m_warpMatrix[1][2] - m_warpMatrix[0][2]*m_warpMatrix[1][1],m_warpMatrix[1][0]*m_warpMatrix[0][2] - m_warpMatrix[0][0]*m_warpMatrix[1][2],m_warpMatrix[0][0]*m_warpMatrix[1][1] - m_warpMatrix[1][0]*m_warpMatrix[0][1],0);
				

			m_FastWarpMatrix = mtxTemp;
		}
		
	public:
		real	m_warpMatrix[3][3],m_warpBackMatrix[3][3];

		CMatrix m_FastWarpMatrix;
	};
	#pragma pack(pop)
}

