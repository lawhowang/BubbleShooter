#include "resources.h"

Resources * Resources::singleton = nullptr;

Resources::Resources()
{
    resources["bubble_black"].load(":/images/Black.png");
    resources["bubble_red"].load(":/images/Red.png");
    resources["bubble_yellow"].load(":/images/Yellow.png");
    resources["bubble_green"].load(":/images/Green.png");
    resources["bubble_blue"].load(":/images/Blue.png");
    resources["bubble_purple"].load(":/images/Purple.png");
    resources["cloud"].load(":/images/cloud.png");
    resources["gameover"].load(":/images/gameover.png");
    resources["victory"].load(":/images/victory.png");
    resources["restart"].load(":/images/restart.png");
}

