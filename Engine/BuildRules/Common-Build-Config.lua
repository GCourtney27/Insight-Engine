
-- Build Configurations

macros = {}
macros["debug"]		= "IE_DEBUG"
macros["debugEd"]   = "IE_DEBUG_EDITOR"
macros["withEd"]	= "IE_WITH_EDITOR"
macros["dev"]		= "IE_DEVELOPMENT"
macros["shipping"]	= "IE_SHIPPING"

filter "configurations:DebugEditor"
defines { "%{macros.debug}=1", "%{macros.withEd}=1", "%{macros.debugEd}=1" }
runtime ("Debug")
symbols ("On")
optimize ("Off")

filter "configurations:Development"
defines { "%{macros.dev}=1", "%{macros.debug}=1", "%{macros.withEd}=1"}
runtime ("Release")
symbols ("On")
optimize ("Off")

filter "configurations:DebugGame"
defines { "%{macros.dev}=1", "%{macros.debug}=1" }
runtime ("Release")
symbols ("On")
optimize ("On")

filter "configurations:ShippingGame"
defines { "%{macros.shipping}=1" }
runtime ("Release")
symbols ("Off")
optimize ("On")

