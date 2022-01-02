/*
var tool = tiled.registerTool("BrushTool", {
    name: "Brush Tool",
	//icon: "door_tool.png",
	statusInfo: "Brush tool is active",


	findLayers = function(tool)
	{
		var map = tool.map;

		for (var i = 0; i < map.layerCount; i++)
		{
			var layer = map.layerAt(i);

			let isLinks      = ((layer.isObjectLayer) && (layer.name == "Door links"));
			let isDoors      = ((layer.isObjectLayer) && (layer.name == "Teleport doors"));
			let isBonusdoors = ((layer.isTileLayer)   && (layer.name == "Bonus doors"));
			let isBonuses    = ((layer.isTileLayer)   && (layer.name == "Bonuses"));
			let isCreatures  = ((layer.isTileLayer)   && (layer.name == "Creatures"));
			let isPlayers    = ((layer.isTileLayer)   && (layer.name == "Players"));

			if (isLinks)
			{
				tool.linkLayer = layer;
			}
			if (isDoors)
			{
				tool.doorLayer = layer;
			}
			if (isBonusdoors)
			{
				tool.bonusdoorLayer = layer;
			}
			if (isBonuses)
			{
				tool.bonusesLayer = layer;
			}
			if (isCreatures)
			{
				tool.creaturesLayer = layer;
			}
			if (isPlayers)
			{
				tool.playersLayer = layer;
			}
		}
	},

	selectLayerByTileset = function(tool, tilesetName)
	{
		var map = tool.map;

		for (var i = 0; i < map.layerCount; i++)
		{
			var layer = map.layerAt(i);
			let layerName = layer.name;
			let isNeedLayer = (layerName == tilesetName || layerName == (tilesetName + "1"))
			tiled.log(tilesetName + "1");
			layer.selected = isNeedLayer;
			layer.locked = isNeedLayer;
		}

 		switch (tilesetName)
		{
			case "Players":
				tool.playersLayer.selected = true;
			break;

			case "Creatures":

			break;

			case "Bonuses":

			break;

			case "Bonus doors":

			break;

			case "Teleport doors":

			break;
		}
	},

	selectTilesetByLayer = function(tool, layerName)
	{
		var map = tool.map;

		if (map)
		{
			for (var layerIndex = 0; layerIndex < map.layerCount; layerIndex++)
			{
				var layer = map.layerAt(layerIndex);
				let tmplayerName = layer.name;
				tiled.log("selectTilesetByLayer() tmplayerName = " + tmplayerName + " layerName = " + layerName);
				let isNeedLayer = (layerName == tmplayerName || layerName + "1" == tmplayerName);
				//layer.selected = isNeedLayer;
				//layer.locked = !isNeedLayer;
			}


 			var tileset;
			for (var tilesetIndex = 0; tilesetIndex < map.tilesets.length; tilesetIndex++)
			{
				var tileset = map.tilesets[tilesetIndex];
				let tmpName = set.name;
				let isNeedSet = (layerName == tmpName || layerName + "1" == tmpName);
				if (isNeedSet)
				{
					break;
				}
			}

			if (tileset)
			{
				var view = tiled.mapEditor.tilesetsView;
				view.currentTileset = tileset;
			}
		}
	},

	updateState = function(tool)
	{
		var view = tiled.mapEditor.tilesetsView;
		var currentTilesetName = view.currentTileset.name;
		tiled.log("currentTilesetName = " + currentTilesetName);

		if (view.selectedTiles.length > 0)
		{
			var tileId = view.selectedTiles[0].id;
			tiled.log("selectedTileId = " + tileId);
		}
		selectLayerByTileset(tool, currentTilesetName);

	},


	activated: function()
	{
		findLayers(this);
		this.isActive = true;
	//	updateState(this);
	   // findDoorsAndLinksLayers(this);
		//findObjectsetAndDoorTiles(this);
		//selectLayer(this, this.doorLayer);
		//tiled.log("Brush Linker Activated");
	},

	deactivated: function()
	{
		this.isActive = false;
		tiled.log("deactivated!!!!!");
		this.enabled = true;
	},

 	mouseMoved: function(x, y, modifiers)
	{

	//	updateState(this);
	},

	mousePressed: function(button, x, y, modifiers)
	{
		tiled.log("mousePressed()");
		//updateState(this);
	},

	mouseEntered: function()
	{

		tiled.log("mouseEntered()");
		//updateState(this);
	},

	// работает только с модификаторами
	keyPressed: function(key, modifiers)
	{
		tiled.log("keyPressed()");
	},

	// current layer changed
    updateEnabledState: function()
	{
		if (this.enabled)
		{
			if (this.isActive)
			{
				var map = this.map;
				if (map)
				{
					var selectedlayers = map.selectedLayers;
					var layerscount = map.layerCount;
					var selectedlayercount = selectedlayers.length;

					for (var lIndex = 0; lIndex < layerscount; lIndex++)
					{
						tiled.log("updateEnabledState() layers index" + lIndex);
						var tmplayer = map.layerAt(lIndex);
						if (tmplayer.selected)
						{
							tiled.log("updateEnabledState() + " + "name = " + tmplayer.name);
						}
					}

 					for (var layerIndex = 0; layerIndex < selectedlayercount; layerIndex++)
					{
						var tmplayer = selectedlayers[selectedlayercount];
						if ()
						tiled.log("updateEnabledState() selectedlayers " + layerIndex + "name = " + tmplayer.name);
					}

 					var layer;
					var layername;
					if (layercount > 0)
					{
						layer = selectedlayers[0];
						layername = layer.name;
					}

					if (layername)
					{
						tiled.log("updateEnabledState() layername = " + layername);
						selectTilesetByLayer(this, layername);
					}
				}
			}
		}
		tiled.log("updateEnabledState");
		this.enabled = true;
	},
}) */

	var getNameFirstSelectedLayer = function(map)
	{
		for (var layerIndex = 0; layerIndex < map.layerCount; layerIndex++)
		{
			var layer = map.layerAt(layerIndex);
			let layerName = layer.name;
			if(layer.selected) return layerName;
		}
		return "";
	}

	var selectLayerAndTileset = function(tool, tilesetName, layerName = "", numberSelectTileInTileset = 0)
	{
		//tiled.log("selectLayerAndTileset()");

		if(layerName == "") layerName = tilesetName;

		var map = tool.map;

		var already_selected = false;

		var sel_layer_name = getNameFirstSelectedLayer(map);

		if(layerName.search("Tiles") == 0 && sel_layer_name != "" && sel_layer_name.search("Tiles") == 0) already_selected = true;

		if (map)
		{
			for (var layerIndex = 0; layerIndex < map.layerCount; layerIndex++)
			{
				var layer = map.layerAt(layerIndex);
				let tmplayerName = layer.name;
				//tiled.log("selectTilesetByLayer() tmplayerName = " + tmplayerName + " layerName = " + layerName);
				let isNeedLayer = (layerName == tmplayerName || (tmplayerName.search("Tiles") == 0 && layerName.search("Tiles") == 0) );
				if(!already_selected && isNeedLayer) { layer.selected = isNeedLayer; already_selected = true; }
				else if(layerName.search("Tiles") != 0 || sel_layer_name.search("Tiles") != 0) layer.selected = false;
				layer.locked = !isNeedLayer;
			}

			var tileset;
			for (var tilesetIndex = 0; tilesetIndex < map.tilesets.length; tilesetIndex++)
			{
				var tileset = map.tilesets[tilesetIndex];
				let tmpName = tileset.name;
				let isNeedSet = (tilesetName == tmpName);
				if (isNeedSet)
				{
					// tileset variable is set
					break;
				}
			}

			if(tileset)
			{
				var view = tiled.mapEditor.tilesetsView;
				view.currentTileset = tileset;
				var selectedTiles = view.selectedTiles;
				let selectedTilesCount = selectedTiles.length;
				if (selectedTilesCount == 0)
				{
					// setFirstTileOfTileSet to selectedTiles field

					var firstTile = tileset.tile(numberSelectTileInTileset);
					var newSelectedTiles = [firstTile];
					view.selectedTiles = newSelectedTiles;

					var brushMap =  new TileMap();

					var brushLayer = new TileLayer();
					brushLayer.width = 1;
					brushLayer.height = 1;
					//brushLayer.name = "Players";
					brushLayer.visible = true;
					var brushLayerEdit = brushLayer.edit();
					brushLayerEdit.setTile(0, 0, firstTile);
					brushLayerEdit.apply();
					brushMap.addLayer(brushLayer);

					tiled.mapEditor.currentBrush = brushMap;
				}
			}
		}
	}


