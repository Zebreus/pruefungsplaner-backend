#ifndef EXAMPLANNER_H
#define EXAMPLANNER_H

#include <QObject>
#include <QThread>
#include "day.h"
#include "group.h"
#include "module.h"
#include "plan.h"
#include "timeslot.h"
#include "week.h"

class ExamPlanner : public QObject
{
    Q_OBJECT
    int progress;
    Plan* plan;

public:
    explicit ExamPlanner(QObject *parent = nullptr, Plan *plan = nullptr);
    int getProgress();

signals:
    void finishedPlanning(Plan* finishedPlan);
    void progressChanged(int progress);

public slots:
    void startPlanning();

};

#endif // EXAMPLANNER_H
