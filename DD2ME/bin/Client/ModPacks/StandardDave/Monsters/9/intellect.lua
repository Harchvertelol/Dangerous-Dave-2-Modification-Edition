function setFirstState()
	return "rightrun"
end

function mainFunc()
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	if getDistanceToPlayerXHead(-1, 1) > 0 then
		setState(-1, "rightrun")
	else
		setState(-1, "leftrun")
	end
	local distanceFromFloor = tonumber(getMonsterOption(-1, "options", "distanceFromFloor"))
	if getDistanceToPlayerYHead(-1, 1) > distanceFromFloor then
		local speedDown = getMonsterOption(-1, "options", "speedDown")
		goDown(-1, speedDown, 1)
	else
		local speedUp = getMonsterOption(-1, "options", "speedUp")
		goUp(-1, speedUp, 1)
	end
	local currentState = getState(-1)
	if currentState == "rightrun" then
		local speedRight = getMonsterOption(-1, "options", "speedRight")
		goRight(-1, speedRight, 1)
	else
		if currentState == "leftrun" then
			local speedLeft = getMonsterOption(-1, "options", "speedLeft")
			goLeft(-1, speedLeft, 1)
		end
	end
	if testCollisionPlayer(-1) == 1 then
		killPlayer(-1)
	end
end