#pragma once

namespace Insight {

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
			SCROLL_DOWN
		};

	protected:
		InputBuffer() {}
		virtual ~InputBuffer() {}
	};

}