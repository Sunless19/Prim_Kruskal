#ifndef NODE_H
#define NODE_H

#include <QPainter>
class Node
{
    int value;
    QPoint coordinate;
    bool isSelected;
    bool visited = false;
    QColor color;
    int cost;
public:
    Node();
    Node(QPoint, int);
    Node(Node*);
    ~Node();
    void setValue(int value);
    void setCoordinate(QPoint);
    void setNode(Node* val);
    void setIsSelected(bool);
    void setCoordinate(int x, int y);
    QPoint getCoordinate();
    int getValue();
    int getX();
    int getY();
    bool getIsSelected();
    static const int DimNode = 50;
    void setVisited(bool visited);
    bool getVisited();
    void setColor(const QColor& color);
};

#endif // NODE_H
