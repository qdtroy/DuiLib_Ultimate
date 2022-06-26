#pragma once

#include "..\Graph\Graph_Vector.h"
using namespace Graph;

namespace Motion
{
	// 直线运动
	class CMotion
	{
	public:
		CMotion()
		{

		}

		~CMotion()
		{

		}

		void SetVelocity(const Vector &velocity)
		{
			m_velocity = velocity;
		}

		const Vector GetVelocity()
		{
			return m_velocity;
		}
	
		void SetAcceleration(const Vector &acceleration)
		{
			m_acceleration = acceleration;
		}

		const Vector GetAcceleration()
		{
			return m_acceleration;
		}

		void SetPosition(const Vector &position)
		{
			m_position = position;
		}

		const Vector GetPosition()
		{
			return m_position;
		}

		// 获得tTime 时刻物体的位置
		virtual const Vector GetNowPosition(real tTime) = 0;
	
		// 获得tTime 时刻物体的速度
		virtual const Vector GetNowVelocity(real tTime) = 0;


	protected:
		// 初始速度加速度和位置
		Vector m_velocity;
		Vector m_acceleration;
		Vector m_position;
	};

	// 匀速直线运动
	class CUniformMotion : public CMotion
	{
		// 获得tTime 时刻物体的位置
		virtual const Vector GetNowPosition(real tTime)
		{
			return Vector(m_position+m_velocity*tTime);
		}

		// 获得tTime 时刻物体的速度
		virtual const Vector GetNowVelocity(real tTime)
		{
			return m_velocity;
		}
	};

	// 匀加速直线运动
	class CUniformAccelerationMotion : public CMotion
	{
		// 获得tTime 时刻物体的位置
		virtual const Vector GetNowPosition(real tTime)
		{
			return Vector(m_position + m_velocity*tTime + m_acceleration*tTime*tTime*0.5f);
		}

		// 获得tTime 时刻物体的速度
		virtual const Vector GetNowVelocity(real tTime)
		{
			return Vector(m_velocity + m_acceleration*tTime);
		}
	};

	// 变加速直线运动
	class CVaryAccelerationMotion : public CMotion
	{
		// 获得tTime 时刻物体的位置
		virtual const Vector GetNowPosition(real tTime)
		{
			// acceleration 为 -velocity
			return Vector(m_position + m_velocity*tTime*tTime*0.5f*(1-tTime/3));
		}

		// 获得tTime 时刻物体的速度
		virtual const Vector GetNowVelocity(real tTime)
		{
			return Vector(m_velocity*(1-tTime*tTime));
		}
	};
	
	//阻尼振动
	class CDamp
	{
	public:
		// 获得tTime 时刻物体的振幅
		virtual real GetNowAmplitude(real tTime) = 0;

	public:
		CDamp()
		{
			m_amplitude = 40.0f;
			m_damp = 0.8f;
			m_angularFrequency = 2.0f;
			m_phase = 0.0f;

		}

		CDamp(real amplitude,real damp,real angularFrequency,real phase)
		{
			m_amplitude = amplitude;
			m_damp = damp;
			m_angularFrequency = angularFrequency;
			m_phase = phase;
		}

		void SetAmplitude(real amplitude)
		{
			m_amplitude = amplitude;
		}

		real GetAmplitude() const
		{
			return m_amplitude;
		}

		void SetDamp(real damp)
		{
			m_damp = damp;
		}

		real GetDamp() const
		{
			return m_damp;
		}

		void SetAngularFrequency(real angularFrequency)
		{
			m_angularFrequency = angularFrequency;
		}

		real GetAngularFrequency() const
		{
			return m_angularFrequency;
		}

		void SetPhase(real phase)
		{
			m_phase = phase;
		}

		real GetPhase() const
		{
			return m_phase;
		}

	protected:
		real m_amplitude;			//振幅
		real m_damp;				//阻尼因子
		real m_angularFrequency;	//固有角频率
		real m_phase;				//初始相位
	};

	// 欠阻尼运动(β<ω)
	//x(t) = A * exp(-β*t) * cos(sqrt(ω^2 - β^2) * t + φ) , β是阻尼因子，ω是固有角频率，A是振幅，φ初始相位（为方便计算可以等于0）
	//振幅随时间的推移，呈指数递减， β越大，振动衰减越快；反之，振幅衰减越慢
	class CUnderDamp : public CDamp
	{
	public:
		CUnderDamp()
		{
			m_amplitude = 40.0f;
			m_damp = 0.8f;
			m_angularFrequency = 2.0f;
			m_phase = 0.0f;
		}

	public:	
		real GetNowAmplitude(real tTime)
		{
			return m_amplitude*exp(-m_damp*tTime)*cos(sqrt(m_angularFrequency*m_angularFrequency-m_damp*m_damp)*tTime + m_phase);
		}
	
	};

	// 过阻尼运动(β>ω)
	//x(t) = A * exp(-t*(β-sqrt(β^2-ω^2)) + B * exp(-t*(β+sqrt(β^2-ω^2)) ,
	//随时间的推移，质点坐标单调地趋于零。质点运动是非周期的不往复的。将质点移开平衡位置后释放，质点便慢慢回到平衡位置停下来
	class COverDamp : public CDamp
	{
	public:
		COverDamp()
		{
			m_amplitudeA = m_amplitudeB = m_amplitude;
			m_damp = 0.4f;
			m_angularFrequency = 0.30f;
			m_phase = 0.0f;
		}

		void SetAmplitudeA(real amplitudeA)
		{
			m_amplitudeA = amplitudeA;
		}

		real GetAmplitudeA() const
		{
			return m_amplitudeA;
		}

		void SetAmplitudeB(real amplitudeB)
		{
			m_amplitudeB = amplitudeB;
		}

		real GetAmplitude() const
		{
			return m_amplitudeB;
		}
	public:	
		real GetNowAmplitude(real tTime)
		{
			return m_amplitudeA*exp(-tTime*(m_damp-sqrt(m_damp*m_damp-m_angularFrequency*m_angularFrequency)))
				+ m_amplitudeB*exp(-tTime*(m_damp+sqrt(m_damp*m_damp-m_angularFrequency*m_angularFrequency)));
		}

	protected:
		real m_amplitudeA,m_amplitudeB; //初始两个振幅（初始条件：开始，结束）

	};
	
	// 临界阻尼运动(β=ω)
	//x(t) = (A + B*t) * exp(-β*t)
	//由于阻力较前者小，质点移开平衡位置释放后，质点很快回到平衡位置并停下来
	class CCriticalDamp : public CDamp
	{
	public:
		CCriticalDamp()
		{
			m_amplitudeA = m_amplitudeB = m_amplitude;
			m_damp = 1.0f;
			m_angularFrequency = 1.0f;
			m_phase = 0.0f;
		}

		void SetAmplitudeA(real amplitudeA)
		{
			m_amplitudeA = amplitudeA;
		}

		real GetAmplitudeA() const
		{
			return m_amplitudeA;
		}

		void SetAmplitudeB(real amplitudeB)
		{
			m_amplitudeB = amplitudeB;
		}

		real GetAmplitude() const
		{
			return m_amplitudeB;
		}

	public:	
		real GetNowAmplitude(real tTime)
		{
			return (m_amplitudeA+m_amplitudeB*tTime)*exp(-m_damp*tTime);
		}

	protected:
		real m_amplitudeA,m_amplitudeB; //初始两个振幅（初始条件：开始，结束）

	};

}