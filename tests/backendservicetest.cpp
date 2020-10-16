#ifndef BACKENDSERVICE_TEST_CPP
#define BACKENDSERVICE_TEST_CPP

#include <backendservice.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QString>
#include "qthelper.cpp"
#include "QtJsonTraits.h"
using namespace testing;

class BackendServiceTests : public ::testing::Test {
 protected:
  void SetUp() override {
    loadKeys();
    mutex.reset(new QMutex());
    semesters = loadExampleSemesters();
  }

  void loadKeys() {
    privateKey1 = loadFile(":/securityprovider/testdata/private1.pem");
    ASSERT_NE(privateKey1, "");
    publicKey1 = loadFile(":/securityprovider/testdata/public1.pem");
    ASSERT_NE(publicKey1, "");
    privateKey2 = loadFile(":/securityprovider/testdata/private2.pem");
    ASSERT_NE(privateKey2, "");
    publicKey2 = loadFile(":/securityprovider/testdata/public2.pem");
    ASSERT_NE(publicKey2, "");
  }

  QString loadFile(QString filePath) {
    QFile file(filePath);
    bool opened = file.open(QFile::ReadOnly | QFile::Text);
    EXPECT_TRUE(opened) << "Error opening key file: " << filePath.toStdString();
    if (!opened) {
      file.close();
      return "";
    }
    QString content = QTextStream(&file).readAll();
    file.close();
    return content;
  }

  QSharedPointer<QJsonValue> loadExampleSemesters() {
    QSharedPointer<QJsonValue> semesters(new QJsonValue());

    PlanCsvHelper helper("../pruefungsplaner-backend/res/");
    QSharedPointer<Plan> plan = helper.readPlan();
    EXPECT_NE(plan.get(), nullptr)
        << "Failed to read plan from ../pruefungsplaner-backend/res/.";
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
      return semesters;
    } else {
      return nullptr;
    }
  }

  QString createValidToken(QString key = ""){
      if(key == ""){
          key = privateKey1;
      }
      jwt::builder tokenBuilder = jwt::create<QtJsonTraits>()
      .set_type("JWT")
      .set_issuer("securityprovider")
      .set_audience(QJsonArray{"pruefungsplaner-backend"})
      .set_subject("test")
      .set_issued_at(std::chrono::system_clock::now())
      .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
      .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
      .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));

      return signToken(tokenBuilder, key);
  }

  QString createInvalidToken(QString key = ""){
      if(key == ""){
          key = privateKey1;
      }
      jwt::builder tokenBuilder = jwt::create<QtJsonTraits>()
      .set_type("JWT")
      .set_issuer("not-the-securityprovider")
      .set_audience(QJsonArray{"not-pruefungsplaner-backend"})
      .set_subject("test")
      .set_issued_at(std::chrono::system_clock::now())
      .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
      .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("falsch")))
      .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));

      return signToken(tokenBuilder, key);
  }

  QString signToken(jwt::builder<QtJsonTraits> tokenBuilder, QString key = ""){
      if(key == ""){
          key = privateKey1;
      }

      return tokenBuilder.sign(jwt::algorithm::rs256(publicKey1.toUtf8().constData(),privateKey1.toUtf8().constData(),"",""));
  }

  QString privateKey1;
  QString publicKey1;
  QString privateKey2;
  QString publicKey2;

  QSharedPointer<QMutex> mutex;
  QSharedPointer<QJsonValue> semesters;

  void TearDown() override {}
};

TEST_F(BackendServiceTests, loginSucceedsWithValidToken) {
  BackendService backend(semesters, mutex, publicKey1);
  QString token = createValidToken();
  ASSERT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
}

TEST_F(BackendServiceTests, loginFailsWithInvalidToken) {
    BackendService backend(semesters, mutex, publicKey1);
    QString token = createInvalidToken();
    ASSERT_FALSE(backend.login(token)) << "Token is " << token.toStdString();
}

TEST_F(BackendServiceTests, loginSucceedsOnSecondTryWithDifferentToken) {
    BackendService backend(semesters, mutex, publicKey1);
    QString invalidToken = createInvalidToken();
    QString token = createValidToken();
    ASSERT_FALSE(backend.login(invalidToken)) << "Token is " << token.toStdString();
    ASSERT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
}

TEST_F(BackendServiceTests, invalidTokensGetDetected) {
BackendService backend(semesters, mutex, publicKey1);

    jwt::builder<QtJsonTraits> tokenBuilder;

    //Wrong issuer
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("not-the-securityprovider")
            .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with missing audience";

    //Missing audience
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with missing audience";


    //Missing issuer
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
    .set_subject("test")
            .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with missing audience";

    //Wrong audience
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"not-pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with wrong audience";

    //Wrong pruefungsplanerRead
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("dfasf")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with wrong pruefungsplanerRead";

    //Missing pruefungsplanerRead
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with missing pruefungsplanerRead";

    //Wrong pruefungsplanerWrite
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("fdgdsd")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with wrong pruefungsplanerWrite";

    //Missing pruefungsplanerWrite
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with missing pruefungsplanerWrite";

    //Expired
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now() - std::chrono::seconds{7200})
    .set_expires_at(std::chrono::system_clock::now() - std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "Backend service accepted expired token";

    //not issued yet
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{7200})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "BackendService accepted token with future issued at";

    //No expiration
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_issued_at(std::chrono::system_clock::now())
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "Backend service accepted token with no expiration";

    //No issued at
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_subject("test")
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "Backend service accepted token with no issued at";

    //No subject
    tokenBuilder = jwt::create<QtJsonTraits>()
    .set_type("JWT")
            .set_issuer("securityprovider")
    .set_audience(QJsonArray{"pruefungsplaner-backend"})
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{3600})
    .set_payload_claim("pruefungsplanerRead", jwt::basic_claim<QtJsonTraits>(QString("true")))
    .set_payload_claim("pruefungsplanerWrite", jwt::basic_claim<QtJsonTraits>(QString("true")));
    EXPECT_FALSE(backend.login(signToken(tokenBuilder))) << "Backend service accepted token with no subject";
}

