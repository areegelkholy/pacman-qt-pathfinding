#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gamemanager.h"
#include "spritepickerdialog.h"

#include "MenuScene.h"
#include "StoreDialog.h"
#include "constants.h"
#include "entity.h"
#include <QRandomGenerator>



// initializes main window, sprite picker, and main menu

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , manager(nullptr)
    , hudTimer(nullptr)
{
    ui->setupUi(this);
    // Sprite picker before menu
    SpritePickerDialog* picker = new SpritePickerDialog(this);
    picker->setFixedSize(800, 650);
    picker->move(100, 100); // Same position as MainWindow

    connect(picker, &SpritePickerDialog::spriteChosen, this, [this](const QString& path) {
        chosenPacmanSpritePath = path;
        qDebug() << " Sprite path selected:" << chosenPacmanSpritePath;

        // Initialize menu after sprite selection
        menuScene = new MenuScene(this);
        connect(menuScene, &MenuScene::startGameRequested, this, &MainWindow::startGame);
        ui->graphicsView->setScene(menuScene);

    });

    picker->exec();

    setWindowTitle("Pac‑Man");
    setFixedSize(800, 650);
    move(100, 100);

    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->setStyleSheet("QGraphicsView { border: none; background: black; }");

    // Heads up display (HUD) components
    scoreLabel = new QLabel(this);
    livesLabel = new QLabel(this);
    coinsLabel = new QLabel(this);
    levellabel = new QLabel(this);
    endScreenLabel = new QLabel(this);
    levelPassedLabel = new QLabel(this);
    nextLevelButton = new QPushButton("Next Level");

    setupHUD();
}


MainWindow::~MainWindow()
{
    delete ui;
}


// Configures HUD display and overlays
void MainWindow::setupHUD()
{
    QFont font("Arial", 16, QFont::Bold);
    const char *style = "QLabel { color: white; background: transparent; }";

    for (QLabel *lbl : {scoreLabel, livesLabel, coinsLabel, levellabel}) {
        lbl->setFont(font);
        lbl->setStyleSheet(style);
    }

    scoreLabel->setGeometry(50, 10, 200, 30);
    livesLabel->setGeometry(300, 10, 200, 30);
    coinsLabel->setGeometry(550, 10, 200, 30);
    levellabel->setGeometry(50, 40, 200, 30);

    endScreenLabel->hide();
    endScreenLabel->setGeometry(0, 0, width(), height());
    endScreenLabel->setAlignment(Qt::AlignCenter);
    endScreenLabel->setStyleSheet("QLabel { color: white; background-color: black; }");
    endScreenLabel->setFont(QFont("Arial", 48, QFont::Bold));

    overlayWidget = new QWidget(ui->graphicsView->viewport());
    overlayWidget->setGeometry(0, 0, width(), height());
    overlayWidget->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    overlayWidget->hide();

    overlayLayout = new QVBoxLayout(overlayWidget);
    overlayLayout->setAlignment(Qt::AlignCenter);

    levelPassedLabel->setStyleSheet("QLabel { color: yellow; background: transparent; }");
    levelPassedLabel->setFont(QFont("Arial", 32, QFont::Bold));
    levelPassedLabel->setAlignment(Qt::AlignCenter);

    nextLevelButton->setFixedSize(200, 50);
    nextLevelButton->setFont(QFont("Arial", 14));

    overlayLayout->addWidget(levelPassedLabel);
    overlayLayout->addSpacing(20);
    overlayLayout->addWidget(nextLevelButton);
    overlayWidget->hide();
}


// Starts a new game session and connects level signals
void MainWindow::startGame()
{
    manager = new GameManager(this);

    connect(manager, &GameManager::levelChanged,
            this, [this](Level *lvl) {
                ui->graphicsView->setScene(lvl);
                disconnect(lvl, nullptr, this, nullptr);

                connect(lvl, &Level::gameLost, this, [this]() {
                    showEndScreen("💀 YOU LOSE 💀");
                });

                connect(lvl, &Level::levelWon, this, &MainWindow::showLevelPassedScreen);
                connect(lvl, &Level::bombHitTriggered, this, &MainWindow::startScreenShake);

            });

    connect(manager, &GameManager::gameWon, this, [this]() {
        showEndScreen("🎉 YOU WIN 🎉");
    });

    manager->startGame();
    ui->graphicsView->setScene(manager->getCurrentLevel());
    this->setFocus();

    qDebug() << "MainWindow: Game started";

    hudTimer = new QTimer(this);
    connect(hudTimer, &QTimer::timeout, this, &MainWindow::updateHUD);
    hudTimer->start(100);
    updateHUD();
}


