#pragma once
#include <Insight/Core.h>

#include <chrono>

namespace Insight {

	namespace Profiling {

		struct INSIGHT_API ScopedTimer
		{
			enum class eOutputType
			{
				SECONDS,
				MILISECONDS
			};

			ScopedTimer(const char* pScopeName, eOutputType outputType = eOutputType::MILISECONDS)
				: m_ScopeName(pScopeName), m_OutputType(outputType)
			{
				start = std::chrono::high_resolution_clock::now();
			}

			~ScopedTimer()
			{
				end = std::chrono::high_resolution_clock::now();
				duration = end - start;

				float time = 0.0f;
				switch (m_OutputType) {
				case eOutputType::SECONDS:
				{
					time = duration.count();
					if (time > 1.0f) {
						IE_CORE_WARN("{0} took {1}s. Performance degradation.", m_ScopeName, time);
					}
					else {
						IE_CORE_INFO("{0} took {1}s", m_ScopeName, time);
					}
				}
				case eOutputType::MILISECONDS:
				{
					time = duration.count() * 1000.0f;
					if (time > 1000.0f) {
						IE_CORE_WARN("{0} took {1}s. Performance degradation.", m_ScopeName, time);
					}
					else {
						IE_CORE_INFO("{0} took {1}ms", m_ScopeName, time);
					}
				}
				}

			}
		private:
			std::chrono::time_point<std::chrono::steady_clock> start, end;
			std::chrono::duration<float> duration;

			std::string m_ScopeName;
			eOutputType m_OutputType;
		};

	} // End namespace Profiling

}// End namspace Insight