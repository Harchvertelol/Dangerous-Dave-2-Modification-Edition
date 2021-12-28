function setFirstState()
	setMonsterValue(-1, "step", "12")
	setMonsterValue(-1, "freeze", "0")
	setGlobalValue(-1, "standard_first_boss_service_values", getNV(), "")
	return "leftrun"
end

function getNV()
	return string.format("testDeath%s", tostring(getMonsterID()))
end

function onKill(type)
	if type == 1 then
		return
	end
	if getState(-1) == "star" then
		addDuplicateMonster(-1, getCoordMonsterX(-1) + 8, getCoordMonsterY(-1) + 8, "starclump", 0, 0, -2, string.format("dir=1;timelive=%s;GS_type=standard;", getMonsterOption(-1, "other", "timelivestarclump")))
		addDuplicateMonster(-1, getCoordMonsterX(-1) + 8, getCoordMonsterY(-1) + 8, "starclump", 0, 0, -2, string.format("dir=2;timelive=%s;GS_type=standard;", getMonsterOption(-1, "other", "timelivestarclump")))
		local idMainBoss = tonumber(getMonsterValue(-1, "imb"))
		local lives = tonumber(getNumberOfLives(idMainBoss))
		if lives > 0 then
			lives = lives - 1
			if lives == 0 then
				killMonster(idMainBoss, 0, 1)
			else
				setNumberOfLives(idMainBoss, lives)
			end
		end
		return
	end
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
	end
	setGlobalValue(-1, "standard_first_boss_service_values", getNV(), "1")
end


function calculateStar()
	local nv = getMonsterValue(-1, "nv")
	local testdeath = getGlobalValue(-1, "standard_first_boss_service_values", nv)
	if testdeath ~= "" then
		killMonster(-1, 1)
		return
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstepstar") == 0 then
		nextNumberOfAction(-1)
	end
	local freeze = tonumber(getMonsterValue(-1, "freeze"))
	freeze = freeze - 1
	if freeze > 0 then
		setMonsterValue(-1, "freeze", tostring(freeze))
		return
	end
	local timer = tonumber(getMonsterValue(-1, "timer"))
	local dir = tonumber(getMonsterValue(-1, "dir"))
	local testgoUD = 0
	local testgoLR = 0
	local shift = 3
	if dir == 1 then
		testgoUD = goUp(-1, shift, 1)
		testgoLR = goRight(-1, shift, 1)
	end
	if dir == 2 then
		testgoUD = goDown(-1, shift, 1)
		testgoLR = goRight(-1, shift, 1)
	end
	if dir == 3 then
		testgoUD = goDown(-1, shift, 1)
		testgoLR = goLeft(-1, shift, 1)
	end
	if dir == 4 then
		testgoUD = goUp(-1, shift, 1)
		testgoLR = goLeft(-1, shift, 1)
	end
	local olddir = dir
	if testgoUD == 1 then
		if olddir == 1 then
			dir = 2
		end
		if olddir == 2 then
			dir = 1
		end
		if olddir == 3 then
			dir = 4
		end
		if olddir == 4 then
			dir = 3
		end
	end
	if testgoLR == 1 then
		if olddir == 1 then
			dir = 4
		end
		if olddir == 2 then
			dir = 3
		end
		if olddir == 3 then
			dir = 2
		end
		if olddir == 4 then
			dir = 1
		end
	end
	setMonsterValue(-1, "dir", tostring(dir))
	if testgoLR + testgoUD > 0 then
		timer = timer - 1
	end
	if timer == 0 then
		killMonster(-1, 1)
		return
	end
	setMonsterValue(-1, "timer", tostring(timer))
end

