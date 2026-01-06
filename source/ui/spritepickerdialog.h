#pragma once

#include <QDialog>
#include <QString>

class QPushButton;

class SpritePickerDialog : public QDialog {
    Q_OBJECT

public:
    explicit SpritePickerDialog(QWidget* parent = nullptr);

signals:
    void spriteChosen(const QString& spriteBasePath);

private slots:
    void chooseSprite(const QString& basePath);

private:
    void setupUI();
};
