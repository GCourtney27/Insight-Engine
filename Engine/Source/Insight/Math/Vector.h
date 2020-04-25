#ifndef VECTOR_H
#define VECTOR_H

#include <Insight/Core.h>

#include <DirectXMath.h>

namespace Insight {

#define Vector3_WorldForward WorldForward
#define Vector3_WorldBackward WorldBackward
#define Vector3_WorldLeft WorldLeft
#define Vector3_WorldRight WorldRight
#define Vector3_WorldUp WorldUp
#define Vector3_WorldDown WorldDown

	using namespace DirectX;

	class INSIGHT_API Vector
	{
	public:

	protected:
		virtual ~Vector() {}
		Vector() {}
	};

	static const XMVECTOR WorldForward	= XMVectorSet( 0.0f,  0.0f,  1.0f, 0.0f);
	static const XMVECTOR WorldBackward	= XMVectorSet( 0.0f,  0.0f, -1.0f, 0.0f);
	static const XMVECTOR WorldLeft		= XMVectorSet(-1.0f,  0.0f,  0.0f, 0.0f);
	static const XMVECTOR WorldRight	= XMVectorSet( 1.0f,  0.0f,  0.0f, 0.0f);
	static const XMVECTOR WorldUp		= XMVectorSet( 0.0f,  1.0f,  0.0f, 0.0f);
	static const XMVECTOR WorldDown		= XMVectorSet( 0.0f, -1.0f,  0.0f, 0.0f);

	class INSIGHT_API Vector3 : public Vector
	{
	public:
		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
			: m_Data(x, y, z) 
		{
			m_DataVector = XMLoadFloat3(&m_Data);
		}

		virtual ~Vector3() {}

		static const XMVECTOR WorldForward;
		static const XMVECTOR WorldBackward;
		static const XMVECTOR WorldLeft;
		static const XMVECTOR WorldRight;
		static const XMVECTOR WorldUp;
		static const XMVECTOR WorldDown;

		inline float GetX() const { return m_Data.x; }
		inline float GetY() const { return m_Data.y; }
		inline float GetZ() const { return m_Data.z; }
		inline float& GetXRef() { return m_Data.x; }
		inline float& GetYRef() { return m_Data.y; }
		inline float& GetZRef() { return m_Data.z; }

		inline void SetX(float& x) { m_Data.x = x; UpdateVector(); }
		inline void SetY(float& y) { m_Data.y = y; UpdateVector(); }
		inline void SetZ(float& z) { m_Data.z = z; UpdateVector(); }

		Vector3 operator+(Vector3 const& vec)
		{
			Vector3 newVec(this->m_Data.x + vec.m_Data.x,
							this->m_Data.y + vec.m_Data.y, 
							this->m_Data.z + vec.m_Data.z);
			return newVec;
		}

	private:
		inline void UpdateVector() { m_DataVector = XMLoadFloat3(&m_Data); ; }
	private:
		XMFLOAT3 m_Data;
		XMVECTOR m_DataVector;
	};

}

#endif // !VECTOR_H
