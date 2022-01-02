/*

TileMap.pixelToTile(x: number, y: number): point
TileMap.pixelToTile(position: point): point

TileMap.tileToPixel(x: number, y: number): point
TileMap.tileToPixel(position: point): point

tiled.mapEditor.currentBrush   TileMap
Get or set the currently used tile brush.

TileMap.usedTilesets  Tileset[]
 TileMap.tilesets Tileset[]

*/

/* var tool = tiled.registerTool("DDDoorSetter", {
    name: "Door Setter",
	//icon: "",

	findObjectsTileSet = function(settool)
	{
		var map = settool.map;
		let setsCount = map.tilesets.length;
		
		for (var i = 0; i < setsCount; i++) 
		{
			var tileset = map.tilesets[i];
			let isTileset  = ((tileset.isTileset) && (tileset.name == "DD2 Original Objects"));
			if (isTileset)
			{
				settool.objectTileset = tileset; 
			}
		}
	},

	findDoorsLayer = function(settool)
	{
		var map = settool.map;
		
		for (var i = 0; i < map.layerCount; i++) 
		{
			var layer = map.layerAt(i);
			let isDoors   = ((layer.isObjectLayer) && (layer.name == "Doors"));
			if (isDoors)
			{
				settool.doorsLayer = layer; 
			}
		}
	},
	
	createDoor = function(settool)
	{
		var assets = tiled.openAssets;
		var assetsCount = assets.length;
		var objectsTileSet = settool.objectTileset;
		let doortTile = objectsTileSet.tile(24);
		
		//for (var index = 0; index < assetsCount; index++)
	//	{
		//	var asset = assets[index];
	//	}
		
		let tileSetTileSize = 16;
		var newDoor = new MapObject();
		newDoor.tile = doortTile;
		newDoor.size = Qt.size(tileSetTileSize, tileSetTileSize); //objectsTileset.tileSize;
	//	newDoor.pos = Qt.point(x * tileSetTileSize,(y + 1) * tileSetTileSize);
		//let exitX = divFunc(tmpID, 256);
		//let exitY = tmpID % 256;
		//door.setProperty("Exit_X", exitX);
		//door.setProperty("Exit_Y", exitY);
	
		settool.newDoor = newDoor;
		settool.newDoorId = newDoor.id;
		settool.isNewDoorUsed = false;
		
		//settool.doorsLayer.addObject(door);		
	},
	
	deleteDoor = function(settool)
	{
		//if (settool.isNewDoorUsed == false)
		//{
		//	settool.doorsLayer.removeObject(settool.newDoor);
		///}
	},
	
	setNewDoorPos = function(settool, x, y)
	{	
		var door = settool.newDoor;
		door.pos = Qt.point(x,y);
	},
	
	updateEnabledState: function()
	{
		tiled.log("updateEnabledState");
		this.enabled = true;
		findDoorsLayer(this);
		findObjectsTileSet(this);
	},
	
	activated: function()
	{
		createDoor(this);	
		tiled.log("Door Setter Activated");		
	},
	
	deactivated: function()
	{	
		deleteDoor();
		tiled.log("Door Setter Deactivated");	
	},
	
	mousePressed: function(button, x, y, modifiers) 
	{
		this.isNewDoorUsed = true;
	},
	
	tilePositionChanged: function()
	{
		if (this.isNewDoorUsed == false)
		{
			var x = this.tilePosition.x * 16;
			var y = (this.tilePosition.y+1) * 16;
			var tool = this;
			setNewDoorPos(tool, x, y);
			//tiled.log("onHoverTileChanged");	
		}
	},
	
	mouseEntered: function()
	{
		var x = this.tilePosition.x * 16;
		var y = this.tilePosition.y * 16;
		this.newDoor.pos = Qt.point(x,y)
		this.doorsLayer.addObject(this.newDoor);
		tiled.log("mouseEntered");
	},
}) */

/* var tool = tiled.registerTool("DoorLinker", {
    name: "Door Deleter",
	icon: "door_deleter.png",
	statusInfo: "Door deleter is active",
	
	mousePressed: function(button, x, y, modifiers) 
	{
		let isDoor
	},
)} */

