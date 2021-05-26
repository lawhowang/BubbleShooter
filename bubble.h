#ifndef BUBBLE_H
#define BUBBLE_H

#include <QGraphicsItem>
#include <QPainter>

enum Direction {
    TOP_LEFT, TOP_RIGHT, LEFT, RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NONE
};

enum Color {
    BLACK, RED, YELLOW, GREEN, BLUE, PURPLE, TRANSPARENT
};

class Bubble : public QGraphicsItem
{
public:
    Bubble(Color color, QGraphicsItem* parent = nullptr);
    Bubble(int row, int col, Color color, QGraphicsItem* parent = nullptr);
    ~Bubble() override;
    void setNeighbour(Direction direction, Bubble* neighbour);
    bool hasColoredNeighbour();
    Bubble** getNeighbours();
    Bubble* getNeighbour(Direction direction);
    Color getColor();
    void setColor(Color color);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;
    void eliminateTest(QVector<Bubble*>& traversed);
    void eliminate(QMap<int, QVector<Bubble*>>& bubbles, QVector<Bubble*>& fallingBubbles, int depth = 0);
    bool reachTop(QVector<Bubble*>& traversed);
    const static int width = 72;
    const static int height = 72;

    int row = 0;
    int col = 0;
private:
    Bubble ** neighbours;
    Color color = Color::BLACK;
    bool eliminated = false;
    bool fallen = false;
    void unlink();
};

#endif // BUBBLE_H
