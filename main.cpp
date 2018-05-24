#include "mainmenu.h"
#include <QApplication>
#include <QObject>
#include <QStyle>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainMenu* w = new MainMenu;

    w->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(&a, SIGNAL(lastWindowClosed()),w,SLOT(close()));

    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                       Qt::AlignCenter,
                                       w->size(),
                                       qApp->desktop()->availableGeometry()
                                       )
                   );
    w->show();

    return a.exec();
}
