#include "spritepickerdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QIcon>


// pick sprite before you start the game
SpritePickerDialog::SpritePickerDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Choose Your Pac-Man");
    resize(650, 350);
    setupUI();
}


// creates the layout and buttons for selecting a PacMan skin

void SpritePickerDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* title = new QLabel("Select Your Pac-Man Skin");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: white;");
    mainLayout->addWidget(title);
    mainLayout->addSpacing(20);

    QHBoxLayout* rowLayout = new QHBoxLayout();
    rowLayout->setSpacing(50);
    rowLayout->setAlignment(Qt::AlignCenter);

    QStringList previewPaths = {
        ":/game_objects/pacman/yellow/a3.png",
        ":/game_objects/pacman/red/a3.png",
        ":/game_objects/pacman/blue/a3.png"
    };

    QStringList basePaths = {
        ":/game_objects/pacman/yellow/",
        ":/game_objects/pacman/red/",
        ":/game_objects/pacman/blue/"
    };

    QStringList labels = { "Yellow", "Pink", "Blue" };
    QStringList colors = { "#FFD700", "#FF69B4", "#00BFFF" };

    for (int i = 0; i < previewPaths.size(); ++i) {
        QVBoxLayout* skinLayout = new QVBoxLayout();
        skinLayout->setAlignment(Qt::AlignCenter);

        QPushButton* btn = new QPushButton();
        QPixmap pix(previewPaths[i]);
        pix = pix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        btn->setIcon(QIcon(pix));
        btn->setIconSize(QSize(60, 60));

        btn->setFixedSize(170, 170);
        btn->setStyleSheet(
            "QPushButton { border: 3px solid white; border-radius: 20px; background-color: black; }"
            "QPushButton:hover { border: 3px solid #FFD700; }"
            );

        connect(btn, &QPushButton::clicked, this, [=]() {
            chooseSprite(basePaths[i]);
        });

        QLabel* label = new QLabel(labels[i]);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(colors[i]));

        skinLayout->addWidget(btn);
        skinLayout->addSpacing(10);
        skinLayout->addWidget(label);

        rowLayout->addLayout(skinLayout);
    }

    mainLayout->addLayout(rowLayout);
    setStyleSheet("background-color: #111111;");
    resize(700, 400);
}


// Called when a sprite is selected , emits signal and closes dialog
void SpritePickerDialog::chooseSprite(const QString& basePath)
{
    emit spriteChosen(basePath);
    accept();
}
