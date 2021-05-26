#include "boardscene.h"
#include <QRandomGenerator>
#include <QTimer>
#include <QVector>
#include <QtMath>
#include <QPushButton>
#include <QGraphicsSceneMouseEvent>
#include "resources.h"

BoardScene::BoardScene(QGraphicsView* graphicsView):Scene(graphicsView) {
    // paintBg();

    initBoard();
    initShooter();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateBubbles()));
    timer->start(10);
}

BoardScene::~BoardScene() {
    disconnect(timer, SIGNAL(timeout()), this, SLOT(updateBubbles()));
    timer->stop();
    delete timer;
    delete aimer;
    delete shootingBubble;
    delete clouds[0];
    delete clouds[1];
    delete clouds[2];
    delete clouds[3];
    delete [] clouds;
    delete shootableArea;
    delete filter;
    delete popUp;
    delete restartButton;
    for (const auto bubble: bubbleList) {
        delete bubble;
    }
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            delete bubbles[row][col];
        }
        delete [] bubbles[row];
    }
    delete[] bubbles;
}

void BoardScene::initBoard() {
    this->bubbles = new Bubble **[rows];
    for (int row = 0; row < rows; row++) {
        bubbles[row] = new Bubble *[cols];
        for (int col = 0; col < cols; col++) {
            Color randomColor = static_cast<Color>(QRandomGenerator::global()->generate() % 7);
            if (row >= 9) {
                randomColor = TRANSPARENT;
            }
            if (randomColor != TRANSPARENT) {
                bubbleCount++;
            }
            bubbles[row][col] = new Bubble(row, col, randomColor);
        }
    }
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            bool isEven = row % 2 == 0;

            if (row != 0 && (!isEven || col != 0)) {
                bubbles[row][col]->setNeighbour(TOP_LEFT, bubbles[row - 1][col - isEven]);
            }
            if (row != 0 && (isEven || col != cols - 1)) {
                bubbles[row][col]->setNeighbour(TOP_RIGHT, bubbles[row - 1][col + !isEven]);
            }
            if (col != 0) {
                bubbles[row][col]->setNeighbour(LEFT, bubbles[row][col - 1]);
            }
            if (col != 8) {
                bubbles[row][col]->setNeighbour(RIGHT, bubbles[row][col + 1]);
            }
            if (row != rows - 1 && (!isEven || col != 0)) {
                bubbles[row][col]->setNeighbour(BOTTOM_LEFT, bubbles[row + 1][col - isEven]);
            }
            if (row != rows - 1 && (isEven || col != cols - 1)) {
                bubbles[row][col]->setNeighbour(BOTTOM_RIGHT, bubbles[row + 1][col + !isEven]);
            }
            int centerX = sceneRect().center().x();
            int x = centerX + (col - 4) * (Bubble::width + 10); // 5 is the margin
            int offsetX = row % 2 == 1 ? (Bubble::width+10) * 0.25 : (Bubble::width+10) * -0.25;
            bubbles[row][col]->setPos(x + offsetX, Bubble::height/2 + row * Bubble::height);
            addItem(bubbles[row][col]);
        }
    }
    vanishingDepth = 0;
}

void BoardScene::initShooter() {
    QPointF p(sceneRect().center().x(), 0);
    aimer = new Aimer(p, this);
    int size = 5;
    for (int i = 0; i < size; i++) {
        Color randomColor = static_cast<Color>(QRandomGenerator::global()->generate() % 6);
        Bubble * b = new Bubble(randomColor);

        int centerX = sceneRect().center().x();
        int x = centerX - i * (Bubble::width + 10); // 10 is the margin
        b->setPos(x, sceneRect().bottom() - Bubble::height/2 - 10);
        addItem(b);

        bubbleList.append(b);
    }
}

