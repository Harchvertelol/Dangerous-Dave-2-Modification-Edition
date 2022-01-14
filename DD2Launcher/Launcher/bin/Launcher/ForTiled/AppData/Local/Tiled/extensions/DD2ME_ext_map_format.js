function DDME_func_getTilesetsPath(ts_path)
{
	var test_file = "DDME_Tileset.json";
	var places_search = [ts_path + '/DDME_TileSets/', ts_path + '/../DDME_TileSets/', ts_path + '/../../../Launcher/ForTiled/DDME_TileSets/', ''];//, '%userprofile%/AppData/Local/Tiled/extensions/DDME_TileSets/'];
	var folder;
	var test_ts = null;
	for(var i = 0; test_ts == null && i < places_search.length; i++)
	{
		folder = places_search[i];
		try
		{
			test_ts = new TextFile(folder + test_file, TextFile.ReadOnly);
		}
		catch(err)
		{}
	}
	return folder;
}

var customMapFormat = {
    name: "DD2ME v0.1 map format",
    extension: "lev",

    divFunc = function (val, d)
	{
		return (val - val % d) / d;
	},

    getLayerByName: function(map, layerName)
	{
		var resLayer;

		if (map)
		{
			let layersCount = map.layerCount;

			for (var layerIndex = 0; layerIndex < layersCount; layerIndex++)
			{
				var layer = map.layerAt(layerIndex);
				let tmpLayerName = layer.name;
				let isNamesEqual = (tmpLayerName == layerName);
				if (isNamesEqual)
				{
					resLayer = layer;
					break;
				}
			}
		}

		return resLayer
	},

    write: function(map, fileName)
	{
		let tileSetTileWidth = 16;
		let tileSetTileHeight = 16;

		var file = new TextFile(fileName, TextFile.WriteOnly);
		file.codec = "UTF-8";

		var tiles_number = 1;

		var width = map.width;
		var height = map.height;

        for (var i = 0; i < map.layerCount; i++)
		{
            var layer = map.layerAt(i);
            var isPlayers = false, isCreatures = false, isBonuses = false, isBonusDoors = false, isTeleportDoors = false, isTiles = false, isParameters = false;
			if(layer.isTileLayer && layer.name == "Players") isPlayers = true;
			else if(layer.name == "Creatures") isCreatures = true;
			else if(layer.isTileLayer && layer.name == "Bonuses") isBonuses = true;
			else if(layer.isTileLayer && layer.name == "Bonus doors") isBonusDoors = true;
			else if(layer.name == "Teleport doors") isTeleportDoors = true;
			else if(layer.name == "Parameters") isParameters = true;
			else if(layer.isTileLayer) isTiles = true;

			if(isPlayers)
			{
				var players_number = 1;
				file.write("[Players]\n");
				for (y = 0; y < height; y++)
				{
					for (x = 0; x < width; x++)
					{
						let tileID = layer.cellAt(x, y).tileId;
						if(tileID > 0)
						{
							file.write("player" + players_number + "=" + x * tileSetTileWidth + ";" + y * tileSetTileHeight + ";\n");
							players_number++;
						}
					}
				}
				file.write("\n");
			}

			if (isBonuses)
			{
				file.write("[Bonuses]\n");
				for (y = 0; y < height; y++)
				{
					for (x = 0; x < width; x++)
					{
						let tileID = layer.cellAt(x, y).tileId;
						if(tileID > 0) file.write(x + ";" + y + "=" + tileID + "\n");
					}
				}
				file.write("\n");
			}

			if (isBonusDoors)
			{
				file.write("[BonusDoors]\n");
				for (y = 0; y < height; y++)
				{
					for (x = 0; x < width; x++)
					{
						let tileID = layer.cellAt(x, y).tileId;
						if(tileID > 0) file.write(x + ";" + y + "=" + tileID + "\n");
					}
				}
				file.write("\n");
			}

			if (isTeleportDoors)
			{
				file.write("[Doors]\n");

				let level_door_tile_id = 0;
				let exit_door_tile_id  = 1;

				let count = layer.objectCount;
				for (var index = 0; index < count; index++)
				{
					var obj = layer.objectAt(index);
					let isLevelDoorObj = (obj.tile.id == level_door_tile_id);
					let isExitDoorObj  = (obj.tile.id == exit_door_tile_id);

					if (isLevelDoorObj)
					{
						let tilex = divFunc(obj.x, tileSetTileHeight);
						let tiley = divFunc(obj.y, tileSetTileHeight) - 1;
						let exitcoordX = obj.property("Exit_X");
						let exitcoordY = obj.property("Exit_Y");
						var isOpen = "-1";
						if(obj.property("isOpen")) isOpen = obj.property("isOpen");
						file.write(tilex + ";" + tiley + "=" + isOpen + ";" + exitcoordX + ";" + exitcoordY + ";\n");
					}
				}

				file.write("\n");
				file.write("[ExitLevelDoors]\n");

				for (var index = 0; index < count; index++)
				{
					var obj = layer.objectAt(index);
					let isLevelDoorObj = (obj.tile.id == level_door_tile_id);
					let isExitDoorObj  = (obj.tile.id == exit_door_tile_id);

					if (isExitDoorObj)
					{
						let tilex = divFunc(obj.x, tileSetTileHeight);
						let tiley = divFunc(obj.y, tileSetTileHeight) - 1;
						let dstLevelNumber = obj.property("DestinationLevelNumber");
						file.write(tilex + ";" + tiley + "=" + dstLevelNumber + "\n");
					}
				}

				file.write("\n");
			}

			if (isCreatures)
			{
				file.write("[Monsters]\n");
				let count = layer.objectCount;
				for (var index = 0; index < count; index++)
				{
						var obj = layer.objectAt(index);
						if(obj.tile.id != 0)
						{
							let parameters = "";
							var allProperties = obj.resolvedProperties();
							for(var key in allProperties) if(key != "" && key != "Name" && key != "Type") parameters += key + "=" + allProperties[key] + ";";
							file.write((index + 1) + "=" + obj.tile.id + ";" + obj.x + ";" + (obj.y - tileSetTileHeight) + ";{" + parameters + "};\n");
						}
				}
				file.write("\n");
			}

			if(isParameters)
			{
				file.write("[TilesParams]\n");
				let count = layer.objectCount;
				var name_secrets_index_in_array = -1;
				for(var index = 0; index < count; index++)
				{
					var obj = layer.objectAt(index);
					if(obj.tile.id == 2) name_secrets_index_in_array = index;
					else if(obj.tile.id == 1)
					{
						let parameters = "";
						var allProperties = obj.resolvedProperties();
						for(var key in allProperties) if(key != "" && key != "Name" && key != "Type") parameters += key + "=" + allProperties[key] + ";";
						if(parameters != "") file.write(divFunc(obj.x, 16) + ";" + divFunc(obj.y - tileSetTileHeight, 16) + "=" + parameters + "\n");
					}
				}
				file.write("\n");
				file.write("[Secrets]\n");
				if(name_secrets_index_in_array >= 0)
				{
					var obj = layer.objectAt(name_secrets_index_in_array);
					let parameters = "";
					var allProperties = obj.resolvedProperties();
					for(var key in allProperties) if(key != "" && key != "Name" && key != "Type") file.write(key + "=" + allProperties[key].replace(/\\n/g, "\\\\n").replace(/\n/g, "\\n") + "\n");
				}
				file.write("\n");
			}

			if (isTiles)
			{
				file.write("[Field_Tiles" + tiles_number + "]\n");
				tiles_number++;
				for (y = 0; y < height; y++)
				{
					file.write("l" + (y + 1) + "=");
					for (x = 0; x < width; x++)
					{
						let tileID = layer.cellAt(x, y).tileId;
						file.write(tileID);
						if(x < width - 1) file.write(" ");
						else file.write("\n");
					}
				}
				file.write("\n");
			}
        }

        var layer_params = getLayerByName(map, "Parameters");
		file.write("[info]\n");
		file.write("sizeX=" + width + "\n");
		file.write("sizeY=" + height + "\n");
		file.write("numberoftilesfields=" + (tiles_number - 1) + "\n");
		file.write("levelformat=0.1\n");
		file.write("name=" + layer_params.property("name") + "\n");
		file.write("background=" + layer_params.property("background") + "\n");
		file.write("type=" + layer_params.property("type") + "\n");
		file.write("\n");

		var allProperties = layer_params.resolvedProperties();

		delete allProperties["name"];
		delete allProperties["background"];
		delete allProperties["type"];

		file.write("[options]\n");

		for(var key in allProperties) if(allProperties[key] != "default") file.write(key + "=" + allProperties[key] + "\n");
		file.write("\n");

        file.write("[SpawnPlayers]\n\n");

        file.commit();
    },

	read: function(fileName)
	{
		parseINIString = function(data)
		{
			var regex = {
			section: /^\s*\[\s*([^\]]*)\s*\]\s*$/,
			param: /^\s*([^=]+?)\s*=\s*(.*?)\s*$/,
			comment: /^\s*;.*$/
			};
			var value = {};
			var lines = data.split(/[\r\n]+/);
			var section = null;
			lines.forEach(function(line){
			if(regex.comment.test(line)){
			return;
			}else if(regex.param.test(line)){
			var match = line.match(regex.param);
			if(section){
			value[section][match[1]] = match[2];
			}else{
			value[match[1]] = match[2];
			}
			}else if(regex.section.test(line)){
			var match = line.match(regex.section);
			value[match[1]] = {};
			section = match[1];
			}else if(line.length == 0 && section){
			section = null;
			};
			});
			return value;
		}

		// ------------- TILESETS ------------------------

		var map = new TileMap()
		map.setTileSize(16, 16);
		map.orientation = TileMap.Orthogonal;
		map.setProperty("IsNewMap", false);

		var tilesetsFilePath = FileInfo.path(fileName);

		let tileSetsFolder = DDME_func_getTilesetsPath(tilesetsFilePath);
		let tspath = tileSetsFolder;

		var tilesTileset         = tiled.open(tspath + "DDME_Tileset.json");
		var playersTileset       = tiled.open(tspath + "DDME_Playerset.json");
		var creaturesTileset     = tiled.open(tspath + "DDME_Creatureset.json");
		var bonusesTileset       = tiled.open(tspath + "DDME_Bonusset.json");
		var bonusdoorsTileset    = tiled.open(tspath + "DDME_Bonusdoorset.json");
		var teleportdoorsTileset = tiled.open(tspath + "DDME_Teleportdoorset.json");
		var tileParametersTileset = tiled.open(tspath + "DDME_TileParamSet.json");

		var good_opened_checker = true;

		if (tilesTileset)
		{
			if (tilesTileset.isTileset)
			{
				map.addTileset(tilesTileset);
			}
		}
		else good_opened_checker = false;

		if (playersTileset)
		{
			if (playersTileset.isTileset)
			{
				map.addTileset(playersTileset);
			}
		}

		if (creaturesTileset)
		{
			if (creaturesTileset.isTileset)
			{
				map.addTileset(creaturesTileset);
			}
		}
		else good_opened_checker = false;

		if (bonusesTileset)
		{
			if (bonusesTileset.isTileset)
			{
				map.addTileset(bonusesTileset);
			}
		}
		else good_opened_checker = false;

		if (bonusdoorsTileset)
		{
			if (bonusdoorsTileset.isTileset)
			{
				map.addTileset(bonusdoorsTileset);
			}
		}
		else good_opened_checker = false;

		if (teleportdoorsTileset)
		{
			if (teleportdoorsTileset.isTileset)
			{
				map.addTileset(teleportdoorsTileset);
			}
		}
		else good_opened_checker = false;

		if (tileParametersTileset)
		{
			if (tileParametersTileset.isTileset)
			{
				map.addTileset(tileParametersTileset);
			}
		}
		else good_opened_checker = false;

		if(!good_opened_checker)
		{
			tiled.alert("Error opening level file: tilesets not found!", "Error!");
			return;
		}

		var file = new TextFile(fileName, TextFile.ReadOnly);
		file.codec = "UTF-8";

		var readAllData = file.readAll();

		var levelData = parseINIString(readAllData);

		var width = levelData["info"]["sizeX"];
		var height = levelData["info"]["sizeY"];

		map.setSize(width, height);

		// ------------- OBJECTS (SPRITES) ------------------------

		var playersLayer = new TileLayer();
		playersLayer.width = width;
		playersLayer.height = height;
		playersLayer.name = "Players";
		playersLayer.visible = true;
		var playersLayerEdit = playersLayer.edit();

		var creaturesLayer = new ObjectGroup();
		creaturesLayer.width = width;
		creaturesLayer.height = height;
		creaturesLayer.name = "Creatures";
		creaturesLayer.visible = true;

		var bonusLayer = new TileLayer();
		bonusLayer.width = width;
		bonusLayer.height = height;
		bonusLayer.name = "Bonuses";
		bonusLayer.visible = true;
		var bonusLayerEdit = bonusLayer.edit();

		var bonusDoorLayer = new TileLayer();
		bonusDoorLayer.width = width;
		bonusDoorLayer.height = height;
		bonusDoorLayer.name = "Bonus doors";
		bonusDoorLayer.visible = true;
		var bonusDoorLayerEdit = bonusDoorLayer.edit();

		var teleportDoorsLayer = new ObjectGroup();
		teleportDoorsLayer.width = width;
		teleportDoorsLayer.height = height;
		teleportDoorsLayer.name = "Teleport doors";
		teleportDoorsLayer.visible = true;

		var doorsLinksLayer = new ObjectGroup();
		doorsLinksLayer.width = width;
		doorsLinksLayer.height = height;
		doorsLinksLayer.name = "Door links";
		//doorsLinksLayer.locked = true;
		doorsLinksLayer.visible = true;

		/*var tileParamatersLayer = new ObjectGroup();
		tileParamatersLayer.height = height;
		tileParamatersLayer.width = width;
		tileParamatersLayer.name = "Tile parameters";
		tileParamatersLayer.visible = false;*/

		var paramatersLayer = new ObjectGroup();
		paramatersLayer.height = height;
		paramatersLayer.width = width;
		paramatersLayer.name = "Parameters";
		paramatersLayer.locked = false;
		paramatersLayer.visible = true;

		// Read properties

		var params;

		params = levelData["info"];
		for(var key in params)
			if(key != "sizeX" && key != "sizeY" && key != "numberoftilesfields" && key != "levelformat") paramatersLayer.setProperty(key, params[key]);
		params = levelData["options"];
		paramatersLayer.setProperty("distancelivemonstersX", "default");
		paramatersLayer.setProperty("distancelivemonstersY", "default");
		paramatersLayer.setProperty("distanceactivatemonstersX", "default");
		paramatersLayer.setProperty("distanceactivatemonstersY", "default");
		paramatersLayer.setProperty("changelevelongameover", "default");
		paramatersLayer.setProperty("numberphysictilesfield", "default");
		paramatersLayer.setProperty("numberobjectstilesfield", "default");
		for(var key in params)
			if(key != "sizeX" && key != "sizeY" && key != "numberoftilesfields" && key != "levelformat") paramatersLayer.setProperty(key, params[key]);

		//----------------

		let tileSetTileWidth = tilesTileset.tileSize.width;
		let tileSetTileHeight = tilesTileset.tileSize.height;

		var tile, tileID;

		for (var y = 0; y < height; y++)
		{
			for (var x = 0; x < width; x++)
			{
				// fill tiles layers to empty value

				let emptyTileID = 0;
				let playersEmptyTile    = playersTileset.tile(emptyTileID);
				let creaturesEmptyTile  = creaturesTileset.tile(emptyTileID);
				let bonusesEmptyTile    = bonusesTileset.tile(emptyTileID);
				let bonusDoorsEmptyTile = bonusdoorsTileset.tile(emptyTileID);

				playersLayerEdit.setTile(x, y, playersEmptyTile);
				bonusLayerEdit.setTile(x, y, bonusesEmptyTile);
				bonusDoorLayerEdit.setTile(x, y, bonusDoorsEmptyTile);
			}
		}

		var stop = false, sch = 1, setObjs;
		var tileID = 1;

		//tiled.log(levelData["Players"]["player"]);

		while(!stop && Object.keys(levelData["Players"]).length > 0)
		{
			var coords = levelData["Players"]["player" + sch].split(";");
			tile = playersTileset.tile(tileID);
			playersLayerEdit.setTile(coords[0] / tileSetTileWidth, coords[1] / tileSetTileHeight, tile);
			sch++;
			if(!levelData["Players"]["player" + sch]) stop = true;
		}

		setObjs = levelData["Monsters"];
		for(var key in setObjs)
		{
			var tmp_info = setObjs[key].split("{");
			var info = tmp_info[0].substr(0, tmp_info[0].length - 1).split(";");
			info.push("{" + tmp_info[1].substr(0, tmp_info[1].length - 2) + "}");
			tileID = info[0];

			var monster = new MapObject();
			tile = creaturesTileset.tile(tileID);
			var x = parseInt(info[1]);
			var y = parseInt(info[2]);
			monster.tile = tile;
			monster.size = playersTileset.tileSize;
			monster.pos = Qt.point(x, y + tileSetTileHeight);
			info[3] = info[3].substr(1);
			info[3] = info[3].substr(0, info[3].length - 1);
			//monster.setProperty("parameters", info[3]);
			var params_array = info[3].split(";");
			for(var i = 0; i < params_array.length; i++)
			{
				if(params_array[i] != "")
				{
					var par_name = params_array[i].substr(0, params_array[i].search("="));
					var par_value = params_array[i].substr(params_array[i].search("=") + 1);
					monster.setProperty(par_name, par_value);
				}
			}
			creaturesLayer.addObject(monster);
		}

		setObjs = levelData["Bonuses"];
		for(var key in setObjs)
		{
			var coords = key.split(";");
			tileID = setObjs[key];
			tile = bonusesTileset.tile(tileID);
			bonusLayerEdit.setTile(coords[0], coords[1], tile);
		}

		setObjs = levelData["BonusDoors"];
		for(var key in setObjs)
		{
			var coords = key.split(";");
			tileID = setObjs[key];
			tile = bonusdoorsTileset.tile(tileID);
			bonusDoorLayerEdit.setTile(coords[0], coords[1], tile);
		}

		setObjs = levelData["Doors"];
		for(var key in setObjs)
		{
			tileID = 0;
			var coords = key.split(";");
			var info = setObjs[key].split(";");
			var x = parseInt(coords[0]);
			var y = parseInt(coords[1]);
			var door = new MapObject();
			tile = teleportdoorsTileset.tile(tileID);
			door.tile = tile;
			door.size = playersTileset.tileSize;
			door.pos = Qt.point(x * tileSetTileWidth, (y + 1) * tileSetTileHeight);
			let exitX = parseInt(info[1]);
			let exitY = parseInt(info[2]);
			door.setProperty("Exit_X", exitX);
			door.setProperty("Exit_Y", exitY);
			door.setProperty("isOpen", info[0]);
			teleportDoorsLayer.addObject(door);
		}

		setObjs = levelData["ExitLevelDoors"];
		for(var key in setObjs)
		{
			tileID = 1;
			var coords = key.split(";");
			var info = setObjs[key];
			var x = parseInt(coords[0]);
			var y = parseInt(coords[1]);

			var door = new MapObject();
			tile = teleportdoorsTileset.tile(tileID);
			door.tile = tile;
			door.size = playersTileset.tileSize;
			door.pos = Qt.point(x * tileSetTileWidth,(y + 1) * tileSetTileHeight);
			let nextLevel = info;
			door.setProperty("DestinationLevelNumber", nextLevel);
			teleportDoorsLayer.addObject(door);
		}

		setObjs = levelData["TilesParams"];
		for(var key in setObjs)
		{
			tileID = 1;
			var coords = key.split(";");
			var info = setObjs[key].split(";");
			var x = parseInt(coords[0]);
			var y = parseInt(coords[1]);

			var tile_param = new MapObject();
			tile = tileParametersTileset.tile(tileID);
			tile_param.tile = tile;
			tile_param.size = playersTileset.tileSize;
			tile_param.pos = Qt.point(x * tileSetTileWidth,(y + 1) * tileSetTileHeight);
			for(var i = 0; i < info.length; i++) if(info[i] != "") tile_param.setProperty(info[i].substr(0, info[i].search("=")), info[i].substr(info[i].search("=") + 1));
			paramatersLayer.addObject(tile_param);
		}

		// Create secrets name object

		{
			tileID = 2;
			var tile_param = new MapObject();
			tile = tileParametersTileset.tile(tileID);
			tile_param.tile = tile;
			tile_param.size = playersTileset.tileSize;
			var x = -1;
			var y = -1;
			tile_param.pos = Qt.point(x * tileSetTileWidth, (y + 1) * tileSetTileHeight);

			setObjs = levelData["Secrets"];
			for(var key in setObjs) if(key != "") tile_param.setProperty(key, setObjs[key].replace(/\\n/g, "\n").replace(/\\\n/g, "\\n"));
			paramatersLayer.addObject(tile_param);
		}

		delete levelData["info"];
		delete levelData["options"];
		delete levelData["SpawnPlayers"]; //!!!
		delete levelData["TilesParams"]; //!!!

		delete levelData["Players"];
		delete levelData["Monsters"];
		delete levelData["Bonuses"];
		delete levelData["BonusDoors"];
		delete levelData["Doors"];
		delete levelData["ExitLevelDoors"];
		delete levelData["Secrets"];

		// ------------- TILES ------------------------

		for(var key in levelData)
		{
			var tilesLayer = new TileLayer();
			tilesLayer.width = width;
			tilesLayer.height = height;
			tilesLayer.name = "Tiles";
			tilesLayer.visible = true;
			var tilesLayerEdit = tilesLayer.edit();
			//...
			var layer_name = key.substr(6);
			tilesLayer.name = layer_name;
			for (var y = 0; y < height; y++)
			{
				var str_level = levelData[key]["l" + (y + 1)];
				var ids = str_level.split(" ");
				for (var x = 0; x < width; x++)
				{
					var tileID = parseInt(ids[x]);
					if(tileID >= 0) tilesLayerEdit.setTile(x, y, tilesTileset.tile(tileID));
				}
			}
			//...
			tilesLayerEdit.apply();
			map.addLayer(tilesLayer);
		}

		// -----------------------------

		playersLayerEdit.apply();
		bonusLayerEdit.apply();
		bonusDoorLayerEdit.apply();

		map.addLayer(playersLayer);
		map.addLayer(creaturesLayer);
		map.addLayer(bonusLayer);
		map.addLayer(bonusDoorLayer);
		map.addLayer(teleportDoorsLayer);
		map.addLayer(doorsLinksLayer);
		//map.addLayer(tileParamatersLayer);
		map.addLayer(paramatersLayer);

		updateLinks(doorsLinksLayer, teleportDoorsLayer);

	   // -----------------------------------------------

	   file.close();

	   return map;

    },
}

tiled.registerMapFormat("DDHME0.1", customMapFormat)