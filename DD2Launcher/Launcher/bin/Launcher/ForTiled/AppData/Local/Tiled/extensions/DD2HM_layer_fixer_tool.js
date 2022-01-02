var tool = tiled.registerTool("DD Layer Fixer Tool", {
    name: "Layer Fixer",
	icon: "layer_fixer_tool.png",
	statusInfo: "Layer fixer tool is active",
	
	getLayerByName = function(map, layerName)
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
	
	getTilesetByName = function(map, tilesetName)
	{
		var resTileset;
		
		if (map)
		{
			let tilesetCount = map.tilesets.length;
				
			//tiled.log("getTilesetByName() tilesetCount = " + tilesetCount); 
				
			for (var tilesetIndex = 0; tilesetIndex < tilesetCount; tilesetIndex++) 
			{
				var tileset = map.tilesets[tilesetIndex];
				let tmpTilesetName = tileset.name;
				let isNamesEqual = (tmpTilesetName == tilesetName);
				if (isNamesEqual)
				{
					resTileset = tileset;
					break;
				}
			}
		}
		
		//tiled.log("getTilesetByName() tilesetName = " + tilesetName + " resTileset = " + resTileset)
		
		return resTileset
	},
	
	fixLayerOfTiles = function(map)
	{
		if (map)
		{
			let layersCount = map.layerCount;
			
			for (var layerIndex = 0; layerIndex < layersCount; layerIndex++) 
			{
				let width = map.width;
				let height = map.height;
				var layer = map.layerAt(layerIndex);
				let layerName = layer.name;
				let currentLayerTileset = getTilesetByName(map, layerName);
				
				if (layer.isTileLayer)
				{
					for (var y = 0; y < height; y++) 
					{           
						for (var x = 0; x < width; x++)
						{
							var tile = layer.tileAt(x, y);
							
							if (tile)
							{
								//let tilesetOfTile = tile.tileSet;
								let tilesetOfTile =  tile.asset;
								//let asset = tile.asset;
								
								if (tilesetOfTile)
								{
									let tilesetNameOfTile = tilesetOfTile.name;	
									//tiled.log("fixLayerOfTiles() tilesetOfTile = " + tilesetNameOfTile);
									let isNeedFixLayer = (tilesetNameOfTile != layerName);
									//tiled.log("isNeedFixLayer");
									if (isNeedFixLayer)
									{ 
										// find correct layer to current tile
										var correctLayer = getLayerByName(map, tilesetNameOfTile);
										
										if (correctLayer)
										{										
											if (correctLayer.isTileLayer)
											{
												// move tile to corect layer
												var layerEdit = correctLayer.edit();
												layerEdit.setTile(x, y, tile);
												layerEdit.apply();
												
												// find tile which need to set on current layer.
												if (currentLayerTileset)
												{
													var toSrcTile = currentLayerTileset.tile(0);
													var sourceLayerEdit = layer.edit();
													sourceLayerEdit.setTile(x, y, toSrcTile);
													sourceLayerEdit.apply();
												}
											}
											else if (correctLayer.isObjectLayer) // move tiles to objects. 
											{
												// it is door on some tileLayer 
												// if some object already exist on correct-dst layer, then need delete it
												
												var dsrPos = Qt.point(x * 16, y * 16);
												var obj = checkObjectExistUnderMouse(correctLayer, dsrPos.x, dsrPos.y);
									
												var isObjectOnDstLayerExist = false;
												
												if (obj)
												{
													isObjectOnDstLayerExist = true;
												}
												
												if (isObjectOnDstLayerExist)
												{
													//tiled.log("IS OBJECT ON DST LEVEL");
													correctLayer.removeObject(obj);
												}
										
												// ------- add object 
												var door = new MapObject();
												door.tile = tile;
												door.size = Qt.size(16,16);
												door.pos = Qt.point(x * 16, (y + 1) * 16);
												correctLayer.addObject(door);	
												
												// ------- set empty to current layer 
												var toSrcTile = currentLayerTileset.tile(0);
												var sourceLayerEdit = layer.edit();
												sourceLayerEdit.setTile(x, y, toSrcTile);
												sourceLayerEdit.apply();
												
												var linkLayer = getLayerByName(map, "Door links");
												var doorLayer = getLayerByName(map, "Teleport doors");
												if (linkLayer && doorLayer)
												{
													updateLinks(linkLayer, doorLayer);
												}
											}
											
										}
									}
								}
								else
								{
									//tiled.log("fixLayerOfTiles() tilesetOfTile not exist " + tilesetOfTile);
								}
							}
						}
					}
				}
				else if (layer.isObjectLayer) // objects to tiles 
				{
					let objectCount = layer.objectCount;
					for (var index = 0; index < objectCount; index++)
					{			
						var tmpobj = layer.objectAt(index);
						var objTile = tmpobj.tile;
						if (objTile)
						{
							let tilesetOfTile = objTile.tileSet;
							if (tilesetOfTile)
							{
								let tilesetNameOfTile = tilesetOfTile.name;	
								let isNeedFixLayer = (tilesetNameOfTile != layerName);
								
								if (isNeedFixLayer)
								{
									var correctLayer = getLayerByName(map, tilesetNameOfTile);
									if (correctLayer)
									{
										if (correctLayer.isTileLayer) // objects to tile
										{
											var layerEdit = correctLayer.edit();
											let pos = posObjPixelToPixel(tmpobj.pos, 16);
											layerEdit.setTile(pos.x, pos.y, objTile);
											layerEdit.apply();
										}
										else if (correctLayer.isObjectLayer) // objects to objects. 
										{
											// nothing. door on door layer
										}
										
									}
								}
							}
						}
					}
					
				}
			}
			
			// ----------  find daves and set count to one -------------
			var playersLayer = getLayerByName(map, "Players");
			
			if (playersLayer)
			{ 
				let width  = playersLayer.width;
				let height = playersLayer.height;
				
				var playersLayerEdit = playersLayer.edit();
				var playersTileset = getTilesetByName(map, "Players");
				
				if (playersTileset)
				{
					let emptyPlayerTile = playersTileset.tile(0);
					
					var isFirstDaveFinded = false;
					
					for (var y = 0; y < height; y++) 
					{           
						for (var x = 0; x < width; x++)
						{
							var tile = playersLayer.tileAt(x, y);
									
							if (tile)
							{
								if (!isFirstDaveFinded)
								{
									isFirstDaveFinded = (tile.id == 1);
								}
								else
								{
									playersLayerEdit.setTile(x, y, emptyPlayerTile);
									playersLayerEdit.apply();	
								}	
							}
						}
					}
				}
			}
			
		}			
	},
	
	activated: function() 	
	{
		fixLayerOfTiles(this.map);		
	},
})