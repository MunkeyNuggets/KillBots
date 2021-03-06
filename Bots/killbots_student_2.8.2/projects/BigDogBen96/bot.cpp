#include "bot.h"
#include "time.h"

extern "C"
{
	BotInterface27 BOT_API *CreateBot27()
	{
		return new BigDogBen96();
	}
}

BigDogBen96::BigDogBen96()
{
	m_rand(time(0) + (int)this);
}

BigDogBen96::~BigDogBen96()
{
}

void BigDogBen96::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_initialData = initialData;
	attrib.health = 1.0;
	attrib.motor = .1;
	attrib.weaponSpeed = 1.0;
	attrib.weaponStrength = 1.5;
	dir.set(1, 0);
	m_map.init(initialData.mapData.width, initialData.mapData.height);
	setPoint();
	for (int y = 0; y < m_map.m_height; ++y)
	{
		for (int x = 0; x < m_map.m_width; ++x)
		{
			m_map.getNode(NodePos(x, y)).wall = m_initialData.mapData.data[x + y*m_map.m_width].wall;
		}
	}
	pathFind(NodePos(target.x, target.y));
	currentMovementState = MovementState::MS_Random;
	lastMovementState = currentMovementState;
	aimDir.set(0, 1);
}

void BigDogBen96::update(const BotInput &input, BotOutput27 &output)
{
	output.lines.clear();
	output.text.clear();
	kf::Vector2 delta = target - input.position;
	output.moveDirection = delta;
	if (delta.length() < 0.8)
	{
		setPoint();
		pathFind(NodePos(target.x, target.y));
	}
	bool spotted = false;
	for (int i = 0; i < input.scanResult.size(); ++i)
	{
		if (input.scanResult[i].type == VisibleThing::e_robot)
		{
			enemyPos = input.scanResult[i].position;
			enemySpottedFrame = frameNumber;
			spotted = true;
		}
	}

	if (spotted)
	{
		if (currentMovementState == MovementState::MS_Random)
		{
			output.lookDirection = enemyPos - input.position;
			aimDir = output.lookDirection.rotate(m_initialData.scanFOV * -2);
			output.action = BotOutput27::Action::shoot;
		}
	}
	else
	{
		output.action = BotOutput27::Action::scan;
		aimDir = aimDir.rotate(m_initialData.scanFOV * 2);
		output.lookDirection = aimDir;
	}
	m_map.draw(output.lines, output.text, true, false, false);
	kf::Vector2 parent = kf::convertVector2 <kf::Vector2>(m_map.getNode(input.position).parent);
	output.moveDirection = parent - input.position + kf::Vector2(0.5f, 0.5f);
	output.motor = 1.0;

}

void BigDogBen96::result(bool won)
{
}

void BigDogBen96::bulletResult(bool hit)
{

}

void BigDogBen96::pathFind(NodePos endOfPath)
{
	//Map holds data.
	//	NodePos can identify a node in the map.
	std::vector<NodePos> openList;
	//To find a path :
	//Clear map data.
	m_map.clear();
	//pathFound = false
	bool pathFound = false;
	//	Push end node into openList.
	openList.push_back(endOfPath);
	//	While(openList is not empty) // if it's empty, there's no valid path
	while (openList.size() != 0 && pathFound == false)
	{
		NodePos currentPos = openList.front();
		for (int i = 1; i < openList.size(); ++i)
		{
			if (m_map.getNode(openList[i]).f < m_map.getNode(currentPos).f)
			{
				currentPos = openList[i];
			}
		}
		//	For oy = -1 to 1 // offset y
		for (int oy = -1; oy < 2; oy++)
		{
			//	For ox = -1 to 1 // offset x
			for (int ox = -1; ox < 2; ox++)
			{
				//	If ox == 0 and oy == 0 then continue // don't look at itself
				if (ox == 0 && oy == 0 || ox != 0 && oy != 0)
				{
					continue;
				}
					//	Adjacent nodepos is current nodepos + (x, y)
					NodePos adjacentPos = currentPos + NodePos(ox, oy);
					//	New G = current node G + adjacent node C.
					float g = m_map.getNode(currentPos).g + m_map.getNode(adjacentPos).c;
					//	If adjacent node is closed or a wall
					if (m_map.getNode(adjacentPos).wall || m_map.getNode(adjacentPos).state == Node::StateClosed)
				{
					//	Do nothing.
						continue;
				}
				//	Else If adjacent node is open and new g < adjacent g
				else if (m_map.getNode(adjacentPos).state == Node::StateOpen && g < m_map.getNode(adjacentPos).g)
				{
					//	adjacent G = new G
					m_map.getNode(adjacentPos).g = g;
					//	adjacent h = heuristic
					m_map.getNode(adjacentPos).h = 0;
					//	adjacent parent = current nodepos
					m_map.getNode(adjacentPos).parent = currentPos;
					//	adjacent f = adjacent g + adjacent heuristic
					m_map.getNode(adjacentPos).f = m_map.getNode(adjacentPos).g + m_map.getNode(adjacentPos).h;
					//	End
				}
				//	Else If adjacent node is unused
				else if (m_map.getNode(adjacentPos).state == Node::StateNone)
				{
					//	adjacent G = new G
					m_map.getNode(adjacentPos).g = g;
					//	adjacent h = heuristic
					m_map.getNode(adjacentPos).h = 0;
					//	adjacent parent = current nodepos
					m_map.getNode(adjacentPos).parent = currentPos;
					//	adjacent f = adjacent g + adjacent heuristic
					m_map.getNode(adjacentPos).f = m_map.getNode(adjacentPos).g + m_map.getNode(adjacentPos).h;
					//	set adjacent node to open and push on the open list
					m_map.getNode(adjacentPos).state = Node::StateOpen;
					openList.push_back(adjacentPos);
				}
			}
		}
		//	Remove current node from open list and change it's state to closed.
		m_map.getNode(currentPos).state = Node::StateClosed;
		for (auto it = openList.begin(); it != openList.end(); ++it)
		{
			if (*it == currentPos) 
			{
				openList.erase(it);
				break;
			}
		}

	}
}
//	Now you can follow parent nodes from target back until the start to get the path.

//	To fill the map(dijkstra), set adjacent h to 0 above.
//	This pseudo code doesn't stop when it finds a path, it keeps filling. To make it stop early, change the while loop to be while(openList is not empty and pathFound is false)

void BigDogBen96::setPoint()
{
	int index;
	do
	{
		target.set(int(m_rand.norm() * (m_initialData.mapData.width - 3)) + 1.5, int(m_rand.norm() * (m_initialData.mapData.height - 3)) + 1.5);
		index = int(target.x) + int(target.y) * m_initialData.mapData.width;
	} while (m_initialData.mapData.data[index].wall);
}

void BigDogBen96::resetPoint()
{

}


