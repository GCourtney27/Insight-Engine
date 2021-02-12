
-- Build Configurations

filter "configurations:DebugEditor"
	defines { "IE_DEBUG=1", "IE_WITH_EDITOR=1" }
	runtime ("Debug")
	symbols ("On")
	optimize ("Off")

filter "configurations:Development"
	defines { "IE_DEVELOPMENT=1", "IE_DEBUG=1", "IE_WITH_EDITOR=1"}
	runtime ("Release")
	symbols ("On")
	optimize ("Off")

filter "configurations:DebugGame"
	defines { "IE_DEVELOPMENT=1" }
	runtime ("Release")
	symbols ("On")
	optimize ("On")

filter "configurations:ShippingGame"
	defines { "IE_SHIPPING=1" }
	runtime ("Release")
	symbols ("Off")
	optimize ("On")