var tool = tiled.registerTool("DD Tiles Tool", {
    name: "Tiles",
	icon: "tool_tiles.png",
	statusInfo: "Tiles tool is active",

	activated: function()
	{
		selectLayerAndTileset(this, "Tiles", "Tiles1");
	},

})

var tool = tiled.registerTool("DD Players Tool", {
    name: "Players",
	icon: "tool_players.png",
	statusInfo: "Players tool is active",

	activated: function()
	{
		selectLayerAndTileset(this, "Players");
	},
})

var tool = tiled.registerTool("DD Creatures Tool", {
    name: "Creatures",
	icon: "tool_creatures.png",
	statusInfo: "Creatures tool is active",

	activated: function()
	{
		tiled.trigger("CreateTileObjectTool");
		selectLayerAndTileset(this, "Creatures");
	},
})

var tool = tiled.registerTool("DD Bonuses Tool", {
    name: "Bonuses",
	icon: "tool_bonuses.png",
	statusInfo: "Bonuses tool is active",

	activated: function()
	{
		selectLayerAndTileset(this, "Bonuses");
	},
})

var tool = tiled.registerTool("DD Bonus doors Tool", {
    name: "Bonus doors",
	icon: "tool_bonus_doors.png",
	statusInfo: "Bonus doors tool is active",

	activated: function()
	{
		selectLayerAndTileset(this, "Bonus doors");
	},
})

var tool = tiled.registerTool("DD Tile parameters Tool", {
    name: "Tile parameters",
	icon: "tool_tile_parameters.png",
	statusInfo: "Tile parameters tool is active",

	activated: function()
	{
		selectLayerAndTileset(this, "Tile params", "Parameters", 1);
		tiled.trigger("CreateTileObjectTool");
	},
})