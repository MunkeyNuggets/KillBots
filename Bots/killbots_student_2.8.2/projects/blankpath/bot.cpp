#include "bot.h"
#include "time.h"

extern "C"
{
	BotInterface27 BOT_API *CreateBot27()
	{
		return new Blank();
	}
}

Blank::Blank()
{
	m_rand(time(0)+(int)this);
}

Blank::~Blank()
{
}

void Blank::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_initialData = initialData;
	attrib.health=1.0;
	attrib.motor=1.0;
	attrib.weaponSpeed=1.0;
	attrib.weaponStrength=1.0;
	dir.set(1, 0);
	m_map.init(initialData.mapData.width, initialData.mapData.height);
	for (int y = 0; y<m_map.m_height; ++y)
	{
		for (int x = 0; x<m_map.m_width; ++x)
		{
			m_map.getNode(NodePos(x, y)).wall = m_initialData.mapData.data[x + y*m_map.m_width].wall;
		}
	}
}

void Blank::update(const BotInput &input, BotOutput27 &output)
{
	output.lines.clear();
	output.text.clear();
	m_map.draw(output.lines, output.text, true, true, true);
	output.moveDirection = dir;
	output.motor = 1.0;
	output.lookDirection.set(0,1);
	output.action = BotOutput::scan;
}

void Blank::result(bool won)
{
}

void Blank::bulletResult(bool hit)
{

}

void Blank::pathFind(NodePos end)
{
	//Map holds data.
	//	NodePos can identify a node in the map.
	//	std::vector<NodePos> openList;

	//To find a path :
	//Clear map data.
	//	pathFound = false
	//	Push end node into openList.
	//	While(openList is not empty) // if it's empty, there's no valid path
	//	Find node in openList with the smallest F value.
	//	For oy = -1 to 1 // offset y
	//	For ox = -1 to 1 // offset x
	//	If ox == 0 and oy == 0 then continue // don't look at itself
	//	Adjacent nodepos is current nodepos + (x, y)
	//	New G = current node G + adjacent node C.
	//	If adjacent node is closed or a wall
	//	Do nothing.
	//	Else If adjacent node is open and new g < adjacent g
	//	adjacent G = new G
	//	adjacent h = heuristic
	//	adjacent parent = current nodepos
	//	adjacent f = adjacent g + adjacent heuristic
	//	End
	//	Else If adjacent node is unused
	//	adjacent G = new G
	//	adjacent h = heuristic
	//	adjacent parent = current nodepos
	//	adjacent f = adjacent g + adjacent heuristic
	//	set adjacent node to open and push on the open list
	//	End
	//	End
	//	End
	//	Remove current node from open list and change it's state to closed.
	//	End

	//	Now you can follow parent nodes from target back until the start to get the path.

	//	To fill the map(dijkstra), set adjacent h to 0 above.
	//	This pseudo code doesn't stop when it finds a path, it keeps filling. To make it stop early, change the while loop to be while(openList is not empty and pathFound is false)
}

