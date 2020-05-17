function setFirstState()
	return "hanging"
end

function mainFunc()
	nextAdditionalNumberOfAction(-1)
	if getAdditionalNumberOfAction(-1) % getMonsterOption(-1, "other", "animationstep") == 0 then
		if getState(-1) ~= "break" then
			nextNumberOfAction(-1)
		else
			if getMonsterFrame(-1) == 2 then
				changeNextLevel()
				return
			else
				nextNumberOfAction(-1)
				if getMonsterFrame(-1) == 2 then
					goDown(-1, 16, 1)
					goLeft(-1, 16, 0)
				end
			end
		end
	end
	local testFreedom = getGlobalValue(-1, "goodfriendfreedom")
	if testFreedom == "1" then
		if getState(-1) ~= "break" then
			local speedFall = getMonsterOption(-1, "options", "speedFall")
			testgo = goDown(-1, speedFall, 1)
			if testgo ~= 0 then
				setNullNumberOfAction(-1)
				setNullAdditionalNumberOfAction(-1)
				setState(-1, "break")
			end
		end
	end
end