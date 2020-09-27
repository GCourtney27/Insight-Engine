#pragma once

namespace Retina {

	class InputBuffer
	{
	public:
		enum KeyPressState
		{
			PRESSED,
			RELEASED
		};
		enum MouseScrollState
		{
			SCROLL_UP,
			SCROLL_DOWN,
			NONE
		};

	protected:
		InputBuffer() {}
		virtual ~InputBuffer() {}
	};

}