void BoardScene::paintBg() {
    QLinearGradient gradient;
    gradient.setCoordinateMode(QLinearGradient::ObjectBoundingMode);
    gradient.setStart(0.0, 0.0);
    gradient.setFinalStop(0.0, 1.0);
    gradient.setColorAt(0.0, QColor::fromRgb(27, 50, 104));
    gradient.setColorAt(0.45, QColor::fromRgb(58, 92, 163));
    gradient.setColorAt(1.0, QColor::fromRgb(139, 60, 162));
    setBackgroundBrush(gradient);

    if (clouds == nullptr) {
        clouds = new QGraphicsPixmapItem*[3];
        clouds[0] = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("cloud"));
        clouds[1] = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("cloud"));
        clouds[2] = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("cloud"));
        clouds[3] = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("cloud"));
        clouds[0]->setPos(sceneRect().left() - clouds[0]->boundingRect().width(), -400);
        clouds[1]->setPos(sceneRect().right(), -200);
        clouds[2]->setPos(sceneRect().left() - clouds[2]->boundingRect().width(), 0);
        clouds[3]->setPos(sceneRect().right(), 200);
        addItem(clouds[0]);
        addItem(clouds[1]);
        addItem(clouds[2]);
        addItem(clouds[3]);
    }
    if (shootableArea == nullptr) {
        shootableArea = new QGraphicsRectItem(sceneRect());
        shootableArea->setPen(Qt::NoPen);
        QBrush brush(QColor(0,0,0,25));
        shootableArea->setBrush(brush);
        addItem(shootableArea);
    }
    if (popUp != nullptr && filter == nullptr) {
        filter = new QGraphicsRectItem(sceneRect());
        filter->setPen(Qt::NoPen);
        QBrush brush(QColor(0,0,0,175));
        filter->setBrush(brush);
        filter->setZValue(90);
        addItem(filter);
    }

    // Move clouds
    for (int i = 0; i < 4; i++) {
        qreal direction = i % 2 ? -1 : 1;
        qreal speed = (i + 1) * 0.5 * direction;
        clouds[i]->setPos(clouds[i]->pos().x() + speed, clouds[i]->pos().y());
        if (clouds[i]->x() < sceneRect().left() - clouds[i]->boundingRect().width() || clouds[i]->x() > sceneRect().right()) {
            clouds[i]->setOpacity(clouds[i]->opacity() - 0.05);
        } else {
            clouds[i]->setOpacity(clouds[i]->opacity() + 0.05);
        }
        if (clouds[i]->opacity() <= 0) {
            if (i % 2 == 0)
                clouds[i]->setPos(sceneRect().left() - clouds[i]->boundingRect().width(), i * 200);
            else
                clouds[i]->setPos(sceneRect().right(), (i - 2) * 200);
        }
    }
}

void BoardScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    double mouseX = mouseEvent->scenePos().x();
    double mouseY = mouseEvent->scenePos().y();
    mousePos.setX(mouseX);
    mousePos.setY(mouseY);
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
    if (gameState == IDLE) {
        // Left click to shoot bubble
        if (mouseEvent->buttons() & Qt::LeftButton)
        {
            gameState = PAUSE;
            shootingAngle = aimer->getAngle();
            // Clone the first bubble in the bubble list
            shootingBubble = new Bubble(-1, -1, bubbleList.first()->getColor());
            shootingBubble->setPos(bubbleList.first()->pos());
            shootingBubble->setZValue(89);
            addItem(shootingBubble);
            bubbleCount++; // Add the number of total bubbles by 1

            // Move the color of the bubbles in the bubble list
            QVector<Bubble*>::iterator iter = bubbleList.begin();
            for(auto iter = bubbleList.begin(); iter != bubbleList.end(); ++iter) {
                Bubble * curr = *iter;
                if (iter == bubbleList.end() - 1) {
                    Color randomColor = static_cast<Color>(QRandomGenerator::global()->generate() % 6);
                    curr->setColor(randomColor);
                }  else {
                    Bubble * next = *(iter + 1);
                    curr->setColor(next->getColor());
                }
            }
        }
        // Right click to swap bubbles
        if (mouseEvent->buttons() & Qt::RightButton) {
            // Swap first and second
            Bubble * first = bubbleList.first();
            Bubble * second = bubbleList.at(1);
            Color temp = first->getColor();
            first->setColor(second->getColor());
            second->setColor(temp);
        }
    } else if (gameState == VICTORY || gameState == GAMEOVER) {
        if (restartButton != nullptr && restartButton->isUnderMouse()) {
            emit restart();
        }
    }
}

