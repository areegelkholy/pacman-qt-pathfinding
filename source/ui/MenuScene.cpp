#include "MenuScene.h"
#include <QGraphicsProxyWidget>
#include <QFont>
#include <QGraphicsPixmapItem>

// Sets up the main scene
MenuScene::MenuScene(QObject *parent)
    : QGraphicsScene(parent)
{
    QPixmap bg(":/game_objects/map_objects/back.png");
    QPixmap scaledBg = bg.scaled(800, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    setSceneRect(0, 0, 800, 600);
    addPixmap(scaledBg);

    startButton = new QPushButton("Start Game");
    startButton->setFont(QFont("Arial", 28, QFont::Bold));
    startButton->setFixedSize(240, 70);
    startButton->setStyleSheet(
        "QPushButton {"
        " background-color: #FFEA00;"
        " color: black;"
        " font-size: 28px;"
        " font-weight: bold;"
        " padding: 10px 20px;"
        " border: 2px solid white;"
        " border-radius: 0px;"
        "}"
        "QPushButton:hover {"
        " background-color: white;"
        " color: #333;"
        " border: 2px solid #FFD700;"
        "}"
        );


    QGraphicsProxyWidget *proxy = addWidget(startButton);
    proxy->setPos((800 - startButton->width()) / 2, 400);

    // connect button click to start the game
    connect(startButton, &QPushButton::clicked, this, &MenuScene::startGameRequested);
}