TEST_F(BackendServiceTests, loginFailsWhenNotReady) {
    BackendService backend(semesters, mutex, publicKey1);
    BackendService backend2(semesters, mutex, publicKey1);
    QString token = createValidToken();

    ASSERT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
    EXPECT_FALSE(backend.ready());
    ASSERT_FALSE(backend2.login(token)) << "Token is " << token.toStdString();
}

TEST_F(BackendServiceTests, loginReturnsTrueWhenAlreadyAuthorized) {
    BackendService backend(semesters, mutex, publicKey1);
    QString token = createValidToken();

    ASSERT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
    ASSERT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
}

TEST_F(BackendServiceTests, readyAfterReady) {
    BackendService backend(semesters, mutex, publicKey1);
    ASSERT_TRUE(backend.ready());
    ASSERT_TRUE(backend.ready());
    BackendService backend2(semesters, mutex, publicKey1);
    ASSERT_TRUE(backend2.ready());
    ASSERT_TRUE(backend.ready());
}

TEST_F(BackendServiceTests, readyBeforeFirstLogin) {
    BackendService backend(semesters, mutex, publicKey1);
    ASSERT_TRUE(backend.ready());
}

TEST_F(BackendServiceTests, readyAfterFailedLogin) {
    BackendService backend(semesters, mutex, publicKey1);
    QString token = createInvalidToken();
    EXPECT_FALSE(backend.login(token)) << "Token is " << token.toStdString();
    ASSERT_TRUE(backend.ready());
}

TEST_F(BackendServiceTests, notReadyDuringOtherSession) {
    BackendService backend(semesters, mutex, publicKey1);
    BackendService backend2(semesters, mutex, publicKey1);
    QString token = createValidToken();
    EXPECT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
    ASSERT_FALSE(backend.ready());
    ASSERT_FALSE(backend2.ready());
}

TEST_F(BackendServiceTests, readyAfterFinishedSession) {
    BackendService* backend = new BackendService(semesters, mutex, publicKey1);
    BackendService backend2(semesters, mutex, publicKey1);
    QString token = createValidToken();
    EXPECT_TRUE(backend->login(token)) << "Token is " << token.toStdString();
    EXPECT_FALSE(backend->ready());
    EXPECT_FALSE(backend2.ready());
    delete backend;
    EXPECT_TRUE(backend2.ready());
}

TEST_F(BackendServiceTests, getSemestersReturnsUndefinedWithoutAuthorization) {
  BackendService backend(semesters, mutex, publicKey1);
  BackendService backend2(semesters, mutex, publicKey1);
  QString token = createInvalidToken();
  EXPECT_FALSE(backend2.login(token)) << "Token is " << token.toStdString();
  ASSERT_EQ(backend.getSemesters(), QJsonValue::Undefined);
  ASSERT_EQ(backend2.getSemesters(), QJsonValue::Undefined);
}

TEST_F(BackendServiceTests, getSemestersReturnsSemestersWithAuthorization) {
    BackendService backend(semesters, mutex, publicKey1);
    QString token = createValidToken();
    EXPECT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
    //TODO do not check the id values of the json objects, because it is valid to change them
    ASSERT_EQ(backend.getSemesters(), *semesters);
}

TEST_F(BackendServiceTests, setSemestersReturnsFalseWithoutAuthorization) {
    BackendService backend(semesters, mutex, publicKey1);
    BackendService backend2(semesters, mutex, publicKey1);
    QString token = createInvalidToken();
    EXPECT_FALSE(backend2.login(token)) << "Token is " << token.toStdString();
    ASSERT_FALSE(backend.setSemesters(semesters->toArray()));
    ASSERT_FALSE(backend2.setSemesters(semesters->toArray()));
}

TEST_F(BackendServiceTests, setSemestersSetsSemesters) {
    BackendService backend(semesters, mutex, publicKey1);
    QSharedPointer<QJsonValue> modifiedSemesters = loadExampleSemesters();
    QJsonArray tempSemesters = modifiedSemesters->toArray();
    QJsonObject tempPlan = tempSemesters[0].toObject();
    tempPlan.insert("name", "testname239847895");
    tempSemesters.replace(0, tempPlan);
    (*modifiedSemesters) = tempSemesters;
    EXPECT_EQ(modifiedSemesters->toArray()[0].toObject()["name"].toString(), "testname239847895");
    QString token = createValidToken();
    EXPECT_TRUE(backend.login(token)) << "Token is " << token.toStdString();
    ASSERT_TRUE(backend.setSemesters(modifiedSemesters->toArray()));
    ASSERT_EQ(backend.getSemesters().toArray()[0].toObject()["name"].toString(), "testname239847895");
    //TODO do not check the id values of the json objects, because it is valid to change them
    ASSERT_EQ(backend.getSemesters(), *modifiedSemesters);
}
#endif
