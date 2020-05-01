function setFirstState()
	setGlobalValue(-1, "vSpeed", 0)
	setGlobalValue(-1, "actDelay", 0)
	setGlobalValue(-1, "behDelay", 0)
	return "rightrun"	
end

function behRun()
	
  distY = getDistanceToDaveY(-1)

  if getCoordMonsterX(-1)<=getCoordDaveX()
  then
    distX = getDistanceToDaveX(-1,1)
    dir = "right"
  else 
    distX = getDistanceToDaveX(-1,1)
    dir = "left"
  end

  if (distY < 3*16) then 				
  newDir = dir
		if newDir=="left" then
			setState(-1,"leftrun")		
		else
			setState(-1,"rightrun")	
		end
		
		if (distX < 6*16) then 			
			if newDir=="left" then
				setState(-1,"leftprejump")
				setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "prejump1delay"))			
			else
				setState(-1,"rightprejump")
				setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "prejump1delay"))
			end	
		end
  end

end

function behPreJump()

	actDelay = tonumber(getGlobalValue(-1, "actDelay"))
	if actDelay == 0 and (testTileTypeDown(-1,"IMPASSABLE", 1)==1 or testTileTypeDown(-1,"LADDER", 1)==1) then
		if getState(-1)=="leftprejump"  then
			setState(-1,"leftjump")
			--setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "jump1delay"))
		else
			setState(-1,"rightjump")
			--setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "jump1delay"))
		end
		setGlobalValue(-1, "vSpeed", -12) -- -65
	end
end

function behJump()
	
	vSpeed = tonumber(getGlobalValue(-1,"vSpeed"))
	vSpeed = vSpeed + 1.4 -- +3
	if vSpeed > 5 then
		vSpeed = 5
	end
	setGlobalValue(-1, "vSpeed", vSpeed)
	goDown(-1, vSpeed, 1)
	
end

function tileColRun()

	isreturn = (testTileTypeLeft (-1,"IMPASSABLE", 1)==1 and getState(-1)=="leftrun") or
			   (testTileTypeRight(-1,"IMPASSABLE", 1)==1 and getState(-1)=="rightrun")
	
	if isreturn then
		if getState(-1)=="leftrun" then
			setState(-1,"rightrun")
			setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
		else
			setState(-1,"leftrun")
			setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
		end
	else
		
		if (getState(-1)=="leftrun" and goLeft(-1,1,1,1)==1) then
			setState(-1,"leftprejump")
			setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "prejump1delay"))
		end		
		if (getState(-1)=="rightrun" and goRight(-1,1,1,1)==1) then	
			setState(-1,"rightprejump")
			setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "prejump1delay"))
		end
		
	end
end

function tileColJump()
		
	isNegDirOnLand=0
	
	if getState(-1)=="leftjump" then
		testGo = testTileTypeLeft(-1,"IMPASSABLE", 0) 
	else
		testGo = testTileTypeRight(-1,"IMPASSABLE", 0) 
	end
	
	if testGo==1 then
		isNegDirOnLand=1
		setGlobalValue(-1, "behDelay", 0)
	end
	
	if testTileTypeDown(-1,"IMPASSABLE", 1)==1 or testTileTypeDown(-1,"LADDER", 1)==1 then
		
		setGlobalValue(-1, "behDelay", getMonsterValue(-1,"options", "behDelay"))
	
		if isNegDirOnLand==1 then
			 if getState(-1)=="rightjump" then
				 setState(-1,"leftrun")
				 setGlobalValue(-1, "behDelay", 0)
				 --setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
				 goLeft(-1, 0, 0, 1)
				 goUp(-1,8,1)
			 else
				 setState(-1,"rightrun")
				 setGlobalValue(-1, "behDelay", 0)
				 --setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
				 goRight(-1, 0, 0, 1)
				 goUp(-1,8,1)
			 end	 
		else
			if getState(-1)=="rightjump" then
				setState(-1,"rightrun")
				--setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
				goRight(-1, 0, 0, 1)
				goUp(-1,8,1)
			else
				setState(-1,"leftrun")
				--setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
				goLeft(-1, 0, 0, 1)
				goUp(-1,8,1)
			end
		end
	end
		
end

function mainFunc()
	
	if testCollisionDave(-1) == 1 then
		killDave(-1)
		return
	end
	
	actDelay = tonumber(getGlobalValue(-1, "actDelay"))
	actDelay = actDelay - 1
	if actDelay < 0 then 
		actDelay = 0
	end
	setGlobalValue(-1, "actDelay", actDelay)
	
	behDelay = tonumber(getGlobalValue(-1, "behDelay"))
	behDelay = behDelay - 1
	if behDelay < 0 then 
		behDelay = 0
	end
	setGlobalValue(-1, "behDelay", behDelay)

	if getState(-1)=="leftrun" then 
		goLeft(-1, 6, 1, 1)
	end
	
	if getState(-1)=="rightrun" then
		goRight(-1, 6, 1, 1)
	end
	
	if getState(-1)=="leftjump" then
		goLeft(-1, 8, 1, 0)
	end
	
	if getState(-1)=="rightjump" then
		goRight(-1, 8, 1, 0)
	end
	
	if getState(-1)=="leftrun" or getState(-1)=="rightrun" then	
	
		actDelay = tonumber(getGlobalValue(-1, "actDelay"))
		if actDelay == 0 then
			nextNumberOfAction(-1)
			setGlobalValue(-1, "actDelay", getMonsterValue(-1,"options", "run1delay"))
		end
	
		tileColRun()
		
		behDelay = tonumber(getGlobalValue(-1, "behDelay"))
		if behDelay == 0 and (getState(-1)=="leftrun" or getState(-1)=="rightrun") then 
			behRun()
		end
		
		return
	end
	
	if getState(-1)=="leftprejump" or getState(-1)=="rightprejump" then	
	
		behDelay = tonumber(getGlobalValue(-1, "behDelay"))
		if behDelay == 0 then 
			behPreJump()
		end				
		return
	end
	
	if getState(-1)=="leftjump" or getState(-1)=="rightjump" then
		
		tileColJump()
		
		if getState(-1)=="leftjump" or getState(-1)=="rightjump" then
				
			behDelay = tonumber(getGlobalValue(-1, "behDelay"))
			if behDelay == 0 then 
				behJump()
			end
		
		end
		return
	end	

end