function setFirstState()
	setMonsterValue(-1, "timeopenmouth", "31")
	setMonsterValue(-1, "timeaction", "64")
	setMonsterValue(-1, "lhand", "0")
	setMonsterValue(-1, "rhand", "0")
	setMonsterValue(-1, "freeze", "0")
	setGlobalValue(-1, getNV(), "")
	setMonsterValue(-1, "firsthandscreate", "0")
	return "rightrun"
end

function getNV()
	return string.format("testDeath%s", tostring(getMonsterID()))
end

function onKill(type)
	if type == 1 then
		return
	end
	if getState(-1) == "hand" then
		local idBoss = tonumber(getMonsterValue(-1, "idBoss"))
		setMonsterValue(idBoss, getMonsterValue(-1, "tp"), "0")
		if type == 0 then
			lux, luy, rdx, rdy = getMonsterCollision(-1)
			addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumpsfromhands"), lux, luy, rdx, rdy))
		end
		return
	end
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
	end
	setGlobalValue(-1, "goodfriendfreedom", "1")
	setGlobalValue(-1, getNV(), "1")
end

function calculateHand()
	local nv = getMonsterValue(-1, "nv")
	local testdeath = getGlobalValue(-1, nv)
	if testdeath ~= "" then
		killMonster(-1, 1)
	end
	local freeze = tonumber(getMonsterValue(-1, "freeze"))
	if freeze > 0 then
		freeze = freeze - 1
		setMonsterValue(-1, "freeze", tostring(freeze))
		return
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstephand") == 0 then
		nextNumberOfAction(-1)
	else
		if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
			return
		end
	end
	local lstate = 0
	if getDistanceToDaveXHead(-1, 1) > 0 then
		lstate = 1
	else
		lstate = -1
	end
	local distanceFromFloor = tonumber(getMonsterOption(-1, "options", "distanceFromFloorHands"))
	if getDistanceToDaveYHead(-1, 1) > distanceFromFloor then
		local speedDown = tonumber(getMonsterOption(-1, "options", "speedDownHands"))
		goDown(-1, speedDown, 0)
	else
		local speedUp = tonumber(getMonsterOption(-1, "options", "speedUpHands"))
		goUp(-1, speedUp, 0)
	end
	local tp = getMonsterValue(-1, "tp")
	if lstate == 1 then
		local speedRight = tonumber(getMonsterOption(-1, "options", "speedRightHands"))
		if tp == "lhand" and getAdditionalNumberOfAction(-1) % 3 ~= 0 then
			speedRight = speedRight - 1
		end
		goRight(-1, speedRight, 0)
	else
		if lstate == -1 then
			local speedLeft = tonumber(getMonsterOption(-1, "options", "speedLeftHands"))
			if tp == "lhand" and getAdditionalNumberOfAction(-1) % 3 ~= 0 then
				speedLeft = speedLeft - 1
			end
			goLeft(-1, speedLeft, 0)
		end
	end
end

function calculateFireball()
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstepfireball") == 0 then
		nextNumberOfAction(-1)
	end
	local timelife = tonumber(getMonsterValue(-1, "timelife"))
	timelife = timelife - 1
	setMonsterValue(-1, "timelife", tostring(timelife))
	if timelife <= 0 then
		killMonster(-1, 1)
	end
	local tfly = tonumber(getMonsterValue(-1, "tfly"))
	if tfly == 1 then
		goDown(-1, 4, 0)
		goLeft(-1, 4, 0)
	end
	if tfly == 2 then
		goDown(-1, 4, 0)
		goLeft(-1, 2, 0)
	end
	if tfly == 3 then
		goDown(-1, 4, 0)
		goRight(-1, 2, 0)
	end
	if tfly == 4 then
		goDown(-1, 4, 0)
		goRight(-1, 4, 0)
	end
end

function createFireballs()
	addDuplicateMonster(-1, getCoordMonsterX(-1) - 12, getCoordMonsterY(-1) + 55, "fireball", 0, 0, -2, "timelife=32;tfly=1;")
	addDuplicateMonster(-1, getCoordMonsterX(-1) + 12, getCoordMonsterY(-1) + 67, "fireball", 0, 0, -2, "timelife=32;tfly=2;")
	addDuplicateMonster(-1, getCoordMonsterX(-1) + 36, getCoordMonsterY(-1) + 67, "fireball", 0, 0, -2, "timelife=32;tfly=3;")
	addDuplicateMonster(-1, getCoordMonsterX(-1) + 59, getCoordMonsterY(-1) + 55, "fireball", 0, 0, -2, "timelife=32;tfly=4;")
