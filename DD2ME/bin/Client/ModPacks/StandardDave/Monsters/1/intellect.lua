function setFirstState()
	return "leftrun"
end

function onKill(type)
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
		return
	end
end

function mainFunc()
	local oldFrame = 0
	if string.find(getState(-1), "strike") ~= nil then
		oldFrame = getMonsterFrame(-1)
	end
	nextAdditionalNumberOfAction(-1)
	if string.find(getState(-1), "strike") == nil then
		if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
			nextNumberOfAction(-1)
		else
			return
		end
	else
		if getAdditionalNumberOfAction(-1) % (tonumber(getMonsterOption(-1, "other", "animationstep")) - 1) == 0 then
			nextNumberOfAction(-1)
		else
			return
		end
	end
	if getMonsterFrame(-1) == 2 and string.find(getState(-1), "strike") ~= nil and testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	local oldstate = getState(-1)
	local change = 1
	if getMonsterValue(-1, "counterDaveReaction") == "" then
		setMonsterValue(-1, "counterDaveReaction", "0")
	end
	if getMonsterValue(-1, "testdownrun") == "1" or getMonsterValue(-1, "testfirstdownrun") == "1" then
		change = 0
	end
	if testLookDaveX(-1) ~= 0 and getState(-1) == "downrun" then
		if getDistanceToDaveYHead(-1, 1) > 10 then
			change = 0
		end
	end
	if string.find(getState(-1), "strike") ~= nil then
		change = 0
	end
	local cdr = tonumber(getMonsterValue(-1, "counterDaveReaction"))
	if cdr > 0 then
		change = 0
		setMonsterValue(-1, "counterDaveReaction", tostring(cdr - 1))
	end
	if change == 1 then
		if testLookDaveX(-1) == 1 then
			setState(-1, "rightrun")
		else
			if testLookDaveX(-1) == -1 then
				setState(-1, "leftrun")
			end
		end
	end
	if testCollisionDave(-1) == 1 and getDistanceToDaveXHead(-1, 1) < 0 and getState(-1) ~= "downrun" then
		if string.find(getState(-1), "strike") == nil then
			setNullNumberOfAction(-1)
		end
		setState(-1, "leftstrike")
	else
		if testCollisionDave(-1) == 1 and getDistanceToDaveXHead(-1, 1) >= 0 and getState(-1) ~= "downrun" then
			if string.find(getState(-1), "strike") == nil then
				setNullNumberOfAction(-1)
			end
			setState(-1, "rightstrike")
		end
	end
	if getDistanceToDaveYHead(-1, 1) >= 16 and getDistanceToDave(-1) <= 16*6 and testTileTypeDown(-1, "LADDER", 0) == 1 and
				(testTileTypeDown(-1, "LADDER", 0, 1) == 1 or testTileTypeDown(-1, "IMPASSABLE", 0, 1) == 1) then
        if getState(-1) ~= "downrun" then
            setMonsterValue(-1, "testdownrun", "0")
            setMonsterValue(-1, "testfirstdownrun", "1")
        end
        setState(-1, "downrun")
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
		else
			if getState(-1) == "downrun" then
				if (getDistanceToDave(-1) <= 16*6 and getDistanceToDaveYHead(-1, 1) >= 16 and testTileTypeDown(-1, "IMPASSABLE", 1) == 0 ) or getMonsterValue(-1, "testdownrun") == "1" or getMonsterValue(-1, "testfirstdownrun") == "1" then
					testgo = goDown(-1, 8, tonumber(getMonsterValue(-1, "testdownrun")))
					setMonsterValue(-1, "testdownrun", tostring((tonumber(getMonsterValue(-1, "testdownrun")) + 1) % 2))
					setMonsterValue(-1, "testfirstdownrun", "0")
				else
					if getMonsterValue(-1, "testdownrun") == "0" and getMonsterValue(-1, "testfirstdownrun") == "0" then
						if testLookDaveX(-1) == -1 then
							setState(-1, "leftrun")
						else
							setState(-1, "rightrun")
						end
					end
				end
			end
		end
	end
	if testgo ~= 0 then
		if getState(-1) == "rightrun" then
			setState(-1, "leftrun")
			local start_rand = tonumber(getMonsterOption(-1, "options", "startRandomShiftPlayerReaction"))
			local end_rand = tonumber(getMonsterOption(-1, "options", "endRandomShiftPlayerReaction"))
			local mult = tonumber(getMonsterOption(-1, "options", "multipleRandomShiftPlayerReaction"))
			setMonsterValue(-1, "counterDaveReaction", tostring(mult*math.random(start_rand, end_rand)))
		else
			if getState(-1) == "leftrun" then
				setState(-1, "rightrun")
				local start_rand = tonumber(getMonsterOption(-1, "options", "startRandomShiftPlayerReaction"))
				local end_rand = tonumber(getMonsterOption(-1, "options", "endRandomShiftPlayerReaction"))
				local mult = tonumber(getMonsterOption(-1, "options", "multipleRandomShiftPlayerReaction"))
				setMonsterValue(-1, "counterDaveReaction", tostring(mult*math.random(start_rand, end_rand)))
			else
				if getState(-1) == "downrun" then
					if testLookDaveX(-1) == -1 then
						setState(-1, "leftrun")
					else
						setState(-1, "rightrun")
					end
				end
			end
		end
	end
	local newState = ""
	if oldFrame == 2 and string.find(getState(-1), "strike") ~= nil and testCollisionDave(-1) == 0 then
		newState = string.gsub(getState(-1), "strike", "run")
		setState(-1, newState)
	end
end

