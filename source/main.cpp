#include "mainwindow.h"
#include <QApplication>


// entry point of the game
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // Create the main game window
    MainWindow w;
    w.setStyleSheet("QMainWindow {background: 'black';}");
    w.show();

    // start the event loop
    return a.exec();
}
