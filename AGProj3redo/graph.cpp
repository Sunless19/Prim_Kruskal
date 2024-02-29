#include "graph.h"
#include <QFile>
#include <unordered_map>
#include <unordered_set>
#include<stack>
#include <queue>
Graph::Graph()
	:isOriented(true)
	,m_root(nullptr)
{

}

Graph::Graph(Graph& g)
	:isOriented(true)
{
	for (Node* n : g.m_nodes)
	{
		addNode(n);
	}
	for (Arch* ed : g.m_arches)
	{
		Node* selectFirstNode{ nullptr }, * selectSecondNode{ nullptr }, * firstNode{ ed->getFirstNode() }, * secondNode{ ed->getSecondNode() };
		for (Node* n : g.m_nodes)
		{
			if (n->getValue() == firstNode->getValue())
				selectFirstNode = n;
			else
				if (n->getValue() == secondNode->getValue())
					selectSecondNode = n;
			if (selectFirstNode && selectSecondNode)
			{
				addArch(new Arch{ selectFirstNode ,selectSecondNode });
				break;
			}
		}
	}
}

Graph::~Graph()
{
	for (int i = 0; i < m_arches.size(); i++)
		delete m_arches[i];

	for (int i = 0; i < m_nodes.size(); i++)
		delete m_nodes[i];
}

void Graph::addNode(Node* n1)
{
	Node* n = new Node(n1);
	m_nodes.push_back(n);
}

void Graph::addNode(QPoint p)
{
	Node* n = new Node;
	n->setCoordinate(p);
	n->setValue(m_nodes.size() + 1);
	m_nodes.push_back(n);
}

void Graph::addArch(Arch* arch)
{

	Arch* ed = new Arch(arch->getFirstNode(), arch->getSecondNode());
	ed->setCost(arch->getCost());
	m_arches.push_back(ed);
}

void Graph::addArch(Node* firstNode, Node* secondNode)
{
	Arch* arch = new Arch(firstNode, secondNode);
	m_arches.push_back(arch);
}

void Graph::setOrientedOrNot(bool tOrF)
{
	isOriented = tOrF;
}

void Graph::FindRoot()
{
	std::unordered_set<Node*> potentialRoots(m_nodes.begin(), m_nodes.end());

	for (Arch* arch : m_arches) {
		potentialRoots.erase(arch->getSecondNode());
	}

	if (potentialRoots.size() == 1)
		this->m_root = *potentialRoots.begin();
	else
		this->m_root = nullptr;
}

std::vector<Node*> Graph::getNodes()
{
	return m_nodes;
}

std::vector<Arch*> Graph::getArches()
{
	return m_arches;
}

std::vector<Node*> Graph::getNeighbors(Node* node)
{
	std::vector<Node*> neighbors;

	for (Arch* ed : m_arches)
	{
		if (ed->getFirstNode() == node)
		{
			neighbors.push_back(ed->getSecondNode());
		}
		else if (ed->getSecondNode() == node && !isOriented)
		{
			neighbors.push_back(ed->getFirstNode());
		}
	}

	return neighbors;
}

std::vector<std::vector<int>> Graph::findConnectedComponents()
{
	std::set<int> N;
	for (auto& node : m_nodes)
	{
		N.insert(node->getValue());
	}

	std::vector<std::vector<int>> connectedComponents;
	std::set<int> U, V, W, N_prime;
	int p = 0;

	if (N.empty()) return connectedComponents;

	int s = *(N.begin());
	U = N;
	U.erase(s);
	V.insert(s);
	W.clear();
	N_prime.insert(s);

	while (W != N) {
		while (!V.empty()) {
			int x = *(V.rbegin());
			std::vector<int> toMove;

			for (int y : U) {
				if (hasEdge(x, y) || hasEdge(y, x)) {
					toMove.push_back(y);
				}
			}

			for (int y : toMove) {
				U.erase(y);
				V.insert(y);
				N_prime.insert(y);
			}

			V.erase(x);
			W.insert(x);
		}
		connectedComponents.push_back(std::vector<int>(N_prime.begin(), N_prime.end()));

		N_prime.clear();

		if (!U.empty()) {
			s = *(U.begin());
			U.erase(s);
			V.insert(s);
			p += 1;
			N_prime.insert(s);
		}
		else {
			break;
		}
	}

	return connectedComponents;
}

