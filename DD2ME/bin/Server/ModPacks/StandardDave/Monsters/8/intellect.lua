function setFirstState()
	setMonsterValue(-1, "activate", "0")
	setMonsterValue(-1, "dirV", "1")
	setMonsterValue(-1, "speedY", "0")
	setMonsterValue(-1, "dirH", "1")
	setMonsterValue(-1, "speedX", "0")
	setMonsterValue(-1, "danger", "0")
	setMonsterValue(-1, "oldCoordPlayerX", "-1")
	return "invis"
end

function onKill(type)
	if type == 0 then
		lux, luy, rdx, rdy = getMonsterCollision(-1)
		addMonster(getMonsterOption(-1, "other", "numberscriptclumps"), getCoordMonsterX(-1), getCoordMonsterY(-1), "init", 0, 0, -2, string.format("clumps=%s;phys_box_LU_X=%d;phys_box_LU_Y=%d;phys_box_RD_X=%d;phys_box_RD_Y=%d;", getMonsterOption(-1, "other", "clumps"), lux, luy, rdx, rdy))
		return
	end
end

function mainFunc()
	local oldCoordPlayerX = getMonsterValue(-1, "oldCoordPlayerX")
	if oldCoordPlayerX == "" then
		oldCoordPlayerX = 0
	else
		oldCoordPlayerX = tonumber(oldCoordPlayerX)
	end
	if oldCoordPlayerX < 0 then
		oldCoordPlayerX = getCoordPlayerX()
		setMonsterValue(-1, "oldCoordPlayerX", tostring(cdx))
	end
	local danger = getMonsterValue(-1, "danger")
	if danger == "1" and testCollisionPlayer(-1) == 1 then
		killPlayer(-1)
	end
	nextAdditionalNumberOfAction(-1)
	local activate = tonumber(getMonsterValue(-1, "activate"))
	if activate == 0 then
		local cdx = getCoordPlayerX()
		setMonsterValue(-1, "oldCoordPlayerX", tostring(cdx))
		if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstepghost") == 0 then
			nextNumberOfAction(-1)
		else
			return
		end
		if getDistanceToPlayerXHead(-1) < 96 and getDistanceToPlayerXHead(-1) > 32 then
			local stateplayer = getStatePlayer()
			if (testLookPlayerX(-1) == 1 and cdx - oldCoordPlayerX > 0 and string.find(stateplayer, "right") ~= nil) or (testLookPlayerX(-1) == -1 and cdx - oldCoordPlayerX < 0 and string.find(stateplayer, "left") ~= nil) then
				setMonsterValue(-1, "activate", "1")
				setNullNumberOfAction(-1)
				setNullAdditionalNumberOfAction(-1)
				setMonsterValue(-1, "stateba", getState(-1))
				setState(-1, "invisoff")
				playSound("ghost_view_start")
				return
			end
		end
	end
	if getState(-1) == "invisoff" then
		if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstepinvisoff") == 0 then
			nextNumberOfAction(-1)
			if getMonsterFrame(-1) > 3 then
				setMonsterValue(-1, "danger", "1")
				if getNumberOfLives(-1) < 0 then
					setNumberOfLives(-1, 1)
				end
				playSound("materialisation")
			end
			if getMonsterFrame(-1) == 0 then
				local stateba = getMonsterValue(-1, "stateba")
				if string.find(stateba, "left") ~= nil then
					setState(-1, "leftrun")
				else
					setState(-1, "rightrun")
				end
			end
		else
			return
		end
	end
	local change = 1
	if getState(-1) == "invisoff" then
		change = 0
	end
	if string.find(getState(-1), "run") ~= nil then
		if getAdditionalNumberOfAction(-1) % 6 == 0 then
			return
		end
		local dirV = tonumber(getMonsterValue(-1, "dirV"))
		local speedY = tonumber(getMonsterValue(-1, "speedY"))
		local dirH = tonumber(getMonsterValue(-1, "dirH"))
		local speedX = tonumber(getMonsterValue(-1, "speedX"))
		local maxspeed = 8
		local cdy = getCoordPlayerY()
		local heightline = 0
		local updlineY = cdy + 2 - heightline
		local dpdlineY = cdy + 2 + heightline
		local cmy = getCoordMonsterY(-1)
		if cmy < updlineY then
			dirV = 1
		end
		if cmy > dpdlineY then
			dirV = -1
		end
		if (dirV > 0 and speedY ~= maxspeed) or (dirV < 0 and speedY ~= -maxspeed) then
			speedY = speedY + dirV
		end
		local curspeedY = 5
		local calcspeed = getAdditionalNumberOfAction(-1) % 18
		if calcspeed > 5 then
			curspeedY = 4
		end
		local tt = 3 + (3 - math.abs(speedY))
		if tt > 0 then
			local ocsY = curspeedY
			curspeedY = curspeedY - tt
			if curspeedY < -2 then
				curspeedY = 0
			else
				if curspeedY < 0 then
					curspeedY = ocsY
				end
			end
		end
		if speedY > 0 then
			goDown(-1, curspeedY, 0)
		else
			goUp(-1, curspeedY, 0)
		end

		local cdx = getCoordPlayerX()
		local widthtline = 0
		local lpdlineX = cdx + 2 - widthtline
		local rpdlineX = cdx + 2 + widthtline
		local cmx = getCoordMonsterX(-1)
		if cmx < lpdlineX then
			dirH = 1
		end
		if cmx > rpdlineX then
			dirH = -1
		end
		if (dirH > 0 and speedX ~= maxspeed) or (dirH < 0 and speedX ~= -maxspeed) then
			speedX = speedX + dirH
		end
		local curspeedX = 5
		local calcspeed = getAdditionalNumberOfAction(-1) % 18
		if calcspeed > 5 then
			curspeedX = 4
		end
		tt = 3 + (4 - math.abs(speedX))
		if tt > 0 then
			local ocsX = curspeedX
			curspeedX = curspeedX - tt
			if curspeedX < -2 then
				curspeedX = 0
			else
				if curspeedX < 0 then
					curspeedX = ocsX
				end
			end
		end
		if speedX > 0 then
			goRight(-1, curspeedX, 0)
		else
			goLeft(-1, curspeedX, 0)
		end
		if change == 1 then
			if speedX > 0 then
				setState(-1, "rightrun")
			else
				setState(-1, "leftrun")
			end
		end

		setMonsterValue(-1, "dirV", tostring(dirV))
		setMonsterValue(-1, "speedY", tostring(speedY))
		setMonsterValue(-1, "dirH", tostring(dirH))
		setMonsterValue(-1, "speedX", tostring(speedX))
	end
end