var tool = tiled.registerTool("DD Door tool", {
    name: "Door Tool",
	icon: "door_tool.png",
	statusInfo: "Door tool is active",

	divFunc = function (val, d)
	{
		let res = (val - (val % d)) / d;
		tiled.log("divFunc val = " + val + " d = " + d + "res = " + res); 
		return res;
	},
	
	float2int = function (value)
	{
		return value | 0;
	},
	
	getObjectById = function(obj_layer, obj_id)
	{
		var obj;	
		let count = obj_layer.objectCount;
		for (var index = 0; index < count; index++)
		{			
		    obj = obj_layer.objectAt(index);
			let isObj = (obj.id == obj_id);
			if (isObj)
			{
				break;					
			}
		}			
		return obj;
	},

	findDoorsAndLinksLayers = function(linker)
	{
		var map = linker.map;
		
		for (var i = 0; i < map.layerCount; i++) 
		{
			var layer = map.layerAt(i);
			
			let isLinks   = ((layer.isObjectLayer) && (layer.name == "Door links"));
			let isDoors   = ((layer.isObjectLayer) && (layer.name == "Teleport doors"));
			
			if (isLinks)
			{
				linker.linkLayer = layer; 
			}
			if (isDoors)
			{
				linker.doorLayer = layer; 
			}
		}
	},
	
	findObjectsetAndDoorTiles = function(linker)
	{
		let assets = tiled.openAssets;
		let assetsCount = assets.length;
		
		for (var index = 0; index < assetsCount; index++)
		{
			var asset = assets[index];
			let istileset = asset.isTileset;
			if (istileset)
			{				
				var tileSetName = asset.name;	
				if (tileSetName == "Teleport doors")
				{
					linker.teleportdoorsSet = asset;
					let levelDoorId = 0;
					let exitDoorId  = 1;
					linker.levelDoorTile = asset.tile(levelDoorId);
					linker.exitDoorTile  = asset.tile(exitDoorId);
				}
				if (tileSetName == "Bonus doors")
				{
					linker.bonusdoorsSet = asset;
				}
				if (tileSetName == "Bonuses")
				{
					linker.bonusesSet = asset;
				}
				if (tileSetName == "Creatures")
				{
					linker.creaturesSet = asset;
				}
				if (tileSetName == "Players")
				{
					linker.playersSet = asset;
				}
			}
		}
		
		
/* 		if (linker.objectsTileSet)
		{
			let levelDoorId = 24;
			let exitDoorId  = 25;
			linker.levelDoorTile = linker.objectsTileset.tile(levelDoorId);
			linker.exitDoorTile = linker.objectsTileset.tile(exitDoorId);
		} */
	},

	levelDoorCheck = function(obj)
	{
		let levelDoorId = 0;
		let isLevelDoor = (obj.tile.id == levelDoorId);
		return isLevelDoor;
	},
	
	posPixelToTile = function(pos, tileSize) // correct
	{
		let tileX = divFunc(pos.x, tileSize); 
		let tileY = divFunc(pos.y, tileSize); // 
		var newPos = Qt.point(tileX, tileY);
		return newPos;
	},
	
	coordsPixelToTile = function(x, y, tileSize) // correct
	{	
		let newPos = Qt.point(x, y); 
		res = posPixelToTile(newPos, tileSize);
		//tiled.log("coordsPixelToTile x = " + x + " y = " + y + "respos = " + res); 
		return res;
	},

	polygonToTileCoordPair = function(pol)
	{		
		var pStart       = pol[0];
		var pEnd         = pol[1];
		let tilePosStart = posPixelToTile(pStart, 16);
		let tilePosEnd   = posPixelToTile(pEnd, 16);
		var res          = [tilePosStart, tilePosEnd];
		return res;
	},
	
	// apply to calc object.pos on set object.pos
	posPixelToObjPixel = function(pos, tileSize) 
	{
		var objpos = Qt.point(pos.x, pos.y + tileSize);
		return objpos;
	},
	
	// apply to calc map pos on get object.pos
	posObjPixelToPixel = function(objpos, tileSize)
	{
		var res = Qt.point(objpos.x, objpos.y - tileSize);
		return res;
	},

	addDoorLinkToLayer = function(layer, doorId, doorEnter_tile_X, doorEnter_tile_Y, doorExit_tile_X, doorExit_tile_Y)
	{
		//tiled.log(doorEnter_tile_X + " " + doorEnter_tile_Y + " " + doorExit_tile_X + " " + doorExit_tile_Y);
		
		var lineObject = new MapObject();
		
		let p1_x = doorEnter_tile_X * 16 + 8;
		let p1_y = doorEnter_tile_Y * 16 + 8;
		
		let p2_x = doorExit_tile_X * 16 + 8;
		let p2_y = doorExit_tile_Y * 16 + 8;
		
		var p1 = Qt.point(p1_x, p1_y);
		var p2 = Qt.point(p2_x, p2_y);
		
		var pol = [p1, p2];
		
		lineObject.setProperty("DoorEnterId", doorId);
		lineObject.shape = MapObject.Polyline;
		lineObject.polygon = pol;
		lineObject.visible = true;
		
		layer.addObject(lineObject); 
	},
	
	updateLinks =  function(linkLayer, doorLayer)
	{
		// find link without doors on both point (start, end)
		// find link with incorrect end (door exit != link end)
		// delete that links
		
		var linksCount = linkLayer.objectCount;	
		var linksToDelete = []; 
		
		//tiled.log("updateLinks()");
		
		for (var linkIndex = 0; linkIndex < linksCount; linkIndex++) 
		{
			var link = linkLayer.objectAt(linkIndex);	
			let pair = polygonToTileCoordPair(link.polygon);
			
			let doorId       = link.resolvedProperty("DoorEnterId");
			let start_tile_X = pair[0].x;
			let start_tile_Y = pair[0].y;
			let end_tile_X   = pair[1].x;
			let end_tile_Y   = pair[1].y;
			
			// find door by id 
			var door = getObjectById(doorLayer, doorId); 
			
			var isUsualDoor = false;		
			if (door)
			{
				isUsualDoor = (door.resolvedProperty("Name") == "Door");
				//tiled.log("isUsualDoor");
			}
			
			var isLinkStartFromDoorWithSetId = false;
			var isLinkEndToCorrectCoord      = false;
			var isNeedDeleteLink             = false;
			
			if (door && isUsualDoor)
			{
				//tiled.log("isLinkEndToCorrectCoord");
				let tmppos      = posObjPixelToPixel(door.pos, 16);
				let doorTilePos = posPixelToTile(tmppos,16);

				let isStartTileXEqual = (doorTilePos.x == start_tile_X);
				let isStartTileYEqual = (doorTilePos.y == start_tile_Y);
				let isStartTileCoordEqual = isStartTileXEqual && isStartTileYEqual;
				isLinkStartFromDoorWithSetId = isStartTileCoordEqual;
				
				let doorExitX = door.resolvedProperty("Exit_X");
				let doorExitY = door.resolvedProperty("Exit_Y");
				let isEndTileXEqual = (doorExitX == end_tile_X);
				let isEndTileYEqual = (doorExitY == end_tile_Y);
				let isEndTileCoordEqual = isEndTileXEqual && isEndTileYEqual;
				isLinkEndToCorrectCoord = isEndTileCoordEqual;
				
				if (!isLinkEndToCorrectCoord)
				{
					// change end coord of link to correct coord (door exit coord)
					let p1 = Qt.point(doorTilePos.x * 16 + 8, doorTilePos.y * 16 + 8);
					let p2 = Qt.point(doorExitX     * 16 + 8, doorExitY     * 16 + 8);
					let pol = [p1, p2];
					link.polygon = pol;
				}
				
				if (!isLinkStartFromDoorWithSetId)
				{
					isNeedDeleteLink = true;
				}
			}
			else
			{
				isNeedDeleteLink = true;
			}
			
			if (isNeedDeleteLink)
			{
				linksToDelete.push(link);			
			}		
		}
		if (linksToDelete.length)	
		{
			for (var linkIndex = 0; linkIndex < linksToDelete.length; linkIndex++)
			{
				linkLayer.removeObject(linksToDelete[linkIndex]);
			}
		}
			
		// find doors without out link
		// set link to that doors 
		
		var doorsCount = doorLayer.objectCount;
		
		for (var doorIndex = 0; doorIndex < doorsCount; doorIndex++) 
		{
			var door = doorLayer.objectAt(doorIndex);
			let isLevelDoor = levelDoorCheck(door);
				
			if (isLevelDoor)
			{
				let tmppos = posObjPixelToPixel(door.pos, 16);
				var doorTilePos = posPixelToTile(tmppos, 16);
				
				var isLinkFromDoorFinded = false;
				
				var lnksCount = linkLayer.objectCount;
				
				for (var lnkInd = 0; lnkInd < lnksCount; lnkInd++) 
				{
					var link = linkLayer.objectAt(lnkInd);
					let pairPos = polygonToTileCoordPair(link.polygon);
					let start_tile_X = pairPos[0].x;
					let start_tile_Y = pairPos[0].y;
					let isTileXEqual = (doorTilePos.x == start_tile_X);
					let isTileYEqual = (doorTilePos.y == start_tile_Y);
					var isCoordEqual = (isTileXEqual && isTileYEqual);
									
					isLinkFromDoorFinded = isCoordEqual;
					if (isLinkFromDoorFinded)
					{
						break;
					}
				}
				
				let isNeedCreateLink = !isLinkFromDoorFinded;
				
				if (lnksCount == 0)
				{
					isNeedCreateLink = true;
				}
				
				if (isNeedCreateLink)
				{		
					let tmppos = posObjPixelToPixel(door.pos, 16);
					let enterPos = posPixelToTile(tmppos, 16);
					let enterX = float2int(enterPos.x);
					let enterY = float2int(enterPos.y); 
					let exitX  = door.resolvedProperty("Exit_X");
					let exitY  = door.resolvedProperty("Exit_Y");
					addDoorLinkToLayer(linkLayer, door.id, enterX, enterY, exitX, exitY);	
				}
			}
		}
	},
	
	deleteDoor = function(linker)
	{
		
	},
	
	selectLayer = function(linker, layer)
	{
		var map = linker.map;
		
		for (var i = 0; i < map.layerCount; i++) 
		{
			var tmplayer = map.layerAt(i);
			
			if (layer == tmplayer)
			{
				tmplayer.selected = true;				
			}
			else
			{
				tmplayer.selected = false;	
			}
		}	
	},
	
	activated: function()
	{
	    findDoorsAndLinksLayers(this);	
		findObjectsetAndDoorTiles(this);
		selectLayerAndTileset(this, "Teleport doors");
		
		//selectLayer(this, this.doorLayer);
			
		//tiled.log("Door Linker Activated");		
	},

/* 	modifiersChanged : function(modifiers)
	{
		let ctrlVal = 0x4000000;			
		let isCtrl = (modifiers == ctrlVal);
		if (isCtrl)
		{
			tiled.log("modifiersChanged");
			this.icon = "door_deleter.png";
		}
		
		//tiled.log("modifiers "+ Number.isInteger(modifiers));
		//tiled.log("modifiers "+modifiers);
				
	}, */

	checkObjectUnderMouse = function(obj_x, obj_y, obj_w, obj_h, mouse_x, mouse_y, tileSize)
	{
		let isX = ((obj_x <= mouse_x) && (mouse_x < (obj_x + obj_w)));
		let isY = ((obj_y <= (mouse_y + tileSize)) && ((mouse_y + tileSize) < (obj_y + obj_h)));
		let isUnderMouse = (isX && isY);
		return isUnderMouse;
	},
		
	checkObjectExistUnderMouse = function(obj_layer, mouse_x, mouse_y)
	{
		var obj;
		
		let count = obj_layer.objectCount;
		for (var index = 0; index < count; index++)
		{			
		    var tmpobj = obj_layer.objectAt(index);
			let x = tmpobj.pos.x;
			let y = tmpobj.pos.y;
			var isUnderMouse = checkObjectUnderMouse(x, y, 16, 16, mouse_x, mouse_y, 16);
				
			if (isUnderMouse)
			{
				obj = tmpobj;
				break;					
			}
		}	
		
		return obj;
	},

    mousePressed: function(button, x, y, modifiers) 
	{		
        var objectLayer = this.map.currentLayer;
		
		let isLayerOk = objectLayer && objectLayer.isObjectLayer && (objectLayer.name == "Teleport doors")
		
        if (isLayerOk) 
		{			
			var objects = objectLayer.objects;
			var objectsCount = objectLayer.objectCount;
			
			let isLeftButton  = (button == 1);
			let isRightButton = (button == 2);
			let isMiddleButton = (button == 4);
			
			let ctrlVal = 0x4000000;
			let altVal  = 0x8000000;
			
			let isCtrl = (modifiers == ctrlVal);
			let isAlt  = (modifiers == altVal);
			
			let isUpdateLinks         = isMiddleButton;
			let isSetEnterDoor        = isLeftButton && !isCtrl && !isAlt;
			let isDeleteDoor          = isLeftButton && isCtrl;
			let isSetExitToDoor       = isRightButton;
			let isSetLevelExitDoor    = isLeftButton && isAlt;
		//	let isDeleteLevelExitDoor = isRightButton && isAlt;
					
			if (isSetEnterDoor)
			{					
				var door = checkObjectExistUnderMouse(objectLayer,x,y);		
				var isDoorNotExistAtCoord = (typeof door == "undefined");
				if (door)
				{
					var isUsualDoor = (door.resolvedProperty("Name") == "Door");
					if (!isUsualDoor)
					{
						objectLayer.removeObject(door);
						isDoorNotExistAtCoord = true;
					}
				}
													
				if (isDoorNotExistAtCoord)
				{					
					door = new MapObject();
							
					door.tile = this.levelDoorTile;
					door.size = this.teleportdoorsSet.tileSize;
					let coord = coordsPixelToTile(x,y,16);				
					let tmpPos = Qt.point(coord.x * 16, coord.y * 16);
					var doorPosToSet = posPixelToObjPixel(tmpPos, 16);
					
					//tiled.log ("!!!!! coord " + coord + " tmpPos " + tmpPos + " doorPosToSet " + doorPosToSet);
					
					door.pos = doorPosToSet;
					//tiled.log ("door.pos = " + door.pos);
					
					door.visible = true;
					door.selected = true;
					this.doorLayer.addObject(door);
					
				}
				
				this.enterX = door.x;
				this.enterY = door.y;
				this.enterId = door.id;
							
				tiled.log("Enter door ID = " + this.enterId);
				
				updateLinks(this.linkLayer, this.doorLayer);
			}
			
			if (isDeleteDoor)
			{
				var door = checkObjectExistUnderMouse(objectLayer,x,y);	
				if (door)
				{
					tiled.log("Delete door ID = " + door.id);
					this.doorLayer.removeObject(door);					
					updateLinks(this.linkLayer, this.doorLayer);
				}
			}
			
			if (isSetExitToDoor)
			{
				var coords = coordsPixelToTile(x,y,16);
				let exitX = float2int(coords.x);
				let exitY = float2int(coords.y);							
				var enterobj = getObjectById(objectLayer, this.enterId);
				
				if (enterobj)
				{	
					enterobj.setProperty("Exit_X", exitX);
					enterobj.setProperty("Exit_Y", exitY);
					enterobj.setProperty("isOpen", "-1");
				//	tiled.log("Coords of exit to enter door with ID = " + this.enterId + " Exit: " + exitX + " " + exitY);	
				}	
					
				updateLinks(this.linkLayer, this.doorLayer);
			}
			
			if (isSetLevelExitDoor)
			{
				var door = checkObjectExistUnderMouse(objectLayer,x,y);		
				let isDoorNotExistAtCoord = (typeof door == "undefined");
							
				if (door)
				{
					var isUsualDoor = (door.resolvedProperty("Name") == "Door");
					if (isUsualDoor)
					{
						objectLayer.removeObject(door);
						isDoorNotExistAtCoord = true;	
					}
				}
							
				if (isDoorNotExistAtCoord)
				{					
					door = new MapObject();
							
					door.tile = this.exitDoorTile;
					door.size = this.teleportdoorsSet.tileSize;
					
					
					let coord = coordsPixelToTile(x,y,16);
					
					let tmpPos = Qt.point(coord.x * 16, coord.y * 16);
					var doorPosToSet = posPixelToObjPixel(tmpPos, 16);
					
					door.pos = doorPosToSet;
					
					door.visible = true;
					door.selected = true;
					this.doorLayer.addObject(door);
					
					// ------------------- Destination level number -----------------------
					
/* 					var numberStr = tiled.prompt("Destination level number:", "", "Choose level");
					var number = parseInt(numberStr);
					number = float2int(number);
					if ((number < 0) || (number > 100))
					{
						number = 0;
					}
					if (typeof number == "number")
					{
						door.setProperty("DestinationLevelNumber", number);
					} */
					
					// ------------------------------------------
					
					
			
				}
				updateLinks(this.linkLayer, this.doorLayer);
			}
			
			if (isUpdateLinks)
			{
				updateLinks(this.linkLayer, this.doorLayer);
			}		
        }
    },
	
	//updateEnabledState: function() 
	//{
	//	tiled.log("onChanged");
	//},
	
/* 	tilePositionChanged: function()
	{
		//tiled.log("onHoverTileChanged");
	}, */
	
/*  	updateEnabledState: function()
	{
		findObjectsetAndDoorTiles(this);
		
		tiled.log("updateEnabledState");
		this.enabled = true;
	},  */
	
})