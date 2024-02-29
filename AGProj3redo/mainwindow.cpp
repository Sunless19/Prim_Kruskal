#include "mainwindow.h"
#include <QSet>
#include <QQueue>
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QInputDialog>
MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, secondNode(nullptr)
	, firstNode(nullptr)
	, MousePressed(false)
	, currentPaintEvent(start)

{
	ui->setupUi(this);
	m_colors = { Qt::blue, Qt::red, Qt::green, Qt::yellow, Qt::magenta, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkYellow,
		Qt::darkCyan, Qt::darkMagenta, Qt::darkGray };
	connect(ui->pushButton_3, & QPushButton::clicked, this, & MainWindow::on_kruskal_clicked);
	connect(ui->pushButton_4, & QPushButton::clicked, this, & MainWindow::on_prim_clicked);
}


MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::RightButton)
	{
		if (graf.verifyNodes(e))
			graf.addNode(e->pos());
		else
		{
			for (Node* n : graf.getNodes())
			{
				if (fabs(n->getX() - e->pos().x()) < Node::DimNode / 2.0 &&
					fabs(n->getY() - e->pos().y()) < Node::DimNode / 2.0)
					if (firstNode == nullptr)
					{
						firstNode = n;
						firstNode->setIsSelected(true);
					}
					else
					{
						secondNode = n;
						if (firstNode->getValue() != secondNode->getValue())
						{
							Arch* ed = new Arch(firstNode, secondNode);
							if (graf.verifyArches(ed))
							{
								bool ok;
								QString text = QInputDialog::getText(nullptr, "Get Number", "Enter a number:", QLineEdit::Normal, "", &ok);
								int number;
								if (ok && !text.isEmpty()) {
									bool conversionOk = false;
									number = text.toInt(&conversionOk);

									if (conversionOk) {
										qDebug() << "Entered number:" << number;
									}
									else {
										qDebug() << "Invalid number format entered.";
									}
								}
								else {
									qDebug() << "User canceled the input.";
								}
								ed->setCost(number);
								graf.addArch(ed);
							}
							else
								delete ed;
						}
						firstNode->setIsSelected(false);
						firstNode = nullptr;
						secondNode = nullptr;
					}
			}
		}
	}
	else
		if (e->button() == Qt::LeftButton)
		{
			for (Node* n : graf.getNodes())
			{
				if (fabs(n->getX() - e->pos().x()) < Node::DimNode / 2.0 &&
					fabs(n->getY() - e->pos().y()) < Node::DimNode / 2.0)
				{
					n->setIsSelected(true);
					MousePressed = true;
					break;
				}
			}
		}
	update();
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
	if (MousePressed)
		if (Qt::LeftButton)
		{
			for (Node* n : graf.getNodes())
				if (n->getIsSelected())
				{
					if (graf.verifyNodes(e))
						n->setCoordinate(e->pos());
				}
			update();
		}
}
void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		MousePressed = false;
		for (Node* n : graf.getNodes())
		{
			n->setIsSelected(false);
		}
	}
}

void MainWindow::paintNode(QPainter& p, Node* n)
{
	if (n->getIsSelected())
		p.setPen(Qt::red);
	else
		p.setPen(Qt::black);

	QRect r(n->getX() - Node::DimNode / 2, n->getY() - Node::DimNode / 2, Node::DimNode, Node::DimNode);
	p.drawEllipse(r);
	QString s;
	s.setNum(n->getValue());
	p.drawText(r, Qt::AlignCenter, s);
}

void MainWindow::paintNodeColor(QPainter& p, Node* n, QColor x)
{
	p.setPen(Qt::black);
	p.setBrush(x);
	QRect r(n->getX() - Node::DimNode / 2, n->getY() - Node::DimNode / 2, Node::DimNode, Node::DimNode);
	p.drawEllipse(r);
	QString s;
	s.setNum(n->getValue());
	p.drawText(r, Qt::AlignCenter, s);
}

