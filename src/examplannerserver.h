#ifndef EXAMPLANNERSERVER_H
#define EXAMPLANNERSERVER_H

#include <QObject>

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


class ExamPlannerServer : public QObject
{
    Q_OBJECT
private:
    QJsonValue plans;
public:
    explicit ExamPlannerServer(QObject *parent = nullptr);

signals:

public slots:
    QJsonValue getPlans();
    void setPlans(QString newplans);
};

#endif // EXAMPLANNERSERVER_H
