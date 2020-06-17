#ifndef EXAMPLANNERSERVER_H
#define EXAMPLANNERSERVER_H

#include <QObject>
#include <QThread>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "src/dataModel/day.h"
#include "src/dataModel/group.h"
#include "src/dataModel/module.h"
#include "src/dataModel/plan.h"
#include "src/dataModel/semester.h"
#include "src/dataModel/timeslot.h"
#include "src/dataModel/week.h"

#include "src/examplanner.h"

class ExamPlannerServer : public QObject
{
    Q_OBJECT
private:
    QJsonValue plans;
    QJsonValue plannerPlan;
    int plannerProgress;
    QThread examPlannerThread;
public:
    explicit ExamPlannerServer(QObject *parent = nullptr);

signals:

private slots:
    void finishedPlanning(Plan* finishedPlan);
    void progressChanged(int progress);

public slots:
    QJsonValue getPlans();
    void setPlans(QJsonValue newplans);
    void startPlanning(QJsonValue plan);
    void startPlanningTest();
    int getPlanningProgress();
    QJsonValue getPlannedPlan();
};

#endif // EXAMPLANNERSERVER_H
