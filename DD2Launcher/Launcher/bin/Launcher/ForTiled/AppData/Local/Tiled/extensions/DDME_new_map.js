 function onAssetChanged(asset) // переключение окон ассетов
 {
	// tiled.log("onAssetChanged");
	// сразу после создания новой карты ассет с картой становится активным

	// использовать пользовательские свойства ассетов
	// IsNewMap - карта только создана и ещё не загружен контент.

	if (asset)
	{
		if (asset.isTileMap)
		{
			var map = asset;
			var isNewMap = map.resolvedProperty("IsNewMap");

			if (isNewMap)
			{
				var fileName = map.fileName;

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
				var backgroundObjectsTileset = tiled.open(tspath + "DDME_BackgroundsObjects.json");

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
				else good_opened_checker = false;

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

				if (backgroundObjectsTileset)
				{
					if (backgroundObjectsTileset.isTileset)
					{
						map.addTileset(backgroundObjectsTileset);
					}
				}
				else good_opened_checker = false;


				if (tilesTileset || playersTileset|| creaturesTileset || bonusesTileset || bonusdoorsTileset || teleportdoorsTileset || tileParametersTileset || backgroundObjectsTileset)
				{
					//tiled.log("onAssetChanged() RELOAD MAP");
					tiled.reload(map);
				}

				if(!good_opened_checker)
				{
					tiled.alert("Error opening level file: tilesets not found!", "Error!")
				}

				map.setProperty("IsNewMap", false);

			}
		}
	}
 }

 function assetAboutToBeSaved(asset) // перед сохранением
{
	var isNewMapProperty = asset.resolvedProperty("IsNewMap");
	let isNewMapPropertyExist = (typeof isNewMapProperty != "undefined");

	var isNewMap;
	if (typeof isNewMapProperty == "undefined")
	{
		isNewMap = true;
	}
	else
	{
		isNewMap = isNewMapProperty;
	}

	let isTileMap = asset.isTileMap;
	let isNeedFillMap = isTileMap && isNewMap;

	if (isTileMap && isNewMapPropertyExist)
	{
		let isOldMap = !isNewMap;
		if (isOldMap)
		{
			fixLayerOfTiles(asset);
			//tiled.log("assetAboutToBeSaved() fixLayerOfTiles");
		}
	}

	if (isNeedFillMap)
	{
		//tiled.log("assetAboutToBeSaved() asset.isTileMap");

		var map = asset;

		if (isNewMap)
		{
			var width = map.width;
			var height = map.height;

			// ------------- TILES ------------------------

			var tileset = new Tileset();
			for (var i = 0; i < 52; i++)
			{
				tileset.addTile();
			}
			let emptyTile = tileset.tile(0);
			let wallTile = tileset.tile(51)

			// -----

			var tilesLayer = new TileLayer();
			tilesLayer.width = width;
			tilesLayer.height = height;
			tilesLayer.name = "Tiles1";
			tilesLayer.visible = true;
			var tilesLayerEdit = tilesLayer.edit();

			for (var y = 0; y < height; y++)
			{
				for (var x = 0; x < width; x++)
				{
					tilesLayerEdit.setTile(x, y, emptyTile);

					// set 2 tile wall on perimeter

					let isTopLine    = (y < 2);
					let isBottomLine = (y > height - 3);
					let isLeftLine   = (x < 2);
					let isRightLine  = (x > width - 3);

					if (isTopLine || isBottomLine || isLeftLine || isRightLine)
					{
						tilesLayerEdit.setTile(x, y, wallTile);
					}
				}
			}

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

			for (var y = 0; y < height; y++)
			{
				for (var x = 0; x < width; x++)
				{
					playersLayerEdit.setTile(x, y, emptyTile);
					bonusLayerEdit.setTile(x, y, emptyTile);
					bonusDoorLayerEdit.setTile(x, y, emptyTile);
				}
			}

			for(var i = map.layerCount - 1; i >= 0; i--) map.removeLayerAt(i);

			paramatersLayer.setProperty("name", "");
			paramatersLayer.setProperty("type", "single");

			/*{
				tileID = 2;
				var tile_param = new MapObject();
				tile = tileParametersTileset.tile(tileID);
				tile_param.tile = tile;
				tile_param.size = playersTileset.tileSize;
				var x = -1;
				var y = -1;
				tile_param.pos = Qt.point(x * tileSetTileWidth, (y + 1) * tileSetTileHeight);
			}*/

			tilesLayerEdit.apply();
			playersLayerEdit.apply();
			bonusLayerEdit.apply();
			bonusDoorLayerEdit.apply();

			map.addLayer(tilesLayer);
			map.addLayer(playersLayer);
			map.addLayer(creaturesLayer);
			map.addLayer(bonusLayer);
			map.addLayer(bonusDoorLayer);
			map.addLayer(teleportDoorsLayer);
			map.addLayer(doorsLinksLayer);
			map.addLayer(paramatersLayer);

			asset.setProperty("IsNewMap", true);
		}
	}
}

tiled.activeAssetChanged.connect(onAssetChanged)
tiled.assetAboutToBeSaved.connect(assetAboutToBeSaved)