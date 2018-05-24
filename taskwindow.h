#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:

    TaskWindow(QWidget *parent = 0);

    ~TaskWindow();

    //Tipo da tarefa
    int taskType;

    //Número de repetições
    int taskRep;

    //Velocidade da tarefa
    int taskVel;

    //Câmera utilizada
    int cam;

    void setInfo(int camera, int rep, int vel, int type);

private:
    Ui::TaskWindow *ui;
};

#endif // TASKWINDOW_H
