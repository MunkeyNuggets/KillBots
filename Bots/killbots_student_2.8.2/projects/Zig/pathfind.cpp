#include "pathfind.h"
#include "kf/kf_log.h"

kf::Vector2 convert(NodePos np)
{
	return kf::Vector2(np.x, np.y);
}

NodePos convert(kf::Vector2 v)
{
	return NodePos(v.x, v.y);
}

Node::Node()
{
	clear();
}

void Node::clear()
{
	c = 1;
	f = 0;
	g = 0;
	h = 0;
	//wall = false;
	parent.set(-1, -1);
	state = NodeState::StateNone;
}

Map::Map()
{
	m_width = 0;
	m_height = 0;
}

Map::~Map()
{

}

void Map::clear()
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			getNode(NodePos(x, y)).clear();
		}
	}
}

void Map::init(int width, int height)
{
	m_width = width;
	m_height = height;
	m_nodes.resize(width*height);
	clear();
}

Node &Map::getNode(const NodePos &np)
{
	if (np.x < 0 || np.y < 0 || np.x >= m_width || np.y >= m_height)
	{
		kf_log("Invalid node coordinate: " << np.x << "," << np.y);
	}
	return m_nodes[np.x + np.y*m_width];
}

Node &Map::getNode(const kf::Vector2 &np)
{
	return getNode(NodePos(np.x, np.y));
}

Node &Map::operator[](const NodePos &np)
{
	return getNode(np);
}

Node &Map::operator[](const kf::Vector2 &np)
{
	return getNode(np);
}

void Map::drawCircle(std::vector<Line> &lines, kf::Vector2 centre, float radius, float r, float g, float b, float fullness)
{
	float angle = (3.14159265*2.0) / 32;
	for (float i = 0; i < 3.14159265*2.0; i += angle*fullness)
	{
		kf::Vector2 p1(cos(i)*radius, sin(i)*radius);
		kf::Vector2 p2(cos(i + angle)*radius, sin(i + angle)*radius);
		Line l;
		l.start = p1 + centre;
		l.end = p2 + centre;
		l.r = r;
		l.g = g;
		l.b = b;
		lines.push_back(l);
	}
}

void Map::drawArrow(std::vector<Line> &lines, kf::Vector2 start, kf::Vector2 end, float head, float r, float g, float b)
{
	kf::Vector2 dir(end - start);
	dir.normalise();
	kf::Vector2 perp = dir.perpCW();
	Line l;
	l.r = r;
	l.g = g;
	l.b = b;
	l.start = start;
	l.end = end;
	lines.push_back(l);
	l.start = end;
	l.end = end + perp*head*0.8 - dir*head;
	lines.push_back(l);
	l.end = end - perp*head*0.8 - dir*head;
	lines.push_back(l);
}

void Map::draw(std::vector<Line> &lines, std::vector<TextMsg> &text, bool drawPath, bool drawWeights, bool drawState)
{
	Line l;
	l.r = 1.0f;
	l.g = 1.0f;
	l.b = 1.0f;
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			switch (m_nodes[x + y*m_width].state)
			{
			case Node::StateClosed:
				if (drawState)
					drawCircle(lines, kf::Vector2(x + 0.5f, y + 0.5f), 0.4, 1, 1, 1, 2);
				if (drawPath)
					drawArrow(lines, kf::Vector2(x + 0.5f, y + 0.5f), kf::convertVector2<kf::Vector2>(m_nodes[x + y*m_width].parent) + kf::Vector2(0.5f, 0.5f), 0.2, 0, 1, 0);
				break;
			case Node::StateOpen:
				if (drawState)
					drawCircle(lines, kf::Vector2(x + 0.5f, y + 0.5f), 0.4, 1, 1, 0, 1);
				if (drawPath)
					drawArrow(lines, kf::Vector2(x + 0.5f, y + 0.5f), kf::convertVector2<kf::Vector2>(m_nodes[x + y*m_width].parent) + kf::Vector2(0.5f, 0.5f), 0.2, 0, 1, 0);
				break;
			case Node::StateNone:
				break;
			}
			if (drawWeights &&  !m_nodes[x + y*m_width].wall)
			{
				char buf[100];
				sprintf(buf, "G=%d\nH=%d", m_nodes[x + y*m_width].g, m_nodes[x + y*m_width].h);
				TextMsg t(buf, kf::Vector2(x, y), 1, 1, 1, 16);
				text.push_back(t);
			}
		}
	}
}

