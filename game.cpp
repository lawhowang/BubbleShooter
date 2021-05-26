#include "game.h"
#include "boardscene.h"

Game::Game(QGraphicsView * graphicsView)
{
    this->graphicsView = graphicsView;
    this->graphicsView->setMouseTracking(true);
    this->init();
}

Game::~Game()
{
    delete scene;
}

void Game::resetGame() {
    disconnect(scene, SIGNAL(restart()),
            this, SLOT(resetGame()));
    BoardScene * newScene = new BoardScene(graphicsView);
    connect(newScene, SIGNAL(restart()),
            this, SLOT(resetGame()));
    BoardScene * oldScene = qobject_cast<BoardScene*>(this->scene);
    scene = newScene;
    oldScene->deleteLater();
}

void Game::init() {
    scene = new BoardScene(graphicsView);
    connect(scene, SIGNAL(restart()),
            this, SLOT(resetGame()));
}

QGraphicsScene* Game::getScene() {
    return scene;
}
