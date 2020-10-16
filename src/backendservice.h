#ifndef EXAMPLANNERSERVER_H
#define EXAMPLANNERSERVER_H

#include <QObject>
#include <QThread>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <plancsvhelper.h>
#include "day.h"
#include "group.h"
#include "module.h"
#include "plan.h"
#include "semester.h"
#include "timeslot.h"
#include "week.h"

#include <jwt-cpp/jwt.h>

#include "src/QtJsonTraits.h"

class BackendService : public QObject {
  Q_OBJECT

 public:
  explicit BackendService(const QString& publicKey,
                             QObject* parent = nullptr);

 public slots:
  bool login(QString token);
  QJsonValue getPlans();
  void setPlans(QJsonValue newplans);

 private:
  static QJsonValue plans;
  QString publicKey;
  bool authorized;
};

#endif  // EXAMPLANNERSERVER_H
