#include "scene.h"
#include "bubble.h"

Scene::Scene(QGraphicsView * graphicsView)
{
    this->graphicsView = graphicsView;
    graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);
    graphicsView->setScene(this);
    QRectF exactRect(0, 0, 768, 1024);
    setSceneRect(exactRect);
    QBrush bg;
    bg.setColor(QColor(29, 32, 33));
    bg.setStyle(Qt::SolidPattern);
    setBackgroundBrush(bg);
}
