#ifndef EXAMPLANNERSERVER_H
#define EXAMPLANNERSERVER_H

#include <QObject>
#include <QThread>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "day.h"
#include "group.h"
#include "module.h"
#include "plan.h"
#include "semester.h"
#include "timeslot.h"
#include "week.h"

#include "csvloader.h"

#include <jwt-cpp/jwt.h>

#include "src/QtJsonTraits.h"
#include "src/examplanner.h"

class ExamPlannerServer : public QObject
{
    Q_OBJECT
private:
    static QJsonValue plans;
    QJsonValue plannerPlan;
    int plannerProgress;
    QThread examPlannerThread;
    QString publicKey;
    bool authorized;

public:
    explicit ExamPlannerServer(const QString& publicKey, QObject *parent = nullptr);

signals:

private slots:
    void finishedPlanning(Plan* finishedPlan);
    void progressChanged(int progress);

public slots:
    bool login(QString token);
    QJsonValue getPlans();
    void setPlans(QJsonValue newplans);
    void startPlanning(QJsonValue plan);
    void startPlanningTest();
    int getPlanningProgress();
    QJsonValue getPlannedPlan();
};

#endif // EXAMPLANNERSERVER_H