void MainWindow::colorConnectedComponents()
{
	for (Node* n : graf.getNodes())
		n->setVisited(false);

	int colorIndex = 0;

	for (Node* n : graf.getNodes())
	{
		if (!n->getVisited())
		{
			// Perform a DFS to find connected component
			QStack<Node*> stack;
			stack.push(n);

			while (!stack.isEmpty())
			{
				Node* currentNode = stack.pop();

				if (!currentNode->getVisited())
				{
					currentNode->setVisited(true);
					currentNode->setColor(m_colors[colorIndex]);
					for (Node* neighbor : graf.getNeighbors(currentNode))
					{
						if (!neighbor->getVisited())
							stack.push(neighbor);
					}
				}
			}

			// Move to the next color for the next connected component
			colorIndex = (colorIndex + 1) % m_colors.size();
		}
	}

	update();
}

void MainWindow::on_pushButton_clicked()
{
	std::vector<std::vector<int>> connectedComponents = graf.findConnectedComponents();
	graf.saveConnectedComponents();

	if (ui->radioButton3->isChecked())
		currentPaintEvent = AfterButtonPress;
	update();
	//Asta daca n-o apelez degeaba .
}

void MainWindow::TopologicalSort()
{

	if (ui->pushButton->isDown())
	{
		if (!graf.hasCycles())
		{
			graf.WriteInFile(graf.topologicalSort(resultEdgeUsed));
			PopUpWindow* a = new PopUpWindow(graf.getNodes(), resultEdgeUsed, this);
			a[0].resize(900, 900);
			a[0].show();
		}
		else
			qDebug() << "Has cycles";
	}
}

void MainWindow::on_kruskal_clicked()
{
	std::vector<Arch*> result;
	result = graf.kruskalMST(graf.getNodes(),graf.getArches());
	PopUpWindow* b = new PopUpWindow(graf.getNodes(), graf.kruskalMST(graf.getNodes(), graf.getArches()), this);
	b[0].resize(1100, 1100);
	b[0].show();
}

void MainWindow::on_prim_clicked()
{
	std::vector<Arch*> result; 
	graf.Prim(result);
	PopUpWindow* b = new PopUpWindow(graf.getNodes(), result, this); 
	b[0].resize(1100, 1100); 
	b[0].show(); 
}



void MainWindow::ClickedButton()
{
	graf.setOrientedOrNot(false);

	/*if (ui->radioButton3->isChecked())
	{

		graf.setOrientedOrNot(false);
		graf.saveMatrixUnoriented();
		graf.saveConnectedComponents();
	}

	if (graf.isOrientedOrNot())
	{
		if (ui->radioButton1->isChecked())
		{
			graf.saveConnectedComponents();
			graf.saveMatrixOriented();
			graf.saveConnectedComponents();
		}

		else
			if (ui->radioButton2->isChecked())
			{
				graf.setOrientedOrNot(false);
				graf.saveConnectedComponents();
				graf.reset();
			}
	}
	else
	{
		if (ui->radioButton1->isChecked())
		{
			graf.setOrientedOrNot(true);
			graf.saveConnectedComponents();
			graf.reset();
		}
		else
			if (ui->radioButton2->isChecked())
			{
				graf.saveConnectedComponents();
				graf.saveMatrixUnoriented();
			}
	}*/

}

void MainWindow::drawLine(Node* N1, Node* N2, int radius, QPainter& p, int cost)
{
	p.setPen(Qt::black);
	qreal dx = N2->getX() - N1->getX();
	qreal dy = N2->getY() - N1->getY();
	qreal distance = qSqrt(dx * dx + dy * dy);
	qreal x1_intersec = N1->getX() + (dx * radius / distance);
	qreal y1_intersec = N1->getY() + (dy * radius / distance);
	qreal x2_intersec = N2->getX() - (dx * radius / distance);
	qreal y2_intersec = N2->getY() - (dy * radius / distance);

	p.drawLine(x1_intersec, y1_intersec, x2_intersec, y2_intersec);

	qreal midX = (x1_intersec + x2_intersec) / 2;
	qreal midY = (y1_intersec + y2_intersec) / 2;
	// Desenează un dreptunghi la mijlocul liniei
	qreal rectangleWidth = 35;
	qreal rectangleHeight = 35;
	qreal rectX = midX - rectangleWidth / 2;
	qreal rectY = midY - rectangleHeight / 2;
	p.setBrush(Qt::blue);
	QRect r(rectX, rectY, rectangleWidth, rectangleHeight);
	p.drawRect(r);
	QString s;
	s.setNum(cost);
	p.drawText(r, Qt::AlignCenter, s);

}

