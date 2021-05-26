#ifndef AIMER_H
#define AIMER_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "bubble.h"

class Aimer
{
public:
    Aimer(QPointF& mousePos, QGraphicsScene * scene);
    ~Aimer();
    void updateMousePos(QPointF &mousePos);
    qreal getAngle();
private:
    QPointF mousePos;
    QGraphicsLineItem* line;
    QGraphicsScene* scene;
    qreal angle = 90.0;
};

#endif // AIMER_H
