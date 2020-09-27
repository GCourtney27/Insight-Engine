#pragma once

#include <Retina/Core.h>

using Microsoft::WRL::ComPtr;

namespace Retina {


	class RETINA_API D3D11Helper
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
