function setFirstState()
	return "rightrun"	
end

function mainFunc()
	if testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	if string.find(getState(-1), "prejump") ~= nil then
		local fbj = tonumber(getGlobalValue(-1, "freezeBeforeJump"))
		if fbj > 0 then
			setGlobalValue(-1, "freezeBeforeJump", tostring(fbj - 1))
		else
			if string.find(getState(-1), "left") then
				setState(-1, "leftjump")
			else
				setState(-1, "rightjump")
			end
			goUp(-1, 4, 1)
			setGlobalValue(-1, "jumpNow", "4")
		end
	else
		if string.find(getState(-1), "jump") ~= nil then
			local stopjumpstate = ""
			local shiftU = 5
			local shiftLR = 7
			if getState(-1) == "leftjump" then
				goLeft(-1, shiftLR, 1)
				stopjumpstate = "leftrun"
			else
				goRight(-1, shiftLR, 1)
				stopjumpstate = "rightrun"
			end
			local jn = tonumber(getGlobalValue(-1, "jumpNow"))
			if jn ~= 0 then
				if jn < 16 * 3 - 6 then
					jn = jn + shiftU
					local tstgoup = goUp(-1, shiftU, 1)
					if tstgoup ~= 0 then
						setGlobalValue(-1, "jumpNow", "0")
						setGlobalValue(-1, "freezeUpJump", "0")
					else
						setGlobalValue(-1, "jumpNow", tostring(jn))
					end
				else
					setGlobalValue(-1, "jumpNow", "0")
					setGlobalValue(-1, "freezeUpJump", "5")
				end
			else
				local fuj = tonumber(getGlobalValue(-1, "freezeUpJump"))
				if fuj > 0 then
					setGlobalValue(-1, "freezeUpJump", tostring(fuj - 1))
				else
					local tstgodown = goDown(-1, shiftU, 1, 0)
					if tstgodown ~= 0 then
						setState(-1, stopjumpstate)
						goUp(-1, 8, 0)
						setGlobalValue(-1, "counterDaveReaction", tostring(6))
					end
				end
			end
			return
		end
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterValue(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	local change = 0
	if getGlobalValue(-1, "counterDaveReaction") == "" then
		setGlobalValue(-1, "counterDaveReaction", "0")
	end
	if testLookDaveX(-1) ~= 0 and string.find(getState(-1), "run") ~= nil then
		change = 1
	end
	local cdr = tonumber(getGlobalValue(-1, "counterDaveReaction"))
	if cdr > 0 then
		local oldstate = getState(-1)
		if getDistanceToDaveXHead(-1) <= 0 and oldstate ~= "leftrun" or getDistanceToDaveXHead(-1) > 0 and oldstate ~= "rightrun" then
			change = 0
			setGlobalValue(-1, "counterDaveReaction", tostring(cdr - 1))
		end
	end
	if change == 1 then
		if testLookDaveX(-1) == 1 then
			setState(-1, "rightrun")
			if getDistanceToDaveXHead(-1) <= 7 * 16 then
				setState(-1, "rightprejump")
				local fbj = getMonsterValue(-1, "options", "freezeBeforeJump")
				setGlobalValue(-1, "freezeBeforeJump", fbj)
			end
		else
			if testLookDaveX(-1) == -1 then
				setState(-1, "leftrun")
				if getDistanceToDaveXHead(-1) <= 7 * 16 then
					setState(-1, "leftprejump")
					local fbj = getMonsterValue(-1, "options", "freezeBeforeJump")
					setGlobalValue(-1, "freezeBeforeJump", fbj)
				end
			end
		end
	end
	local speed = getMonsterValue(-1, "options", string.format("speed%d", getMonsterFrame(-1) + 1))
	local testgo = 0
	local typecorrect = 0
	if getState(-1) == "rightrun" then
		testgo, typecorrect = goRight(-1, speed, 1, 1)
	else
		if getState(-1) == "leftrun" then
			testgo, typecorrect = goLeft(-1, speed, 1, 1)
		end
	end
	if testgo ~= 0 then
		if getState(-1) == "rightrun" then
			if typecorrect == 2 then
				setState(-1, "rightprejump")
				goRight(-1, 16, 0)
				local fbj = getMonsterValue(-1, "options", "freezeBeforeJump")
				setGlobalValue(-1, "freezeBeforeJump", fbj)
			else
				setState(-1, "leftrun")
			end
		else
			if getState(-1) == "leftrun" then
				if typecorrect == 2 then
					setState(-1, "leftprejump")
					goLeft(-1, 16, 0)
					local fbj = getMonsterValue(-1, "options", "freezeBeforeJump")
					setGlobalValue(-1, "freezeBeforeJump", fbj)
				else
					setState(-1, "rightrun")
				end
			end
		end
	end
end