function calculateStarClump()
	timelive = getMonsterValue(-1, "timelive")
	timelive = timelive - 1
	if timelive == 0 then
		killMonster(-1, 1)
		return
	end
	setMonsterValue(-1, "timelive", timelive)
	dir = getMonsterValue(-1, "dir")
	speedclumpLR = tonumber(getMonsterOption(-1, "options", "speedclump"))
	speedclumpUD = getMonsterValue(-1, "speedclumpUD")
	if speedclumpUD == "" then
		speedclumpUD = speedclumpLR
	else
		speedclumpUD = tonumber(speedclumpUD)
	end
	if dir == "1" then
		goUp(-1, speedclumpUD, 0)
		goLeft(-1, speedclumpLR, 0)
	end
	if dir == "2" then
		goUp(-1, speedclumpUD, 0)
		goRight(-1, speedclumpLR, 0)
	end
	if dir == "3" then
		goDown(-1, speedclumpUD, 0)
		goRight(-1, speedclumpLR, 0)
	end
	if dir == "4" then
		goDown(-1, speedclumpUD, 0)
		goLeft(-1, speedclumpLR, 0)
	end
	changeUDspeedclump = tonumber(getMonsterOption(-1, "options", "changeUDSpeedClump"))
	if tonumber(dir) < 3 then
		speedclumpUD = speedclumpUD - changeUDspeedclump
	else
		speedclumpUD = speedclumpUD + changeUDspeedclump
		if speedclumpUD > speedclumpLR then
			speedclumpUD = speedclumpLR
		end
	end
	if speedclumpUD <= 0 then
		if dir == "1" then
			setMonsterValue(-1, "dir", "4")
		else
			if dir == "2" then
				setMonsterValue(-1, "dir", "3")
			end
		end
		speedclumpUD = 0
	end
	setMonsterValue(-1, "speedclumpUD", speedclumpUD)
	return
end

function mainFunc()
	if testCollisionDave(-1) == 1 then
		killDave(-1)
	end
	if getState(-1) == "star" then
		calculateStar()
		return
	end
	if getState(-1) == "starclump" then
		calculateStarClump()
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
		numbstep = 16
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
		local speed = tonumber(getMonsterOption(-1, "options", string.format("speed%dright", getMonsterFrame(-1) + 1)))
		local correctionspeed = tonumber(getMonsterOption(-1, "options", string.format("speed%drightcorrection", getMonsterFrame(-1) + 1)))
		if correctionspeed > 0 then
			goLeft(-1, correctionspeed, 0)
		else
			goRight(-1, -correctionspeed, 0)
		end
		testgo = goRight(-1, speed + correctionspeed, 1, 1)
	else
		if getState(-1) == "leftrun" then
			local speed = tonumber(getMonsterOption(-1, "options", string.format("speed%dleft", getMonsterFrame(-1) + 1)))
			local correctionspeed = tonumber(getMonsterOption(-1, "options", string.format("speed%dleftcorrection", getMonsterFrame(-1) + 1)))
			if correctionspeed > 0 then
				goRight(-1, correctionspeed, 0)
			else
				goLeft(-1, -correctionspeed, 0)
			end
			testgo = goLeft(-1, speed + correctionspeed, 1, 1)
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
		if (getMonsterFrame(-1) + 1) % 2 == 1 then
			local curstate = getState(-1)
			local params = string.format("timer=13;freeze=15;nv=%s;imb=%s;", getNV(), tostring(getMonsterID()))
			if string.find(curstate, "left") ~= nil then
				addDuplicateMonster(-1, getCoordMonsterX(-1) + 10, getCoordMonsterY(-1) + 15, "star", 0, 0, 1, string.format("%sdir=3;GS_no_points=true;GS_type=standard;", params))
			else
				if string.find(curstate, "right") ~= nil then
					addDuplicateMonster(-1, getCoordMonsterX(-1) + 30, getCoordMonsterY(-1) + 15, "star", 0, 0, 1, string.format("%sdir=2;GS_no_points=true;GS_type=standard;", params))
				end
			end
			setMonsterValue(-1, "freeze", "8")
		else
			numbstep = 1
		end
	end
	setMonsterValue(-1, "step", tostring(numbstep))
end