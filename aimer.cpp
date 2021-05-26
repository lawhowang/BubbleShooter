#include "aimer.h"
#include <QtMath>

Aimer::Aimer(QPointF& mousePos, QGraphicsScene * scene): scene(scene)
{
    QPen pen(QColor::fromRgb(255,255,255,128), 8, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    line = scene->addLine(scene->sceneRect().center().x(), scene->sceneRect().bottom() - Bubble::height/2-10, mousePos.x(), mousePos.y(), pen);
    this->updateMousePos(mousePos);
}

Aimer::~Aimer()
{
    delete line;
}

qreal Aimer::getAngle() {
    return angle;
}

void Aimer::updateMousePos(QPointF &mousePos) {
    this->mousePos.setX(mousePos.x());
    this->mousePos.setY(mousePos.y());
    QGraphicsLineItem * mainLineItem = line;
    //  Calculate angle
    QLineF tmp(scene->sceneRect().center().x(), scene->sceneRect().bottom() - Bubble::height/2 - 10, mousePos.x(), mousePos.y());
    qreal angle = tmp.angle();
    if (angle < 25 || angle > 270) {
        angle = 25;
    }
    if (angle > 155 && angle <= 270) {
        angle = 155;
    }
    this->angle = angle;
    QLineF l1 = mainLineItem->line();
    l1.setAngle(angle);
    l1.setLength(200);
    mainLineItem->setLine(l1);
}
