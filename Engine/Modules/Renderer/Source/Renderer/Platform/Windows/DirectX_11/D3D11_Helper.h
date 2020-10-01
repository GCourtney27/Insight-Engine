#pragma once

#include <Insight/Core.h>

using Microsoft::WRL::ComPtr;

namespace Insight {


	class INSIGHT_API D3D11Helper
	{
	public:
		D3D11Helper(HWND hWindow, uint32_t Width, uint32_t Height);
		~D3D11Helper();

		bool Init();

	protected:
		void CreateDXGIFactory();

	private:
		ComPtr<IDXGIFactory> pFactory;


	};

}
