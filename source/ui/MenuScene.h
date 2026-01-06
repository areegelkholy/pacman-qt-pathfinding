#ifndef MENUSCENE_H
#define MENUSCENE_H

#include <QGraphicsScene>
#include <QPushButton>

class MenuScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MenuScene(QObject *parent = nullptr);

signals:
    void startGameRequested();

private:
    QPushButton *startButton;
};

#endif
