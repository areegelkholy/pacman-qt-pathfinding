#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QTimer>
#include <QBoxLayout>
#include "gamemanager.h"
#include "level.h"
#include "MenuScene.h"
#include "StoreDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event) override;


private slots:
    void updateHUD();
    void startScreenShake();

private:
    Ui::MainWindow *ui;
    GameManager* manager;
    MenuScene *menuScene;


    QLabel *scoreLabel;
    QWidget* overlayWidget;
    QVBoxLayout* overlayLayout;
    QLabel *livesLabel;
    QLabel *coinsLabel;
    QLabel *endScreenLabel;
    QLabel *levellabel;
    QLabel* levelPassedLabel;
    QPushButton* nextLevelButton;
    QTimer *hudTimer;




    void setupHUD();
    void showEndScreen(const QString &msg);
    void showLevelPassedScreen();



    void startGame();
    void showInGameButtons(bool show);
    void showEndButtons();



    bool paused = false;

};

#endif