void MainWindow::drawLineColor(Node* N1, Node* N2, int radius, QPainter& p, QColor x)
{
	p.setPen(x);
	qreal dx = N2->getX() - N1->getX();
	qreal dy = N2->getY() - N1->getY();
	qreal distance = qSqrt(dx * dx + dy * dy);
	qreal x1_intersec = N1->getX() + (dx * radius / distance);
	qreal y1_intersec = N1->getY() + (dy * radius / distance);
	qreal x2_intersec = N2->getX() - (dx * radius / distance);
	qreal y2_intersec = N2->getY() - (dy * radius / distance);

	p.drawLine(x1_intersec, y1_intersec, x2_intersec, y2_intersec);
}

void MainWindow::drawArrow(QPoint Q1, QPoint Q2, QPainter& p)
{
	qreal arrowSize = Node::DimNode / 2.0;
	p.setBrush(Qt::black);
	QLineF line(Q2, Q1);

	double angle = std::atan2(-line.dy(), line.dx());
	QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
	QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

	QPolygonF arrowHead;
	arrowHead.clear();
	arrowHead << line.p1() << arrowP1 << arrowP2;
	p.drawPolygon(arrowHead);

	p.setBrush(Qt::white);
}

void MainWindow::paintEvent(QPaintEvent* e)
{

	QPainter p(this);
	if (currentPaintEvent == start)
	{
		for (Node* n : graf.getNodes())
			paintNode(p, n);

		for (Arch* ed : graf.getArches())
			drawLine(ed->getFirstNode(), ed->getSecondNode(), Node::DimNode / 2, p, ed->getCost());

		ClickedButton();
		//TopologicalSort();
		update();
	}

	//if (ui->pushButton_2->isDown())
	//{
	//	graf.FindRoot();
	//	if (graf.GetRoot() != nullptr)
	//		currentPaintEvent = Root;
	//}

	//if (currentPaintEvent == Root)
	//{
	//	Node* x = graf.GetRoot();
	//	/*for (Node* n : graf.getNodes())
	//		if (n->getValue() == x->getValue()) {
	//			paintNodeColor(p, n,Qt::green);
	//		}
	//		else paintNode(p, n);*/
	//	paintNodeColor(p, x,Qt::green);

	//	/*for (Arch* ed : graf.getArches())
	//		drawLine(ed->getFirstNode(), ed->getSecondNode(), Node::DimNode / 2, p);*/

	//	ClickedButton();
	//	TopologicalSort();
	//	update();
	//}

	//if (ui->radioButton3->isChecked())
	//	currentPaintEvent = AfterButtonPress;

	//if (currentPaintEvent == AfterButtonPress)
	//{
	//	int i = 0;
	//	for (auto x : graf.findConnectedComponents())
	//	{
	//		for (int y = 0; y < x.size(); y++)
	//		{
	//			for (Node* n : graf.getNodes())
	//				if (n->getValue() == x[y])
	//					paintNodeColor(p, n, m_colors[i]);
	//			for (Arch* ed : graf.getArches())
	//				drawLine(ed->getFirstNode(), ed->getSecondNode(), Node::DimNode / 2, p);
	//		}
	//		if (i != m_colors.size())
	//			i += 1;
	//		else i = 0;
	//	}
	//	update();
	//	ClickedButton();
	//}
}

