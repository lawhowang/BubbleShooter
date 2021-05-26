#ifndef BOARDSCENE_H
#define BOARDSCENE_H


#include <QGraphicsScene>
#include <QGraphicsView>
#include "scene.h"
#include "bubble.h"
#include "aimer.h"

enum GameState {
    IDLE, GAMEOVER, VICTORY, PAUSE
};

class BoardScene : public Scene
{
    Q_OBJECT
public:
    BoardScene(QGraphicsView* graphicsView);
    ~BoardScene();
signals:
    void restart();
private:
    const int rows = 13;
    const int cols = 9;
    int bubbleCount = 0;
    Bubble*** bubbles;
    QVector<Bubble*> bubbleList;
    QMap<int, QVector<Bubble*>> vanishingBubbles;
    QVector<Bubble*> fallingBubbles;
    int vanishingDepth = 0;
    void initBoard();
    void initShooter();
    void paintBg();
    QTimer *timer;
    Aimer *aimer;
    Bubble* shootingBubble = nullptr;
    qreal shootingAngle;
    QPointF mousePos;
    QGraphicsRectItem *shootableArea = nullptr;
    QGraphicsPixmapItem **clouds = nullptr;
    QGraphicsPixmapItem *popUp = nullptr;
    QGraphicsRectItem *filter = nullptr;
    GameState gameState = IDLE;
    bool showRestartButton = false;
    QGraphicsPixmapItem *restartButton = nullptr;
protected slots:
    void updateBubbles();
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
};

#endif // BOARDSCENE_H
