#pragma once

#include <Insight/Core.h>
#include <DirectX12/TK/Inc/SimpleMath.h>

namespace Insight {

	namespace Math {
		
		/*
			Lightweight platform independent vector library.
			Meant to serve as shader resources for a graphics API,
			avoid using for general math operations.
		*/

		// Vector 2 with two 32-bit floating point components
		struct ieFloat2
		{
			float x;
			float y;

			ieFloat2() = default;

			ieFloat2(const ieFloat2&) = default;
			ieFloat2& operator=(const ieFloat2&) = default;

			ieFloat2(ieFloat2&&) = default;
			ieFloat2& operator=(ieFloat2&&) = default;

			constexpr ieFloat2(float _X, float _Y)
				: x(_X), y(_Y) {}
		};

		// Vector 3 with three 32-bit floating point components
		struct ieFloat3
		{
			float x;
			float y;
			float z;

			ieFloat3() = default;

			ieFloat3(const ieFloat3&) = default;
			ieFloat3& operator=(const ieFloat3&) = default;

			ieFloat3(ieFloat3&&) = default;
			ieFloat3& operator=(ieFloat3&&) = default;

			constexpr ieFloat3(float _X, float _Y, float _Z)
				: x(_X), y(_Y), z(_Z) {}
		};

		// Vector 4 with four 32-bit floating point components
		struct ieFloat4
		{
			float x;
			float y;
			float z;
			float w;

			ieFloat4() = default;

			ieFloat4(const ieFloat4&) = default;
			ieFloat4& operator=(const ieFloat4&) = default;

			ieFloat4(ieFloat4&&) = default;
			ieFloat4& operator=(ieFloat4&&) = default;

			constexpr ieFloat4(float _X, float _Y, float _Z, float _W)
				: x(_X), y(_Y), z(_Z), w(_W) {}
		};

	} // End namespace Math

	namespace Math {

		/*
			Standard vectors for general math operations. Avoid using API
			specific vector libraries as inter-using different math libraries
			my cause issues when compiling for other platforms. 
			Use ieVectors instead.
		*/

#if defined IE_PLATFORM_WINDOWS
		using ieVector2 = DirectX::SimpleMath::Vector2;
		using ieVector3 = DirectX::SimpleMath::Vector3;
		using ieVector4 = DirectX::SimpleMath::Vector4;
#elif defined IE_PLATFORM_MAC
		using ieVector2 = glm::vec2;
		using ieVector3 = glm::vec3;
		using ieVector4 = glm::vec4;
#endif // IE_PLATFORM_WINDOWS


		namespace Vector2 {
			constexpr ieVector2 Zero{ 0.0f, 0.0f };
			constexpr ieVector2 One{ 1.0f, 1.0f };
		}

		namespace Vector3 {
			constexpr ieVector3 Up{ 0.0f,  1.0f,  0.0f };
			constexpr ieVector3 Down{ 0.0f, -1.0f,  0.0f };
			constexpr ieVector3 Left{ -1.0f,  0.0f,  0.0f };
			constexpr ieVector3 Right{ 1.0f,  0.0f,  0.0f };
			constexpr ieVector3 Forward{ 0.0f,  0.0f,  1.0f };
			constexpr ieVector3 Backward{ 0.0f,  0.0f, -1.0f };

			constexpr ieVector3 Zero{ 0.0f, 0.0f, 0.0f };
			constexpr ieVector3 One{ 1.0f, 1.0f, 1.0f };
		}

		namespace Vector4 {

			constexpr ieVector4 Zero{ 0.0f, 0.0f, 0.0f, 0.0f };
			constexpr ieVector4 One{ 1.0f, 1.0f, 1.0f, 1.0f };
		}

	} // End namespace Math

}// End namespace Insight
