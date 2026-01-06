#include "StoreDialog.h"
#include "core/level.h"
#include "entities/ghost.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>


// Constructo of the in-game store dialog
StoreDialog::StoreDialog(Level* level, QWidget* parent)
    : QDialog(parent), level(level)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    this->setStyleSheet(
        "QDialog { background-color: rgba(0, 0, 0, 150); border-radius: 12px; }"
        );

    setWindowTitle("Store");
    setFixedSize(320, 320);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);

    QLabel *info = new QLabel(QString("🪙 Coins: %1").arg(level->getCoins()), this);
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("QLabel { color: white; font-size: 18px; font-weight: bold; }");
    layout->addWidget(info);

    // Purchase buttons
    QPushButton *speedButton = new QPushButton("  Buy Speed Boost (10 coins)", this);
    speedButton->setIcon(QIcon(":/game_objects/store/speed_boost.png"));
    speedButton->setIconSize(QSize(32, 32));
    speedButton->setStyleSheet("QPushButton { color: black; font-size: 14px; font-weight: bold; padding: 8px; text-align: left; }");

    QPushButton *shieldButton = new QPushButton("  Buy Shield (15 coins)", this);
    shieldButton->setIcon(QIcon(":/game_objects/store/shield.png"));
    shieldButton->setIconSize(QSize(32, 32));
    shieldButton->setStyleSheet("QPushButton { color: black; font-size: 14px; font-weight: bold; padding: 8px; text-align: left; }");


    QPushButton *elixirButton = new QPushButton("  Buy Elixir (+3 lives for 30 coins)", this);
    elixirButton->setIcon(QIcon(":/game_objects/store/clock.png"));
    elixirButton->setIconSize(QSize(32, 32));
    elixirButton->setStyleSheet("QPushButton { color: black; font-size: 14px; font-weight: bold; padding: 8px; text-align: left; }");


    layout->addWidget(speedButton);
    layout->addWidget(shieldButton);
    layout->addWidget(elixirButton);

    // Connect button signals to  purchase slots
    connect(speedButton, &QPushButton::clicked, this, &StoreDialog::buySpeedBoost);
    connect(shieldButton, &QPushButton::clicked, this, &StoreDialog::buyShield);
    connect(elixirButton, &QPushButton::clicked, this, &StoreDialog::buyElixir);
}


//  to buy a Speed Boost

void StoreDialog::buySpeedBoost()
{
    if (level->getCoins() >= 10) {
        level->setCoins(level->getCoins() - 10);
        level->pacman->activateSpeedBoost();
        level->setPacmanSpeed(5);
    }
    close();
}


//  to buy a shield
void StoreDialog::buyShield()
{
    if (level->getCoins() >= 15) {
        level->setCoins(level->getCoins() - 15);
        level->activateShield();

        close();
    } else {
        QMessageBox::warning(this, "Not Enough Coins", "You need at least 15 coins to buy a shield.");
    }
}
 // to buy an Elixir (extra lives)
void StoreDialog::buyElixir()
{
    if (level->getCoins() >= 30) {
        level->setCoins(level->getCoins() - 30);
        level->increaseLives(3);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Elixir");
        msgBox.setText("You gained 3 extra lives!");
        msgBox.setStyleSheet(
            "QMessageBox { background-color: rgba(30, 30, 30, 200); color: white; } "
            "QLabel { color: white; } "
            "QPushButton { background-color: white; color: black; }"
            );
        msgBox.exec();
        close();
    } else {
        QMessageBox::warning(this, "Not Enough Coins", "You need at least 30 coins to buy an elixir.");
    }
}
