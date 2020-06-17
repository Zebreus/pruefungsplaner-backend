#ifndef EXAMPLANNER_H
#define EXAMPLANNER_H

#include <QObject>
#include <QThread>
#include "src/dataModel/day.h"
#include "src/dataModel/group.h"
#include "src/dataModel/module.h"
#include "src/dataModel/plan.h"
#include "src/dataModel/timeslot.h"
#include "src/dataModel/week.h"

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
