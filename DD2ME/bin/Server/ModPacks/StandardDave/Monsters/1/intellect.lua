function setFirstState()
	return "rightrun"
end

function mainFunc()
	local oldFrame = 0
	if string.find(getState(-1), "strike") ~= nil then
		oldFrame = getMonsterFrame(-1)
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterValue(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	if getMonsterFrame(-1) == 2 and string.find(getState(-1), "strike") ~= nil and testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	local oldstate = getState(-1)
	local change = 1
	if getGlobalValue(-1, "counterDaveReaction") == "" then
		setGlobalValue(-1, "counterDaveReaction", "0")
	end
	if getGlobalValue(-1, "testdownrun") == "1" or getGlobalValue(-1, "testfirstdownrun") == "1" then
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
	local cdr = tonumber(getGlobalValue(-1, "counterDaveReaction"))
	if cdr > 0 then
		change = 0
		setGlobalValue(-1, "counterDaveReaction", tostring(cdr - 1))
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
            setGlobalValue(-1, "testdownrun", "0")
            setGlobalValue(-1, "testfirstdownrun", "1")
        end
        setState(-1, "downrun")
	end
	if oldstate ~= getState(-1) then
		return
	end
	local speed = getMonsterValue(-1, "options", string.format("speed%d", getMonsterFrame(-1) + 1))
	local testgo = 0
	if getState(-1) == "rightrun" then
		testgo = goRight(-1, speed, 1, 1)
	else
		if getState(-1) == "leftrun" then
			testgo = goLeft(-1, speed, 1, 1)
		else
			if getState(-1) == "downrun" then
				if (getDistanceToDave(-1) <= 16*6 and getDistanceToDaveYHead(-1, 1) >= 16) or getGlobalValue(-1, "testdownrun") == "1" or getGlobalValue(-1, "testfirstdownrun") == "1" then
					testgo = goDown(-1, 8, 1)
					setGlobalValue(-1, "testdownrun", tostring((tonumber(getGlobalValue(-1, "testdownrun")) + 1) % 2))
					setGlobalValue(-1, "testfirstdownrun", "0")
				else
					if getGlobalValue(-1, "testdownrun") == "0" and getGlobalValue(-1, "testfirstdownrun") == "0" then
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
			setGlobalValue(-1, "counterDaveReaction", tostring(2*math.random(1, 4)))
		else
			if getState(-1) == "leftrun" then
				setState(-1, "rightrun")
				setGlobalValue(-1, "counterDaveReaction", tostring(2*math.random(1,4)))
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

