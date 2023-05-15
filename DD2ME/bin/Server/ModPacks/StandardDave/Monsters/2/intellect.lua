function setFirstState()
	return "rightrun"
end

function onKill(type)
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
		return
	end
end

function calculateKnife()
	local testgo = -1
	if getState(-1) == "leftknife" then
		testgo = goLeft(-1, 5, 1, 0)
	else
		if getState(-1) == "rightknife" then
			testgo = goRight(-1, 5, 1, 0)
		end
	end
	if testgo == 0 and testCollisionPlayer(-1) == 1 then
		killPlayer(-1)
	end
	if testgo ~= 0 then
		setState(-1, "traceshoot")
		setNullNumberOfAction(-1)
		setNullAdditionalNumberOfAction(-1)
	end
end

function mainFunc()
	if string.find(getState(-1), "knife") ~= nil then
		calculateKnife(-1)
		return
	end
	if getState(-1) == "traceshoot" and getNumberOfAction(-1) == 1 then
		killMonster(-1, 1)
	end
	local oldFrame = 0
	if string.find(getState(-1), "strike") ~= nil then
		oldFrame = getMonsterFrame(-1)
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	if getState(-1) == "traceshoot" then
		return
	end
	if getMonsterFrame(-1) == 2 and string.find(getState(-1), "strike") ~= nil then
		if getState(-1) == "leftstrike" then
			addDuplicateMonster(-1, getCoordMonsterX(-1), getCoordMonsterY(-1), "leftknife", 0, 0, -2)
			playSound("knife")
			return
		else
			if getState(-1) == "rightstrike" then
				if math.random(5) ~= 1 then
					addDuplicateMonster(-1, getCoordMonsterX(-1) + 34, getCoordMonsterY(-1), "rightknife", 0, 0, -2)
					playSound("knife")
				end
				return
			end
		end
	end
	local oldstate = getState(-1)
	if math.random(50) == 1 then
		if testLookPlayerX(-1) == -1 and getDistanceToPlayerXHead(-1, 1) < -16*5 then
			if string.find(getState(-1), "strike") == nil then
				setNullNumberOfAction(-1)
			end
			setState(-1, "leftstrike")
		else
			if testLookPlayerX(-1) == 1 and getDistanceToPlayerXHead(-1, 1) > 16*5 then
				if string.find(getState(-1), "strike") == nil then
					setNullNumberOfAction(-1)
				end
				setState(-1, "rightstrike")
			end
		end
	end
	if oldstate ~= getState(-1) then
		return
	end
	local speed = getMonsterOption(-1, "options", string.format("speed%d", getMonsterFrame(-1) + 1))
	local testgo = 0
	if getState(-1) == "rightrun" then
		testgo = goRight(-1, speed, 1, 1)
	else
		if getState(-1) == "leftrun" then
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
	local newState = ""
	if oldFrame == 2 and string.find(getState(-1), "strike") ~= nil then
		newState = string.gsub(getState(-1), "strike", "run")
		setState(-1, newState)
	end
end
