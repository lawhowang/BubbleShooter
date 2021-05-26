#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>

class Scene : public QGraphicsScene
{
public:
    Scene(QGraphicsView * graphicsView);
protected:
    QGraphicsView * graphicsView = nullptr;
};

#endif // SCENE_H
