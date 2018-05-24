#ifndef TASK_H
#define TASK_H

#include <QObject>

class Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(int cam, int task, int velocity, int repetitions);

signals:

public slots:
};

#endif // TASK_H
