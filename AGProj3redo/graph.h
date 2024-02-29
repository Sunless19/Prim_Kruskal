#ifndef GRAPH_H
#define GRAPH_H
#include<set>
#include "arch.h"
#include "node.h"
#include <vector>
#include <QMouseEvent>
#include <fstream>

class Graph
{
public:
    Graph();
    Graph(Graph& g);
    ~Graph();
    void addNode(Node* n);
    void addNode(QPoint p);
    void addArch(Arch*);
    void addArch(Node*, Node*);
    void setOrientedOrNot(bool);
    void FindRoot();

    std::vector<Node*> getNodes();
    std::vector<Arch*> getArches();
    std::vector<Node*> getNeighbors(Node* node);
    std::vector<std::vector<int>> findConnectedComponents();
    std::vector<Node*> topologicalSort(std::vector<Arch*>& resultEdgeUsed);


    Node* GetNod(QMouseEvent* e);
    Node* GetRoot();
    bool isOrientedOrNot();

    bool verifyNodes(QMouseEvent* e);
    bool verifyArches(Arch* ed);
    bool hasEdge(int, int);
    bool hasCycles();
    bool isTree();
    bool compareArch(Arch* a, Arch* b);
    
    int findSet(Node* node, std::vector<int>& parent);
    void unionSets(int set1, int set2, std::vector<int>& parent);
    std::vector<Arch*> kruskalMST(std::vector<Node*> m_nodes, std::vector<Arch*> m_arches);
    void Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y);
    int findA(std::vector<int>& parent, int i);
       
    void Prim(std::vector<Arch*>& m_selectedArches);
    
    void saveConnectedComponents();
    void saveMatrixUnoriented();
    void saveMatrixOriented();
    void WriteInFile(const std::vector<Node*>& vector);
    void reset();


private:
    std::vector<Node*> m_nodes;
    std::vector<Arch*> m_arches;
    bool isOriented;
    Node* m_root;

};

#endif // GRAPH_H