std::vector<Node*> Graph::topologicalSort(std::vector<Arch*>& resultEdgeUsed)
{

	std::unordered_set<Node*> visitedSet;
	std::vector<Node*> result;

	std::stack<std::pair<Node*, bool>> stack;

	for (Node* node : m_nodes) {
		if (!visitedSet.count(node)) {
			stack.push({ node, false });

			while (!stack.empty()) {
				auto currentPair = stack.top();
				stack.pop();
				Node* current = currentPair.first;
				bool visited = currentPair.second;

				if (!visited) {
					visitedSet.insert(current);
					stack.push({ current, true });

					for (Arch* arch : m_arches) {
						if (arch->getFirstNode() == current && !visitedSet.count(arch->getSecondNode()))
						{
							stack.push({ arch->getSecondNode(), false });
							resultEdgeUsed.push_back(arch);
						}
					}
				}
				else {
					result.push_back(current);
				}
			}
		}
	}
	std::reverse(result.begin(), result.end());
	return result;
}

Node* Graph::GetNod(QMouseEvent* e)
{
	for (Node* n : m_nodes)
	{
		if (fabs(n->getX() - e->pos().x()) < Node::DimNode &&
			fabs(n->getY() - e->pos().y()) < Node::DimNode)
			return n;
	}
	return nullptr;
}

Node* Graph::GetRoot()
{
	return this->m_root;
}

bool Graph::isOrientedOrNot()
{
	return isOriented;
}

bool Graph::verifyNodes(QMouseEvent* e)
{
	for (Node* n : m_nodes)
	{
		if (!n->getIsSelected())
			if (fabs(n->getX() - e->pos().x()) < Node::DimNode &&
				fabs(n->getY() - e->pos().y()) < Node::DimNode)
				return false;
	}
	return true;
}

bool Graph::verifyArches(Arch* ed)
{
	for (Arch* E : m_arches)
	{
		if (E->getFirstNode()->getCoordinate() == ed->getFirstNode()->getCoordinate() && E->getSecondNode()->getCoordinate() == ed->getSecondNode()->getCoordinate())
			return false;

		if (!isOrientedOrNot())
			if (E->getSecondNode()->getCoordinate() == ed->getFirstNode()->getCoordinate() && E->getFirstNode()->getCoordinate() == ed->getSecondNode()->getCoordinate())
				return false;
	}
	return true;
}

bool Graph::hasEdge(int node1, int node2)
{
	for (Arch* ed : m_arches)
	{
		if ((ed->getFirstNode()->getValue() == node1 && ed->getSecondNode()->getValue() == node2) ||
			(!isOrientedOrNot() && ed->getFirstNode()->getValue() == node2 && ed->getSecondNode()->getValue() == node1))
		{
			return true;
		}
	}

	return false;
}

bool Graph::hasCycles()
{
	std::unordered_set<Node*> visitedSet;
	std::unordered_set<Node*> onStackSet;
	std::stack<Node*> stack;

	for (Node* node : m_nodes) {
		if (!visitedSet.count(node)) {
			stack.push(node);

			while (!stack.empty()) {
				Node* current = stack.top();
				stack.pop();

				visitedSet.insert(current);
				onStackSet.insert(current);

				for (Arch* arch : m_arches) {
					if (arch->getFirstNode() == current) {
						Node* neighbor = arch->getSecondNode();

						if (!visitedSet.count(neighbor)) {
							stack.push(neighbor);
						}
						else if (onStackSet.count(neighbor)) {
							return true;
						}
					}
				}

				onStackSet.erase(current);
			}
		}
	}
	return false;
}

bool Graph::isTree()
{

	if (hasCycles()) {
		return false;
	}

	if (m_arches.size() != m_nodes.size() - 1) {
		return false;
	}

	return true;
}

bool Graph::compareArch(Arch* a, Arch* b)
{
	return a->getCost() < b->getCost();
}

int Graph::findSet(Node* node, std::vector<int>& parent)
{
	while (parent[node->getValue()-1] != -1) 
	{
		node->setValue(parent[node->getValue()]);
	}
	return node->getValue();
}

void Graph::unionSets(int set1, int set2, std::vector<int>& parent)
{
	parent[set1] = set2;
}

std::vector<Arch*> Graph::kruskalMST(std::vector<Node*> nodes, std::vector<Arch*> arches) {
	


	std::ranges::sort(arches, [](Arch* a, Arch* b) {
		return a->getCost() < b->getCost();
		});

	std::vector<Arch*> result;
	std::vector<int> parent;
	std::vector<int> rank;
	int numNodes = nodes.size();
	parent.resize(numNodes);
	rank.resize(numNodes);

	for (int i = 0; i < numNodes; ++i) { 
		parent[i] = i; 
		rank[i] = 0; 
	}

	for (Arch* edge : arches) {
		int srcParent = findA(parent, edge->getFirstNode()->getValue() - 1);  
		int destParent = findA(parent, edge->getSecondNode()->getValue() - 1); 
		 
		if (srcParent != destParent) { 
			result.push_back(edge); 
			Union(parent, rank, srcParent, destParent); 
			
		}
	}
	return result;
}

