#pragma once
#include <Insight/Core.h>

#include <chrono>


#if defined IE_SCOPE_PROFILING_ENABLED
#define ScopedPerfTimer(Message, OutputType) Insight::Profiling::ScopedTimer Timer(Message, Insight::Profiling::ScopedTimer::eOutputType::##OutputType);
#else
	#define ScopedPerfTimer
#endif

namespace Insight {

	namespace Profiling {

		struct INSIGHT_API ScopedTimer
		{
			enum class eOutputType
			{
				eOutputType_Seconds,
				eOutputType_Millis
			};

			ScopedTimer(const char* pScopeName, eOutputType outputType = eOutputType::eOutputType_Millis)
				: m_ScopeName(pScopeName), m_OutputType(outputType)
			{
				m_Start = std::chrono::high_resolution_clock::now();
			}

			~ScopedTimer()
			{
				m_End = std::chrono::high_resolution_clock::now();
				m_Duration = m_End - m_Start;

				float time = 0.0f;
				switch (m_OutputType) {
				case eOutputType::eOutputType_Millis:
				{
					time = m_Duration.count();
					if (time > 1.0f) {
						IE_CORE_WARN("{0} took {1}ms. Performance degradation.", m_ScopeName, time);
					}
					else {
						IE_CORE_INFO("{0} took {1}s", m_ScopeName, time);
					}
				}
				case eOutputType::eOutputType_Seconds:
				{
					time = m_Duration.count() * 1000.0f;
					if (time > 1000.0f) {
						IE_CORE_WARN("{0} took {1}s. Performance degradation.", m_ScopeName, time);
					}
					else {
						IE_CORE_INFO("{0} took {1}s", m_ScopeName, time);
					}
				}
				}

			}
		private:
			std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
			std::chrono::duration<float> m_Duration;

			std::string m_ScopeName;
			eOutputType m_OutputType;
		};

	} // End namespace Profiling

}// End namspace Insight