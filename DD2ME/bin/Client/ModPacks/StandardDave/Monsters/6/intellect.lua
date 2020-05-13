function setFirstState()
	setMonsterValue(-1, "step", "10")
	setMonsterValue(-1, "freeze", "0")
	return "leftrun"
end

function initStar()
	setMonsterValue(-1, "timer", "50")
	setMonsterValue(-1, "dir", "?")
end

function calculateStar()

	getMonsterValue(-1, "timer")
	getMonsterValue(-1, "dir")
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	end
end

function mainFunc()
	if testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	if getState(-1) == "star" then
		calculateStar()
		return
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	local numbstep = tonumber(getMonsterValue(-1, "step"))
	numbstep = numbstep - 1
	local freeze = tonumber(getMonsterValue(-1, "freeze"))
	if freeze == 1 then
		numbstep = 15
	end
	if freeze > 0 then
		freeze = freeze - 1
	end
	setMonsterValue(-1, "freeze", tostring(freeze))
	if freeze > 0 then
		setNumberOfAction(-1, getNumberOfAction(-1) - 1)
		return
	end
	local testgo = 0
	if getState(-1) == "rightrun" then
		local speed = getMonsterOption(-1, "options", string.format("speed%dright", getMonsterFrame(-1) + 1))
		testgo = goRight(-1, speed, 1, 1)
	else
		if getState(-1) == "leftrun" then
			local speed = getMonsterOption(-1, "options", string.format("speed%dleft", getMonsterFrame(-1) + 1))
			testgo = goLeft(-1, speed, 1, 1)
		end
	end
	if testgo ~= 0 then
		if getState(-1) == "rightrun" then
			setState(-1, "leftrun")
		else
			if getState(-1) == "leftrun" then
				setState(-1, "rightrun")
			end
		end
	end
	if numbstep == 0 then
		if math.fmod(getMonsterFrame(-1) + 1, 2) == 1 then
			local curstate = getState(-1)
			local params = "timer=20;"
			if string.find(curstate, "left") ~= nil then
				addDuplicateMonster(-1, getCoordMonsterX(-1) + 10, getCoordMonsterY(-1) + 15, "star", 0, 0, 1, string.format("%s;dir=1;", params))
			else
				if string.find(curstate, "right") ~= nil then
					addDuplicateMonster(-1, getCoordMonsterX(-1) + 30, getCoordMonsterY(-1) + 15, "star", 0, 0, 1, string.format("%s;dir=2;", params))
				end
			end
			setMonsterValue(-1, "freeze", "9")
		else
			numbstep = 1
		end
	end
	setMonsterValue(-1, "step", tostring(numbstep))
end