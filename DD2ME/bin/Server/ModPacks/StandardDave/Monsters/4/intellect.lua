function setFirstState()
	return "leftrun"
end

function changeDirection()
	if getState(-1) == "rightrun" then
		setState(-1, "leftrun")
	else
		if getState(-1) == "leftrun" then
			setState(-1, "rightrun")
		end
	end
end

function mainFunc()
	if testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	local speedattack = getMonsterValue(-1, "options", "speedattack")
	local noa = -1
	local when = -1
	if getState(-1) == "attack" then
		noa = getGlobalValue(-1, "numberofattack")
		when = getGlobalValue(-1, "when")
		if when == "1" and noa*speedattack > 16*6 then
			setGlobalValue(-1, "when", "-1")
			when = -1
			return
		end
		if when == "1" then
			noa = noa + 1
		else
			noa = noa - 1
		end
		if noa == 0 then
			setState(-1, "leftrun")
			setGlobalValue(-1, "noattack", "10")
		end
		setGlobalValue(-1, "numberofattack", noa)
		if when == "1" then
			addImageToFactoryTemporaryImage("monsters", 4, "web", 1, getCoordMonsterX(-1) + 4, getCoordMonsterY(-1) + 0, 2*(16*6/speedattack - noa) + 4, 0, 0, "web")
			goDown(-1, speedattack, 0)
		else
			goUp(-1, speedattack, 0)
		end
	end
	local noattack = getGlobalValue(-1, "noattack")
	if noattack == "" then
		noattack = 0
		setGlobalValue(-1, "noattack", noattack)
	end
	noattack = noattack + 1
	noattack = noattack - 1
	if noattack ~= 0 then
		noattack = noattack - 1
		setGlobalValue(-1, "noattack", noattack)
	end
	if (getState(-1) == "leftrun" or getState(-1) == "rightrun") and noattack == 0 and testLookDaveY(-1) == 1 and getDistanceToDaveXHead(-1, 1) <= 8 and getDistanceToDaveY(-1, 1) > 4 and getDistanceToDaveY(-1, 1) < 16*6 then
		setState(-1, "attack")
		setGlobalValue(-1, "numberofattack", "0")
		setGlobalValue(-1, "when", "1")
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterValue(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	local oldstate = getState(-1)
	if (string.find(getState(-1), "leftrun") ~= nil or string.find(getState(-1), "rightrun") ~= nil) and getDistanceToDave(-1) < 32*16 then
		if getDistanceToDaveXHead(-1, 1) < 4 then
			if string.find(getState(-1), "rightrun") ~= nil then
				changeDirection(-1)
				return
			end
		else
			if string.find(getState(-1), "leftrun") ~= nil then
				changeDirection(-1)
				return
			end
		end
	end
	if oldstate ~= getState(-1) then
		return
	end
	local speed = getMonsterValue(-1, "options", string.format("speed%d", getMonsterFrame(-1) + 1))
	if getState(-1) == "leftrun" then
		goLeft(-1, speed, 1, 1, 1)
	else
		if getState(-1) == "rightrun" then
			goRight(-1, speed, 1, 1, 1)
		end
	end
	if testgo ~= 0 then
		changeDirection()
	end
end