void Graph::saveConnectedComponents()
{
	std::vector<std::vector<int>> connectedComponents = findConnectedComponents();
	std::ofstream out("connectedComponents.txt");
	for (int i = 0; i < connectedComponents.size(); i++)
	{
		for (int j = 0; j < connectedComponents[i].size(); j++)
		{
			out << connectedComponents[i][j] << " ";
		}
		out << "\n";
	}
}

void Graph::saveMatrixUnoriented()
{
	std::ofstream fout("matrice.txt");

	fout << m_nodes.size() << "\n";
	for (int i = 1; i <= m_nodes.size(); i++)
	{
		fout << i << ":";
		for (Arch* ed : m_arches)
		{
			if (ed->getFirstNode()->getValue() == i)
				fout << ed->getSecondNode()->getValue() << ", ";
			else
				if (ed->getSecondNode()->getValue() == i)
					fout << ed->getFirstNode()->getValue() << ", ";
		}
		fout << "\n";
	}
	fout.close();
}

void Graph::saveMatrixOriented()
{
	std::ofstream fout("matrice.txt");

	fout << m_nodes.size() << "\n";
	for (int i = 1; i <= m_nodes.size(); i++)
	{
		fout << i << ":";
		for (Arch* ed : m_arches)
		{
			if (ed->getFirstNode()->getValue() == i)
				fout << ed->getSecondNode()->getValue() << ", ";
		}
		fout << "\n";
	}
	fout.close();
}

void Graph::WriteInFile(const std::vector<Node*>& vector)
{
	std::ofstream fout("text.txt");
	for (const auto& node : vector)
	{
		fout << node->getValue() << " ";
	}
	fout.close();
}

void Graph::reset()
{
	if (m_arches.size() != 0)
	{
		for (int i = 0; i < m_arches.size(); i++)
			delete m_arches[i];
		m_arches.resize(0);
	}

	if (m_nodes.size() != 0)
	{
		for (int i = 0; i < m_nodes.size(); i++)
			delete m_nodes[i];
		m_nodes.resize(0);
	}
}

int Graph::findA(std::vector<int>& parent, int i)
{
	{
		if (parent[i] != i)
			parent[i] = findA(parent, parent[i]); 
		return parent[i]; 
	} 
}

void Graph::Prim(std::vector<Arch*>& m_selectedArches)
{
	m_selectedArches.clear();

	std::priority_queue<std::pair<int, Arch*>> edgeQueue;

	for (Node* node : m_nodes) {
		node->setVisited(false);
	}

	Node* startNode = m_nodes.empty() ? nullptr : m_nodes[0];


	startNode->setVisited(true);

	for (Arch* arch : m_arches) {
		if (arch->getFirstNode() == startNode || arch->getSecondNode() == startNode) {
			edgeQueue.push({ -arch->getCost(), arch }); 
		}
	}

	while (!edgeQueue.empty()) {
		Arch* minArch = edgeQueue.top().second;
		edgeQueue.pop();

		Node* firstNode = minArch->getFirstNode();
		Node* secondNode = minArch->getSecondNode();

		if ((firstNode->getVisited() && !secondNode->getVisited()) ||
			(!firstNode->getVisited() && secondNode->getVisited())) {
			m_selectedArches.push_back(minArch);

			Node* newSelectedNode = firstNode->getVisited() ? secondNode : firstNode;
			newSelectedNode->setVisited(true);

			for (Arch* arch : m_arches) {
				if ((arch->getFirstNode() == newSelectedNode && !arch->getSecondNode()->getVisited()) ||
					(arch->getSecondNode() == newSelectedNode && !arch->getFirstNode()->getVisited())) {
					edgeQueue.push({ -arch->getCost(), arch }); 
				}
			}
		}
	}
}

void Graph::Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y)
{
	int xRoot = findA(parent, x);
	int yRoot = findA(parent, y);

	if (rank[xRoot] < rank[yRoot])
		parent[xRoot] = yRoot; 
	else if (rank[xRoot] > rank[yRoot]) 
		parent[yRoot] = xRoot;
	else {
		parent[yRoot] = xRoot;
		rank[xRoot]++;
	}
}
