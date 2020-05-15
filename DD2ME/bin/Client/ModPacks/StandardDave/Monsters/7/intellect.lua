function setFirstState()
	--setMonsterValue(-1, "activate", "0")
	return "rightrun"
end

function mainFunc()
	nextAdditionalNumberOfAction(-1)
	local activate = tonumber(getMonsterValue(-1, "activate"))
	if activate == 0 then
		if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstepghost") == 0 then
			nextNumberOfAction(-1)
		else
			return
		end
		if testCollisionDave(-1) == 1 then
			setMonsterValue(-1, "activate", "1")
			setNullNumberOfAction(-1)
			setNullAdditionalNumberOfAction(-1)
			setMonsterValue(-1, "stateba", getState(-1))
			setState(-1, "invisoff")
			return
		end
	end
	if getState(-1) == "invisoff" then
		if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstepinvisoff") == 0 then
			nextNumberOfAction(-1)
			if getMonsterFrame(-1) == 0 then
				local stateba = getMonsterValue(-1, "stateba")
				if string.find(stateba, "left") ~= nil then
					setState(-1, "leftrun")
				else
					setState(-1, "rightrun")
				end
			end
		else
			return
		end
	end
	local change = 1
	if getState(-1) == "invisoff" then
		change = 0
	end
	if change == 1 then
		if getDistanceToDaveXHead(-1, 1) > 0 then
			setState(-1, "rightrun")
		else
			setState(-1, "leftrun")
		end
	end
	if string.find(getState(-1), "run") ~= nil then
		--local speed = getMonsterOption(-1, "options", "speed")
		local speed = 4
		local calcspeed = getAdditionalNumberOfAction(-1) % 3
		if calcspeed == 2 then
			speed = 3
		end
		if getState(-1) == "rightrun" then
			goRight(-1, speed, 0)
		else
			if getState(-1) == "leftrun" then
				goLeft(-1, speed, 0)
			end
		end
		speed = 3
		if getDistanceToDaveYHead(-1, 1) > 0 then
			goDown(-1, speed, 0)
		else
			goUp(-1, speed, 0)
		end
	end
end