#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    game = new Game(ui->graphicsView);
}

MainWindow::~MainWindow()
{
    delete game;
    delete ui;
}

void MainWindow::resize()
{
    ui->graphicsView->fitInView(game->getScene()->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::resizeEvent(QResizeEvent * e) {
    resize();
    QMainWindow::resizeEvent(e);
}

bool MainWindow::event(QEvent * event)
{
    if (event->type() == QEvent::Show)
        resize();

    return QMainWindow::event(event);
}