// Updates score, lives, coins, and level number on HUD
void MainWindow::updateHUD()
{
    if (!manager || !manager->getCurrentLevel()) return;

    Level *lvl = manager->getCurrentLevel();
    scoreLabel->setText(QString("🏆 Score: %1").arg(lvl->getScore()));
    livesLabel->setText(QString("❤️ Lives: %1").arg(lvl->getLives()));
    coinsLabel->setText(QString("🪙 Coins: %1").arg(lvl->getCoins()));
    levellabel->setText(QString("🗺️ Level: %1").arg(manager->getCurrentLevelIndex() + 1));
}


// Shows the final end screen with win or lose message
void MainWindow::showEndScreen(const QString &msg)
{
    if (hudTimer) hudTimer->stop();
    if (manager && manager->getCurrentLevel())
        manager->getCurrentLevel()->stop();

    Level *lvl = manager->getCurrentLevel();

    if (msg.contains("YOU WIN")) {
        endScreenLabel->setStyleSheet(
            "QLabel { "
            "background-image: url(:/game_objects/map_objects/treasure.png); "
            "background-repeat: no-repeat; "
            "background-position: center; "
            "color: White; "
            "font-size: 28px; "
            "font-weight: bold; "
            "text-align: center;"
            "}");

        int totalCoins = lvl->getCoins();
        endScreenLabel->setText(QString("🎉 YOU WIN 🎉\n\nTotal Coins Collected: 🪙 %1").arg(totalCoins));
    } else {
        endScreenLabel->setStyleSheet("QLabel { color: white; background-color: black; }");
        endScreenLabel->setText(msg);
    }

    endScreenLabel->setAlignment(Qt::AlignCenter);
    endScreenLabel->setGeometry(0, 0, width(), height());
    endScreenLabel->show();
}


// Display level completion overlay with button to advance
void MainWindow::showLevelPassedScreen()
{
    qDebug() << "✅ showLevelPassedScreen called";

    if (hudTimer) hudTimer->stop();
    if (manager && manager->getCurrentLevel())
        manager->getCurrentLevel()->stop();

    overlayWidget->raise();
    overlayWidget->show();

    levelPassedLabel->setText(QString("✅ You passed Level %1!").arg(manager->getCurrentLevelIndex() + 1));
    levelPassedLabel->raise();
    levelPassedLabel->show();

    nextLevelButton->setText("Next Level");
    nextLevelButton->setStyleSheet("QPushButton { background-color: white; color: black; }");
    nextLevelButton->raise();
    nextLevelButton->show();

    nextLevelButton->disconnect();
    connect(nextLevelButton, &QPushButton::clicked, this, [this]() {
        overlayWidget->hide();
        manager->advanceLevel();
        this->setFocus();
        hudTimer->start(100);
    });
}


//  to handle Keyboard control
void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (!manager) {
        QMainWindow::keyPressEvent(ev);
        return;
    }

    Level *lvl = manager->getCurrentLevel();
    switch (ev->key()) {
    case Qt::Key_Up:    lvl->setNextDirection(Entity::Up);    break;
    case Qt::Key_Down:  lvl->setNextDirection(Entity::Down);  break;
    case Qt::Key_Left:  lvl->setNextDirection(Entity::Left);  break;
    case Qt::Key_Right: lvl->setNextDirection(Entity::Right); break;
    case Qt::Key_S: {
        StoreDialog dlg(lvl, this);
        dlg.setFixedSize(800, 650);
        dlg.move(100, 100);
        dlg.exec();
        this->setFocus();
        break;
    }
    default:
        QMainWindow::keyPressEvent(ev);
    }
}



// Screen shake effect triggered by bombs
void MainWindow::startScreenShake()
{
    QGraphicsView* view = ui->graphicsView;
    QPoint originalPos = view->pos();
    QTimer* shakeTimer = new QTimer(this);
    int* elapsed = new int(0);

    connect(shakeTimer, &QTimer::timeout, this, [=]() mutable {
        if (*elapsed >= 400) {
            shakeTimer->stop();
            view->move(originalPos);
            shakeTimer->deleteLater();
            delete elapsed;
            return;
        }

        int dx = (QRandomGenerator::global()->bounded(11)) - 5;
        int dy = (QRandomGenerator::global()->bounded(11)) - 5;
        view->move(originalPos + QPoint(dx, dy));

        *elapsed += 16;
    });

    shakeTimer->start(16);
}


