function setFirstState()
	setState(-1, "leftrunceiling")
	testgo = goRight(-1, 1, 1)
	if testgo == 0 then
		goLeft(-1, 1, 0)
	end
	testgo = goUp(-1, 1, 1)
	if testgo == 0 then
		goDown(-1, 1, 0)
		setState(-1, "leftrunfloor")
		goDown(-1, 8, 1)
		testgo = goDown(-1, 1, 1)
		if testgo == 0 then
			goUp(-1, 1, 0)
			goUp(-1, 8, 0)
			return "leftdownrun"
		end
		return "leftrunfloor"
	end
	return "leftrunceiling"
end

function onKill(type)
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
		return
	end
end

function changeDirection()
	if string.find(getState(-1), "rightrun") ~= nil then
		setState(-1, string.format("%s%s", "leftrun", string.gsub(getState(-1), "rightrun", "") ) )
	else
		if string.find(getState(-1), "leftrun") ~= nil then
			setState(-1, string.format("%s%s", "rightrun", string.gsub(getState(-1), "leftrun", "") ) )
		end
	end
end

function setJump()
	setNullAdditionalNumberOfAction(-1)
	setNullNumberOfAction(-1)
	local NewState = ""
	if string.find(getState(-1), "floor") ~= nil then
		NewState = string.gsub(getState(-1), "runfloor", "upattack")
		setState(-1, NewState)
		goUp(-1, 8, 0)
	end
	if string.find(getState(-1), "ceiling") ~= nil then
		NewState = string.gsub(getState(-1), "runceiling", "downattack")
		setState(-1, NewState)
	end
end

function mainFunc()
	if testCollisionPlayer(-1) == 1 then
		killPlayer(-1)
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	else
		return
	end
	local NewState = ""
	if string.find(getState(-1), "attack") ~= nil and getAdditionalNumberOfAction(-1) > 4*getMonsterOption(-1, "other", "animationstep") then
		NewState = string.gsub(getState(-1), "attack", "run")
		setState(-1, NewState)
	end
	local oldstate = getState(-1)
	if math.random(10) == 1 and (string.find(getState(-1), "leftrun") ~= nil or string.find(getState(-1), "rightrun") ~= nil) then
		if getDistanceToPlayerX(-1, 1) < 0 then
			if string.find(getState(-1), "rightrun") ~= nil then
				changeDirection()
				return
			end
		else
			if string.find(getState(-1), "leftrun") ~= nil then
				changeDirection()
				return
			end
		end
	end
	if math.random(10) == 1 and math.abs( getDistanceToPlayerX(-1) - getDistanceToPlayerY(-1) ) < 16 and (string.find(getState(-1), "leftrun") ~= nil or string.find(getState(-1), "rightrun") ~= nil) then
		if getDistanceToPlayerX(-1, 1) < 0 then
			if string.find(getState(-1), "rightrun") ~= nil then
				changeDirection()
				return
			else
				if math.random(10) == 1 then
					setJump()
				end
				return
			end
		else
			if string.find(getState(-1), "leftrun") ~= nil then
				changeDirection()
				return
			else
				if math.random(10) == 1 then
					setJump()
				end
				return
			end
		end
	end
	if math.random(10) == 1 and (string.find(getState(-1), "leftrun") ~= nil or string.find(getState(-1), "rightrun") ~= nil) then
		changeDirection()
	else
		if math.random(20) == 1 and (string.find(getState(-1), "leftrun") ~= nil or string.find(getState(-1), "rightrun") ~= nil) then
			setJump()
		end
	end
	if oldstate ~= getState(-1) then
		return
	end
	local speed = getMonsterOption(-1, "options", string.format("speed%d", getMonsterFrame(-1) + 1))
	local testgo = 0
	if string.find(getState(-1), "rightrun") ~= nil then
		if string.find(getState(-1), "ceiling") ~= nil then
			testgo = goRight(-1, speed, 1, 1, 1)
		else
			testgo = goRight(-1, speed, 1, 1, 0)
		end
	else
		if string.find(getState(-1), "leftrun") ~= nil then
			if string.find(getState(-1), "ceiling") ~= nil then
				testgo = goLeft(-1, speed, 1, 1, 1)
			else
				testgo = goLeft(-1, speed, 1, 1, 0)
			end
		end
	end
	local speedX = getMonsterOption(-1, "options", "speedflyX")
	local speedY = getMonsterOption(-1, "options", "speedflyY")
	if string.find(getState(-1), "downrun") ~= nil or string.find(getState(-1), "uprun") ~= nil then
		if string.find(getState(-1), "left") ~= nil then
			goLeft(-1, speedX, 1, 0)
		else
			goRight(-1, speedX, 1, 0)
		end
		if string.find(getState(-1), "downrun") ~= nil then
			testgo = goDown(-1, speedY, 1)
		else
			testgo = goUp(-1, speedY, 1)
		end
		if testgo ~= 0 then
			if string.find(getState(-1), "down") ~= nil then
				NewState = string.gsub(getState(-1), "down", "")
				NewState = string.format("%sfloor", NewState)
				setState(-1, NewState)
				goDown(-1, 8, 1)
				playSound("lambla")
			else
				NewState = string.gsub(getState(-1), "up", "")
				NewState = string.format("%sceiling", NewState)
				setState(-1, NewState)
				playSound("lambla")
			end
		end
	end
	if testgo ~= 0 then
		changeDirection()
	end
end
