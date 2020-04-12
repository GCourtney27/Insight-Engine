#ifndef VECTOR_H
#define VECTOR_H

#include <Insight/Core.h>

#include <DirectXMath.h>

namespace Insight {

	using namespace DirectX;

	class INSIGHT_API Vector
	{
	protected:
		virtual ~Vector() {}
		Vector() {}
	};

	class INSIGHT_API Vector3 : public Vector
	{
	public:
		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
			: m_Data(x, y, z) 
		{
			m_DataVector = XMLoadFloat3(&m_Data);
		}

		virtual ~Vector3() {}

		const XMVECTOR WorldForward	  = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR WorldBackward  = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const XMVECTOR WorldLeft	  = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR WorldRight	  = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR WorldUp		  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR WorldDown	  = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		inline float GetX() const { return m_Data.x; }
		inline float GetY() const { return m_Data.y; }
		inline float GetZ() const { return m_Data.z; }
		inline float& GetXRef() { return m_Data.x; }
		inline float& GetYRef() { return m_Data.y; }
		inline float& GetZRef() { return m_Data.z; }

		inline void SetX(float& x) { m_Data.x = x; }
		inline void SetY(float& y) { m_Data.y = y; }
		inline void SetZ(float& z) { m_Data.z = z; }

		Vector3 operator+(Vector3 const& vec)
		{
			Vector3 newVec(this->m_Data.x + vec.m_Data.x,
							this->m_Data.y + vec.m_Data.y, 
							this->m_Data.z + vec.m_Data.z);
			return newVec;
		}

	private:
		XMFLOAT3 m_Data;
		XMVECTOR m_DataVector;
	};

}

#endif // !VECTOR_H
