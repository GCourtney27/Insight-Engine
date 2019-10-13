
function Update(deltaTime)
	--AdjustPosition(0.0, 0.11 * deltaTime, 0.0)
	AdjustRotation(0.0, 0.11 * deltaTime, 0.0)
	--AdjustScale(0.0, 0.11 * deltaTime, 0.0)


	-- // Input Works but only through ASCII values of the keys you want pressed -- \\
	if(OnKeyPressed(80))
	then
		--DebugLog("Pressed ")
		--AdjustRotation(0.0, 0.11 * deltaTime, 0.0)
	end
	--DebugLog("bool value: "..val..".")
	

end

