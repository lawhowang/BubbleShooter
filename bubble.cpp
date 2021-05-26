#include "bubble.h"
#include "resources.h"
#include <QVector>
#include <QQueue>

Bubble::Bubble(int row, int col, Color color, QGraphicsItem * parent):QGraphicsItem(parent)
{
    this->row = row;
    this->col = col;
    neighbours = new Bubble*[6];
    for (int i = 0; i < 6; i++) {
        neighbours[i] = nullptr;
    }
    this->color = color;
}

Bubble::Bubble(Color color, QGraphicsItem * parent):Bubble(-1, -1, color, parent)
{
}

Bubble::~Bubble()
{
    unlink();
    delete [] neighbours;
}

void Bubble::unlink() {
    this->color = TRANSPARENT;
}

bool Bubble::hasColoredNeighbour() {
    for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
        if (neighbours[dir] != nullptr && neighbours[dir]->color != TRANSPARENT) {
            return true;
        }
    }
    return false;
}

void Bubble::setNeighbour(Direction direction, Bubble* neighbour) {
    neighbours[direction] = neighbour;
}

Bubble** Bubble::getNeighbours() {
    return neighbours;
}

Bubble * Bubble::getNeighbour(Direction direction) {
    return neighbours[direction];
}

Color Bubble::getColor() {
    return color;
}

void Bubble::setColor(Color color) {
    this->color = color;
    this->eliminated = false;
}

QRectF Bubble::boundingRect() const {
    return QRect(-width/2.0, -height/2.0, width, height);
}

/* The shape of the bubble affects collision detection
 */
QPainterPath Bubble::shape() const
{
    QPainterPath path;
    if (row == -1) {
        // The shooting bubble is having a smaller size to make it fit into the gap among the bubble
        path.addEllipse(QRect(-width*0.25, -height*0.25, width*0.5, height*0.5));
    } else {
        path.addEllipse(QRect(-width/2.0, -height/2.0, width, height));
    }
    return path;
}

void Bubble::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPixmap * bubbleImage = nullptr;
    switch (color) {
    case BLACK:
        bubbleImage = Resources::getInstance()->getResource("bubble_black");
    break;
    case RED:
        bubbleImage = Resources::getInstance()->getResource("bubble_red");
    break;
    case YELLOW:
        bubbleImage = Resources::getInstance()->getResource("bubble_yellow");
    break;
    case GREEN:
        bubbleImage = Resources::getInstance()->getResource("bubble_green");
    break;
    case BLUE:
        bubbleImage = Resources::getInstance()->getResource("bubble_blue");
    break;
    case PURPLE:
        bubbleImage = Resources::getInstance()->getResource("bubble_purple");
    break;
    case TRANSPARENT:
        return;
    }
    if (bubbleImage != nullptr)
        painter->drawPixmap(-width/2.0, -height/2.0, width, height, *bubbleImage);
}

bool Bubble::reachTop(QVector<Bubble*>& traversed) {
    if (this->eliminated) {
        return false;
    }
    if (row == 0) {
        return true;
    }
    if (traversed.contains(this)) {
        return false;
    }
    traversed.push_back(this);
    for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
        if (neighbours[dir] != nullptr && neighbours[dir]->color != TRANSPARENT) {
            if (neighbours[dir]->reachTop(traversed)) {
                return true;
            }
        }
    }
    return false;
}

void Bubble::eliminateTest(QVector<Bubble*>& traversed) {
    if (traversed.contains(this)) {
        return;
    }
    traversed.push_back(this);
    for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
        if (neighbours[dir] != nullptr && neighbours[dir]->color == this->color) {
            neighbours[dir]->eliminateTest(traversed);
        }
    }
}

/* Reference: https://stackoverflow.com/questions/31247634/how-to-keep-track-of-depth-in-breadth-first-search
 * For keep tracking depth in BFS way
 */
void Bubble::eliminate(QMap<int, QVector<Bubble*>> &eliminatedBubbles, QVector<Bubble*>& fallingBubbles, int level) {
    QQueue<Bubble*> queue;
    queue.enqueue(this);
    queue.enqueue(nullptr);
    QVector<Bubble*> surroundings; // Neighbours of the eliminated bubbles
    this->eliminated = true;
    while (!queue.empty()) {
        Bubble * b = queue.dequeue();
        if (b == nullptr) {
            level++;
            queue.enqueue(nullptr);
            if (queue.head() == nullptr) break;
            else continue;
        }
        eliminatedBubbles[level].push_back(b);

        for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
            if (b->neighbours[dir] != nullptr && b->neighbours[dir]->getColor() != TRANSPARENT && !b->neighbours[dir]->eliminated) {
                if (b->neighbours[dir]->getColor() == color) {
                    b->neighbours[dir]->eliminated = true;
                    queue.enqueue(b->neighbours[dir]);
                } else {
                    if (!surroundings.contains(b->neighbours[dir])) {
                        surroundings.push_back(b->neighbours[dir]);
                    }
                }
            }
        }
    }
    // Falling bubbles must have been attached to the bubbles that were just removed
    // Reference https://gamedev.stackexchange.com/questions/27927/bubble-shooter-falling-algorithm
    for (auto & s : surroundings) {
        QVector<Bubble*> temp;
        bool rt = s->reachTop(temp);
        if (!rt) {
            for (auto & t : temp) {
                t->eliminated = true;
            }
            fallingBubbles.append(temp);
        }
    }
}