Bubble* lastCollidedTransparentBubble = nullptr;
void BoardScene::updateBubbles()
{
    if (shootingBubble != nullptr) {
        if (shootingBubble->x() - shootingBubble->boundingRect().width() / 2 < sceneRect().left() || shootingBubble->x() + shootingBubble->boundingRect().width() / 2 > sceneRect().right()) {
            shootingAngle = 180 - shootingAngle;
        }
        shootingBubble->moveBy(qCos(qDegreesToRadians(shootingAngle)) * 15, -qSin(qDegreesToRadians(shootingAngle)) * 15);
        QList<QGraphicsItem*> collidingItems = shootingBubble->collidingItems();

        if (shootingBubble->y() < sceneRect().top()) {
            collidingItems.push_back(lastCollidedTransparentBubble);
        }
        for (const auto item : collidingItems) {
            if (Bubble* bubble = dynamic_cast<Bubble*>(item)) {
                if (bubble->row != -1) {
                    if (bubble->getColor() != TRANSPARENT || shootingBubble->y() < sceneRect().top()) {
                        if (lastCollidedTransparentBubble != nullptr) {
                            lastCollidedTransparentBubble->setColor(shootingBubble->getColor());
                            QVector<Bubble*> traversed;
                            lastCollidedTransparentBubble->eliminateTest(traversed);
                            if (traversed.size() >= 3) {
                                lastCollidedTransparentBubble->eliminate(vanishingBubbles, fallingBubbles);
                            }
                        } else {
                            if (gameState != GAMEOVER) {
                                showRestartButton = true;
                            }
                            gameState = GAMEOVER;
                        }
                        delete shootingBubble;
                        shootingBubble = nullptr;
                        lastCollidedTransparentBubble = nullptr;
                        break;
                    } else {
                        lastCollidedTransparentBubble = bubble;
                    }
                }
            }
        }
    }

    if (gameState == GAMEOVER) {
        if (popUp == nullptr) {
            popUp = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("gameover"));
            popUp->setOpacity(0.1);
            popUp->setPos(QPointF(sceneRect().center().x() - popUp->boundingRect().width() / 2, sceneRect().top() - 100));
            popUp->setZValue(91);
            addItem(popUp);
        }
        if (popUp->y() <= sceneRect().center().y() - popUp->boundingRect().height() / 2) {
            popUp->setPos(popUp->pos().x(), popUp->pos().y() + 10);
        }
        if (popUp->opacity() < 1) {
            popUp->setOpacity(popUp->opacity() * 1.1);
        }
    }
    if (gameState == IDLE || gameState == PAUSE) {
        // Moving aimer direction
        aimer->updateMousePos(mousePos);
    }


    // Background
    paintBg();

    // Animate the eliminated bubbles
    if (!vanishingBubbles[vanishingDepth].empty()) {
        const QVector<Bubble*> &bubbles = vanishingBubbles[vanishingDepth];
        bool nextLevel = true;
        for (auto& b : bubbles) {
            b->setScale(b->scale() * 0.85);
            if (b->scale() < 0.1) {
                // Reset back to its original state
                b->setColor(TRANSPARENT);
                b->setScale(1);
            } else {
                // Still have bubbles animating
                nextLevel = false;
            }
        }
        if (nextLevel) {
            bubbleCount -= vanishingBubbles[vanishingDepth].count();
            vanishingBubbles[vanishingDepth].clear();
            vanishingDepth++;
        }
    } else {
        vanishingDepth = 0;
        vanishingBubbles.clear();
        // Animate the bubbles to be fallen down
        if (fallingBubbles.size() > 0) {
            bool finish = true;
            for (auto& b: fallingBubbles) {
                QTransform t = b->transform();
                t.translate(0, t.m32() + 1);
                b->setTransform(t);
                b->setOpacity(b->opacity() * 0.99);
                if (b->transform().m32() + b->pos().y() > sceneRect().bottom()) {
                    // Reset back to its original state
                    QTransform t = b->transform();
                    t.reset();
                    b->setTransform(t);
                    b->setOpacity(1);
                    b->setColor(TRANSPARENT);
                } else {
                    // Still have bubbles animating
                    finish = false;
                }
            }
            if (finish) {
                // All bubbles were animated
                bubbleCount -= fallingBubbles.count();
                fallingBubbles.clear();
            }
        } else {
            if (gameState == PAUSE && shootingBubble == nullptr)
                gameState = IDLE;
        }
    }

    if (bubbleCount == 0) {
        // Won
        gameState = VICTORY;
        showRestartButton = true;
    }

    if (gameState == VICTORY) {
        if (popUp == nullptr) {
            popUp = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("victory"));
            popUp->setOpacity(0.1);
            popUp->setPos(QPointF(sceneRect().center().x() - popUp->boundingRect().width() / 2, sceneRect().top() - 100));
            popUp->setZValue(91);
            addItem(popUp);
        }
        if (popUp->y() <= sceneRect().center().y() - popUp->boundingRect().height() / 2) {
            popUp->setPos(popUp->pos().x(), popUp->pos().y() + 10);
        }
        if (popUp->opacity() < 1) {
            popUp->setOpacity(popUp->opacity() * 1.1);
        }
    }

    if (showRestartButton) {
        if (restartButton == nullptr) {
            restartButton = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("restart"));
            restartButton->setPos(QPointF(sceneRect().center().x() - restartButton->boundingRect().width() / 2, sceneRect().center().y() + 180));
            restartButton->setZValue(101);
            addItem(restartButton);
        }
    }
}
