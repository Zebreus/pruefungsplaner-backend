#include <plan.h>
#include <semester.h>
#include <QCoreApplication>
#include <QList>
#include "configuration.h"
#include "pruefungsplaner-auth/client.h"
#include "server.h"
#include "src/backendservice.h"

QJsonArray createDefaultSemesters() {
  Semester semester;
  semester.setName("Semester 1");
  Plan* newPlan = new Plan(&semester);
  newPlan->setName("Plan 1");
  semester.setPlans(QList<Plan*>{newPlan});
  QList<Week*> weeks;
  weeks.append(new Week(newPlan));
  weeks.append(new Week(newPlan));
  weeks.append(new Week(newPlan));
  weeks[0]->setName("Woche 1");
  weeks[1]->setName("Woche 2");
  weeks[2]->setName("Woche 3");
  newPlan->setWeeks(weeks);
  for (Week* week : weeks) {
    QList<Day*> days;
    for (int x = 0; x < 6; x++) {
      days.append(new Day(week));
    }
    days[0]->setName("Montag");
    days[1]->setName("Dienstag");
    days[2]->setName("Mittwoch");
    days[3]->setName("Donnerstag");
    days[4]->setName("Freitag");
    days[5]->setName("Samstag");
    for (Day* day : days) {
      QList<Timeslot*> timeslots;
      for (int x = 0; x < 6; x++) {
        timeslots.append(new Timeslot(day));
      }
      timeslots[0]->setName("Block 1");
      timeslots[1]->setName("Block 2");
      timeslots[2]->setName("Block 3");
      timeslots[3]->setName("Block 4");
      timeslots[4]->setName("Block 5");
      timeslots[5]->setName("Block 6");
      day->setTimeslots(timeslots);
    }
    week->setDays(days);
  }
  return QJsonArray{semester.toJsonObject()};
}

QJsonArray initializeSemesters(const Configuration& config) {
  QJsonValue semesters = config.getInitialData();
  if (!semesters.isArray()) {
    QFile storage(config.getStoragePath().path() + "/semesters.json");
    if (storage.open(QFile::ReadOnly)) {
      QJsonDocument document = QJsonDocument().fromJson(storage.readAll());
      if (!document.isArray()) {
        QTextStream(stderr) << "Invalid json in storage file "
                            << storage.fileName() << " ." << Qt::endl;
        exit(1);
      }
      semesters = document.array();
    } else {
      semesters = createDefaultSemesters();
    }
  }
  return semesters.toArray();
}

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);

  QSharedPointer<Configuration> configuration(new Configuration(a.arguments()));

  QSharedPointer<QMutex> backendServiceMutex(new QMutex());
  QSharedPointer<QJsonValue> backendServiceSemesters(
      new QJsonValue(initializeSemesters(*configuration)));

  jsonrpc::Server<BackendService> server(configuration->getPort());
  server.setConstructorArguments(backendServiceSemesters, backendServiceMutex,
                                 configuration);
  server.startListening();
  return a.exec();
}
