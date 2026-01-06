#ifndef STOREDIALOG_H
#define STOREDIALOG_H

#include <QDialog>
class Level;
class GameManager;

class StoreDialog : public QDialog
{
    Q_OBJECT
public:
    StoreDialog(Level* level, QWidget* parent = nullptr);
    GameManager* gameManager;


private slots:
    void buySpeedBoost();
    void buyShield();
    void buyElixir();

private:
    Level* level;

};


#endif
