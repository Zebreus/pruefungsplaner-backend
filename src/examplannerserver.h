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
#include <plancsvhelper.h>

#include <jwt-cpp/jwt.h>

#include "src/QtJsonTraits.h"

class ExamPlannerServer : public QObject
{
    Q_OBJECT
private:
    static QJsonValue plans;
    QString publicKey;
    bool authorized;

public:
    explicit ExamPlannerServer(const QString& publicKey, QObject *parent = nullptr);

public slots:
    bool login(QString token);
    QJsonValue getPlans();
    void setPlans(QJsonValue newplans);
};

#endif // EXAMPLANNERSERVER_H
