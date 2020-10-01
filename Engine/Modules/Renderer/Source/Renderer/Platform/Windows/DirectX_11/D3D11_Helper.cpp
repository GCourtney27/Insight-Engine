#include <Renderer_pch.h>

#include "D3D11_Helper.h"

namespace Insight {



	D3D11Helper::D3D11Helper(HWND hWindow, uint32_t Width, uint32_t Height)
	{
	}

	D3D11Helper::~D3D11Helper()
	{
	}

	bool D3D11Helper::Init()
	{
		CreateDXGIFactory();

		return true;
	}

	void D3D11Helper::CreateDXGIFactory()
	{

	}

}
