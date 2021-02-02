#pragma once

#include <Runtime/CoreMacros.h>



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
		Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;


	};

}