end

function createHands()
	local rh = tonumber(getMonsterValue(-1, "rhand"))
	local lh = tonumber(getMonsterValue(-1, "lhand"))
	if rh + lh < 2 then
		setMonsterValue(-1, "freeze", "32")
		local fhc = tonumber(getMonsterValue(-1, "firsthandscreate"))
		local createrighthand = 1
		local createlefthand = 1
		if fhc > 0 then
			if rh + lh == 0 then
				if math.random(1, 2) == 1 then
					createlefthand = 0
				else
					createrighthand = 0
				end
			end
		else
			setMonsterValue(-1, "firsthandscreate", "1")
		end
		if rh == 0 and createrighthand == 1 then
			setMonsterValue(-1, "rhand", "1")
			addDuplicateMonster(-1, getCoordMonsterX(-1) - 5, getCoordMonsterY(-1) + 55, "hand", 0, 0, 2, string.format("tp=rhand;freeze=32;idBoss=%s;nv=%s;", tostring(getMonsterID()), getNV()))
		end
		if lh == 0 and createlefthand == 1 then
			setMonsterValue(-1, "lhand", "1")
			addDuplicateMonster(-1, getCoordMonsterX(-1) + 52, getCoordMonsterY(-1) + 55, "hand", 0, 0, 2, string.format("tp=lhand;freeze=32;idBoss=%s;nv=%s;", tostring(getMonsterID()), getNV()))
		end
	end
end

function mainFunc()
	if testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	local statem = getState(-1)
	if statem == "hand" then
		calculateHand()
		return
	else
		if statem == "fireball" then
			calculateFireball()
			return
		end
	end
	local freeze = tonumber(getMonsterValue(-1, "freeze"))
	if freeze > 0 then
		freeze = freeze - 1
		setMonsterValue(-1, "freeze", tonumber(freeze))
		return
	end
	if statem == "openmouth" then
		local attackCounter = tonumber(getMonsterValue(-1, "attackCounter"))
		attackCounter = attackCounter - 1
		if attackCounter <= 0 then
			createFireballs()
			attackCounter = 0
			local newstate = getMonsterValue(-1, "oldstate")
			setState(-1, newstate)
		end
		setMonsterValue(-1, "attackCounter", tostring(attackCounter))
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		if statem == "openmouth" then
			nextNumberOfAction(-1)
		else
			return
		end
	end
	local timeom = tonumber(getMonsterValue(-1, "timeopenmouth"))
	local timeaction = tonumber(getMonsterValue(-1, "timeaction"))
	if string.find(statem, "run") ~= nil then
		timeaction = timeaction - 1
		if timeaction == 15 then
			createHands()
		end
		if timeaction <= 0 then
			timeaction = 96
			setMonsterValue(-1, "oldstate", statem)
			setState(-1, "openmouth")
			setMonsterValue(-1, "attackCounter", "32")
			playSound("finalboss")
		end
		setMonsterValue(-1, "timeaction", tostring(timeaction))
	end
	if string.find(statem, "run") ~= nil then
		if timeom > 0 then
			timeom = timeom - 1
			if timeom % 16 == 0 then
				nextNumberOfAction(-1)
			end
			setMonsterValue(-1, "timeopenmouth", tostring(timeom))
		else
			if math.random(1, 64) == 1 then
				nextNumberOfAction(-1)
				setMonsterValue(-1, "timeopenmouth", tostring(16+32*2))
			end
		end
	end
	local currentState = getState(-1)
	local testgo = 0
	if currentState == "rightrun" then
		local speedRight = getMonsterOption(-1, "options", "speedRight")
		testgo = goRight(-1, speedRight, 1)
		if testgo ~= 0 then
			setState(-1, "leftrun")
		end
	else
		if currentState == "leftrun" then
			local speedLeft = getMonsterOption(-1, "options", "speedLeft")
			testgo = goLeft(-1, speedLeft, 1)
			if testgo ~= 0 then
				setState(-1, "rightrun")
			end
		end
	end
end