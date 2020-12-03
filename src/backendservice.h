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

#include <QMutex>
#include "configuration.h"
#include "src/QtJsonTraits.h"

class BackendService : public QObject {
  Q_OBJECT

 public:
  /**
   *  @brief Create a  BackendService object
   *  @param [in] configuration is the configuration of the server
   *  @param [in] parent is the parent of this QObject
   */
  explicit BackendService(QSharedPointer<QJsonValue> semesters,
                          QSharedPointer<QMutex> accessMutex,
                          QSharedPointer<Configuration> config,
                          QObject* parent = nullptr);

  /**
   *  @brief Destroys this object and makes the BackendService accesible again
   *
   *  Destroys this object and makes the BackendService accesible again
   */
  ~BackendService();

 public slots:
  /**
   *  @brief Check if the BackendService is can be accesssed
   *  @return True if the Backend is ready
   *
   *  The current implementation of BackendService supports only one client
   * simultaneously. This function can be used to check if no other client is
   * using it. If Backend service is not ready, login will always fail.
   */
  bool ready();

  /**
   *  @brief Authorize the connection
   *  @param [in] token is a json web token signed by pruefungsplaner-auth
   *  @return True if BackendService is ready and the token is valid.
   *
   *  Authorize the connection to the BackendService. token needs to be issued
   * and signed by pruefungsplaner-auth and contains the claims
   * 'pruefungsplanerRead' and 'pruefungsplanerWrite' with the value 'true'.
   * This function will always returns false if the BackendService is not ready.
   * If this BackendService instance is already authorized it will return true.
   */
  bool login(QString token);

  /**
   *  @brief Get all semesters and plans
   *  @return A QJsonValue containing a array of Semester objects
   *
   *  Get all semesters as a QJsonValue. The result is a json array containing
   * objects that can be parsed into Semester objects.
   */
  QJsonValue getSemesters();

  /**
   *  @brief Stores the semesters
   *  @param [in] semesters is a List of all semesters that should be stored.
   *  @return true if the semesters were stored
   *
   *  Replaces the stored semesters with semesters.
   */
  bool setSemesters(QJsonArray semesters);

 private:
  /**
   *  @brief Checks if token is signed and valid
   *  @param [in] token is a json web token signed by pruefungsplaner-auth
   *  @return True if the token is signed and valid
   */
  bool verifyToken(const QString& token);

 private:
  QSharedPointer<QJsonValue> semesters;
  QSharedPointer<QMutex> accessMutex;
  QSharedPointer<Configuration> config;
  /**
   *  @brief Stores if this BackendService instance is authorized
   *  If the instance is authorized it also locks accessMutex
   */
  bool authorized;
};

#endif  // EXAMPLANNERSERVER_H
