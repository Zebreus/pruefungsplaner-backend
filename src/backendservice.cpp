#include "backendservice.h"

BackendService::BackendService(QSharedPointer<QJsonValue> semesters,
                               QSharedPointer<QMutex> accessMutex, const QString& publicKey, QObject* parent)
    : QObject(parent), semesters(semesters), accessMutex(accessMutex), publicKey(publicKey), authorized(false) {
  // Load plans if not initialized yet
  if (semesters->isUndefined() || semesters->isNull()) {
    PlanCsvHelper helper("../pruefungsplaner-backend/res/");
    system("ls ../pruefungsplaner-backend/res/");
    QSharedPointer<Plan> plan = helper.readPlan();
    if (plan != nullptr) {
      plan->setName("Plan A");

      QList<Plan*> newPlans;
      newPlans.append(plan.get());

      Semester* semester = new Semester();
      semester->setName("WS 2019");
      plan->setParent(semester);
      semester->setPlans(newPlans);

      QJsonArray arr;
      QJsonValue semesterValue = semester->toJsonObject();
      arr.append(semesterValue);

      *semesters = QJsonValue(arr);
    } else {
      qDebug() << "Failed to read plan from ../pruefungsplaner-backend/res/.";
    }
  }
}

BackendService::~BackendService() {
  if (authorized) {
    accessMutex->unlock();
  }
}

bool BackendService::ready() {
  if (accessMutex->tryLock(0)) {
    accessMutex->unlock();
    return true;
  } else {
    return false;
  }
}

bool BackendService::login(QString token) {
  if (authorized) {
    return true;
  }

  if (accessMutex->tryLock(0)) {
    if (verifyToken(token)) {
      authorized = true;
      return true;
    }
    accessMutex->unlock();
  }

  return false;
}

QJsonValue BackendService::getSemesters() {
  if (authorized) {
    return *semesters;
  } else {
    return QJsonValue::Undefined;
  }
}

bool BackendService::setSemesters(QJsonArray semesters) {
  if (authorized) {
    *this->semesters = semesters;
    return true;
  } else {
    return false;
  }
}

bool BackendService::verifyToken(const QString& token) {
  try {
    auto verifier =
        jwt::verifier<jwt::default_clock, QtJsonTraits>(jwt::default_clock{})
            .with_claim("pruefungsplanerRead",
                        jwt::basic_claim<QtJsonTraits>(QString("true")))
            .with_claim("pruefungsplanerRead",
                        jwt::basic_claim<QtJsonTraits>(QString("true")))
            .allow_algorithm(jwt::algorithm::rs256(
                publicKey.toUtf8().constData(), "", "", ""))
            .with_audience(QJsonArray{"pruefungsplaner-backend"})
            .with_issuer("securityprovider");

    auto decodedToken = jwt::decode<QtJsonTraits>(token);

    verifier.verify(decodedToken);

    qDebug() << "User " << decodedToken.get_subject()
             << " logged in with valid token";
    return true;
  } catch (std::runtime_error& e) {
    qDebug() << "Invalid token";
    return false;
  } catch (...) {
    qDebug() << "Unexpected exception thrown in BackendService::verifyToken. "
                "This should not happen.";
    return false;
  }
}
