function setFirstState()
	return "init"
end

function addClumps(numbercl, phys_box_LU_X, phys_box_LU_Y, phys_box_RD_X, phys_box_RD_Y)
	pbSX = phys_box_RD_X - phys_box_LU_X;
	pbSY = phys_box_RD_Y - phys_box_LU_Y;
	xc1, yc1, xc2, yc2 = getMonsterCollision(-1)
	pbSCL_X = xc2 - xc1
	pbSCL_Y = yc2 - yc1
	numberrow = math.floor(pbSX / pbSCL_X + 0.5)
	if numberrow < 2 then
		numberrow = 2
	end
	numberclrow = math.floor(numbercl / numberrow + 0.5)
	stepX = pbSX / numberrow
	stepY = pbSY / numberclrow
	startX = getCoordMonsterX(-1) + phys_box_LU_X - pbSCL_X / 2
	startY = getCoordMonsterY(-1) + phys_box_LU_Y
	timeupdeviation = tonumber(getMonsterOption(-1, "other", "timegoupclumpdeviation"))
	timeupset = tonumber(getMonsterOption(-1, "other", "timegoupclump")) + math.random(0, timeupdeviation * 2) - timeupdeviation
	params = string.format("timeup=%s;", timeupset)
	for i = 0, numberrow - 1 do
		for j = 0, numberclrow - 1 do
			if i % 2 == 0 then
				addDuplicateMonster(-1, startX + stepX * i, startY + stepY * j, "leftupfly", 0, 0, -2, params)
			else
				addDuplicateMonster(-1, startX + stepX * i, startY + stepY * j, "rightupfly", 0, 0, -2, params)
			end
		end
	end
end

function mainFunc()
	if getState(-1) == "init" then
		numbercl = tonumber(getMonsterValue(-1, "clumps"))
		phys_box_LU_X = tonumber(getMonsterValue(-1, "phys_box_LU_X"))
		phys_box_LU_Y = tonumber(getMonsterValue(-1, "phys_box_LU_Y"))
		phys_box_RD_X = tonumber(getMonsterValue(-1, "phys_box_RD_X"))
		phys_box_RD_Y = tonumber(getMonsterValue(-1, "phys_box_RD_Y"))
		addClumps(numbercl, phys_box_LU_X, phys_box_LU_Y, phys_box_RD_X, phys_box_RD_Y)
		killMonster(-1, 0)
		return
	end
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		nextNumberOfAction(-1)
	end
	curStateMnst = getState(-1)
	if curStateMnst == "stand" then
		timelivestand = tonumber(getMonsterValue(-1, "timelivestand"))
		timelivestand = timelivestand - 1
		if timelivestand == 0 then
			killMonster(-1, 0)
			return
		end
		setMonsterValue(-1, "timelivestand", timelivestand)
		return
	end
	shift = getMonsterValue(-1, "shift")
	if shift == "" then
		shiftdeviation = tonumber(getMonsterOption(-1, "other", "shiftgoclumpdeviation"))
		shift = tonumber(getMonsterOption(-1, "other", "shiftgoclump")) + math.random(0, shiftdeviation * 2) - shiftdeviation
		setMonsterValue(-1, "curShift", shift)
	end
	shift = tonumber(shift)
	correctStandDown = tonumber(getMonsterOption(-1, "other", "correctstanddown"))
	if getMonsterValue(-1, "correctionUp") == "" then
		x1, x2, y1, y2 = getMonsterCollision(-1)
		correctTmp = correctStandDown - (y2 - y1)
		if correctTmp > 0 and getCoordMonsterY(-1) % 16 ~= 0 then
			goUp(-1, correctTmp, 0)
			goDown(-1, correctTmp, 1)
		end
		setMonsterValue(-1, "correctionUp", "1")
	end
	if string.find(curStateMnst, "up") ~= nil then
		timeup = tonumber(getMonsterValue(-1, "timeup"))
		timeup = timeup - 1
		setMonsterValue(-1, "timeup", timeup)
		if curStateMnst == "leftupfly" then
			resgoL = goLeft(-1, shift, 1)
			resgoU = goUp(-1, shift, 1)
			if resgoL == 1 then
				setState(-1, "rightupfly")
			end
			if resgoU == 1 then
				if string.find(getState(-1), "left") ~= nil then
					setState(-1, "leftdownfly")
				else
					setState(-1, "rightdownfly")
				end
			end
		else
			resgoR = goRight(-1, shift, 1)
			resgoU = goUp(-1, shift, 1)
			if resgoR == 1 then
				setState(-1, "leftupfly")
			end
			if resgoU == 1 then
				if string.find(getState(-1), "left") ~= nil then
					setState(-1, "leftdownfly")
				else
					setState(-1, "rightdownfly")
				end
			end
		end
		if timeup == 0 then
			if string.find(getState(-1), "left") ~= nil then
				setState(-1, "leftdownfly")
			else
				setState(-1, "rightdownfly")
			end
		end
		return
	else
		if curStateMnst == "leftdownfly" then
			resgoL = goLeft(-1, shift, 1)
			resgoD = goDown(-1, shift, 1)
			if resgoL == 1 then
				setState(-1, "rightdownfly")
			end
			if resgoD == 1 then
				setState(-1, "stand")
				goDown(-1, correctStandDown, 1)
				timelivedeviation = tonumber(getMonsterOption(-1, "other", "timelivestanddeviation"))
				timeliveset = tonumber(getMonsterOption(-1, "other", "timelivestand")) + math.random(0, timelivedeviation * 2) - timelivedeviation
				setMonsterValue(-1, "timelivestand", timeliveset)
			end
		else
			resgoR = goRight(-1, shift, 1)
			resgoD = goDown(-1, shift, 1)
			if resgoR == 1 then
				setState(-1, "leftdownfly")
			end
			if resgoD == 1 then
				setState(-1, "stand")
				goDown(-1, correctStandDown, 1)
				timelivedeviation = tonumber(getMonsterOption(-1, "other", "timelivestanddeviation"))
				timeliveset = tonumber(getMonsterOption(-1, "other", "timelivestand")) + math.random(0, timelivedeviation * 2) - timelivedeviation
				setMonsterValue(-1, "timelivestand", timeliveset)
			end
		end
		return
	end
end