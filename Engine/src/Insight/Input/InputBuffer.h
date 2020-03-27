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

	protected:
		InputBuffer() {}
		virtual ~InputBuffer() {}
	};

}