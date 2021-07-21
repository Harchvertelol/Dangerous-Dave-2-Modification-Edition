function setFirstState()
	setMonsterValue(-1, "timerDenyJump", "0")
	return "rightrun"	
end

function onKill(type)
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
		return
	end
end

function mainFunc()
	if testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	if string.find(getState(-1), "prejump") ~= nil then
		local fbj = tonumber(getMonsterValue(-1, "freezeBeforeJump"))
		if fbj > 0 then
			setMonsterValue(-1, "freezeBeforeJump", tostring(fbj - 1))
		else
			if string.find(getState(-1), "left") then
				setState(-1, "leftjump")
			else
				setState(-1, "rightjump")
				goLeft(-1, 8, 0)
				goRight(-1, 8, 1)
			end
			setMonsterValue(-1, "coordXJ", tostring(getCoordMonsterX(-1)))
			setMonsterValue(-1, "coordYJ", tostring(getCoordMonsterY(-1)))
			goUp(-1, 4, 1)
			setMonsterValue(-1, "jumpNow", "4")
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
			local jumax = 16 * 3 - 6
			local jn = tonumber(getMonsterValue(-1, "jumpNow"))
			if jn ~= 0 then
				if jn < jumax then
					shiftU = shiftU + math.floor((jumax - jn) / 16)
					jn = jn + shiftU
					local tstgoup = goUp(-1, shiftU, 1)
					if tstgoup ~= 0 then
						setMonsterValue(-1, "jumpNow", "0")
						setMonsterValue(-1, "freezeUpJump", "0")
					else
						setMonsterValue(-1, "jumpNow", tostring(jn))
					end
				else
					setMonsterValue(-1, "jumpNow", "0")
					setMonsterValue(-1, "freezeUpJump", "5")
				end
			else
				local fuj = tonumber(getMonsterValue(-1, "freezeUpJump"))
				if fuj > 0 then
					setMonsterValue(-1, "freezeUpJump", tostring(fuj - 1))
				else
					local tstgodown = goDown(-1, shiftU, 1)
					if tstgodown ~= 0 then
						setState(-1, stopjumpstate)
						goUp(-1, 8, 1)
						if string.find(stopjumpstate, "right") ~= nil then
							goRight(-1, 8, 1)
						end
						setMonsterValue(-1, "counterDaveReaction", tostring(5))
						local xold = tonumber(getMonsterValue(-1, "coordXJ"))
						local yold = tonumber(getMonsterValue(-1, "coordYJ"))
						local xnow = getCoordMonsterX(-1)
						local ynow = getCoordMonsterY(-1)
						if (xold - xnow)^2 + (yold - ynow)^2 < (16 * 2)^2 then
							setMonsterValue(-1, "timerDenyJump", "2")
						end
					end
				end
			end
			return
		end
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	local change = 0
	if getMonsterValue(-1, "counterDaveReaction") == "" then
		setMonsterValue(-1, "counterDaveReaction", "0")
	end
	if testLookDaveX(-1) ~= 0 and string.find(getState(-1), "run") ~= nil then
		change = 1
	end
	local timerdenyjump = tonumber(getMonsterValue(-1, "timerDenyJump"))
	if timerdenyjump > 0 then
		timerdenyjump = timerdenyjump - 1
		setMonsterValue(-1, "timerDenyJump", tostring(timerdenyjump))
	end
	local cdr = tonumber(getMonsterValue(-1, "counterDaveReaction"))
	if cdr > 0 then
		local oldstate = getState(-1)
		if getDistanceToDaveXHead(-1, 1) <= 0 and oldstate ~= "leftrun" or getDistanceToDaveXHead(-1, 1) > 0 and oldstate ~= "rightrun" then
			change = 0
			setMonsterValue(-1, "counterDaveReaction", tostring(cdr - 1))
		else
			setMonsterValue(-1, "counterDaveReaction", "0")
		end
	end
	if change == 1 then
		if testLookDaveX(-1) == 1 then
			setState(-1, "rightrun")
			if getDistanceToDaveXHead(-1) <= 7 * 16 then
				setState(-1, "rightprejump")
				local fbj = getMonsterOption(-1, "options", "freezeBeforeJump")
				setMonsterValue(-1, "freezeBeforeJump", fbj)
			end
		else
			if testLookDaveX(-1) == -1 then
				setState(-1, "leftrun")
				if getDistanceToDaveXHead(-1) <= 6 * 16 then
					setState(-1, "leftprejump")
					local fbj = getMonsterOption(-1, "options", "freezeBeforeJump")
					setMonsterValue(-1, "freezeBeforeJump", fbj)
				end
			end
		end
	end
	local speed = getMonsterOption(-1, "options", string.format("speed%d", getMonsterFrame(-1) + 1))
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
			if typecorrect == 2 and timerdenyjump == 0 then
				setState(-1, "rightprejump")
				goRight(-1, 16, 1)
				local fbj = getMonsterOption(-1, "options", "freezeBeforeJump")
				setMonsterValue(-1, "freezeBeforeJump", fbj)
			else
				if testLookDaveX(-1) == 1 and typecorrect == 1 and timerdenyjump == 0 then
					setState(-1, "rightprejump")
					local fbj = getMonsterOption(-1, "options", "freezeBeforeJump")
					setMonsterValue(-1, "freezeBeforeJump", fbj)
				else
					setState(-1, "leftrun")
				end
			end
		else
			if getState(-1) == "leftrun" then
				if typecorrect == 2 and timerdenyjump == 0 then
					setState(-1, "leftprejump")
					goLeft(-1, 16, 1)
					local fbj = getMonsterOption(-1, "options", "freezeBeforeJump")
					setMonsterValue(-1, "freezeBeforeJump", fbj)
				else
					if testLookDaveX(-1) == -1 and typecorrect == 1 and timerdenyjump == 0 then
						setState(-1, "leftprejump")
						local fbj = getMonsterOption(-1, "options", "freezeBeforeJump")
						setMonsterValue(-1, "freezeBeforeJump", fbj)
					else
						setState(-1, "rightrun")
					end
				end
			end
		end
	end
end