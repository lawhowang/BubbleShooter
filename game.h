#ifndef GAME_H
#define GAME_H

#include <QGraphicsScene>
#include <QGraphicsView>

class Game : public QObject
{
    Q_OBJECT
public:
    Game(QGraphicsView * graphicsView);
    ~Game();
    QGraphicsScene * getScene();
public slots:
    void resetGame();
private:
    QGraphicsView * graphicsView = nullptr;
    QGraphicsScene * scene = nullptr;
    void init();
};

#endif // GAME_H
