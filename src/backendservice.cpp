#include "backendservice.h"

BackendService::BackendService(QSharedPointer<QJsonValue> semesters,
                               QSharedPointer<QMutex> accessMutex,
                               QSharedPointer<Configuration> config,
                               QObject* parent)
    : QObject(parent),
      semesters(semesters),
      accessMutex(accessMutex),
      config(config),
      authorized(false) {}

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
            .with_claim("pruefungsplanerWrite",
                        jwt::basic_claim<QtJsonTraits>(QString("true")))
            .allow_algorithm(jwt::algorithm::rs256(
                publicKey.toUtf8().constData(), "", "", ""))
            .with_audience(QJsonArray{"pruefungsplaner-backend"})
            .with_issuer("securityprovider")
            .issued_at_leeway(0)
            .not_before_leeway(0)
            .expires_at_leeway(0)
            .leeway(0);

    auto decodedToken = jwt::decode<QtJsonTraits>(token);

    if (!decodedToken.has_expires_at()) {
      throw std::runtime_error("Missing exp");
    }

    if (!decodedToken.has_issued_at()) {
      throw std::runtime_error("Missing iat");
    }

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
  return false;
}
