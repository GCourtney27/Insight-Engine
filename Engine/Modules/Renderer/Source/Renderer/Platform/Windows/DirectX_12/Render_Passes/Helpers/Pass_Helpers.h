#pragma once

#include <Insight/Core.h>

namespace Insight {

	class ThresholdDownSampleHelper
	{
	public:
		ThresholdDownSampleHelper() = default;
		~ThresholdDownSampleHelper() = default;

	protected:


	private:
		ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	};

	class GaussianBlurHelper
	{
	public:
		GaussianBlurHelper() = default;
		~GaussianBlurHelper() = default;

	protected:

	private:
		ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	};

}
