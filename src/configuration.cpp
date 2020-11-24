#include "configuration.h"

Configuration::Configuration(const QList<QString> &arguments, QObject *parent) : QObject(parent), address(""), port(0)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Pruefungsplaner backend server");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configFileOption("config",
            "Load configuration from <config>.",
            "config");
    parser.addOption(configFileOption);

    QCommandLineOption portOption(QStringList() << "p" << "port",
            "The server will listen on port <port>.",
            "port");
    parser.addOption(portOption);

    QCommandLineOption addressOption(QStringList() << "a" << "address",
            "The server will listen on address <address>.",
            "address");
    parser.addOption(addressOption);

    QCommandLineOption publicKeyOption("public-key",
            "The public RSA256 key file in .pem format.",
            "privatekey");
    parser.addOption(publicKeyOption);

    QCommandLineOption issuerOption("issuer",
            "The jwt need to have this issuer set.",
            "issuer");
    parser.addOption(issuerOption);

    QCommandLineOption claimsOption("claims",
            "Specify a required claim. You can use this option multiple times to require multiple claims. If present, all required claims set in the config file are ignored",
            "claims");
    parser.addOption(claimsOption);

    QCommandLineOption checkOption("check",
            "If set the public key and issuer will be checked with the auth server.");
    parser.addOption(checkOption);

    QCommandLineOption noCheckOption("no-check",
            "If set the public key and issuer will not be checked with the auth server. This is the default behaviour.");
    parser.addOption(noCheckOption);

    QCommandLineOption retrieveOption("retrieve",
            "If set the public key and issuer will be retrieved from the auth server.");
    parser.addOption(retrieveOption);

    QCommandLineOption noRetrieveOption("no-retrieve",
            "If set the public key and issuer will not be retrieved from the auth server. This is the default behaviour.");
    parser.addOption(noCheckOption);

    QCommandLineOption authServerOption("auth-server",
            "The url of the auth server. This should look like \"wss://0.0.0.0:443\".",
            "auth-server");
    parser.addOption(authServerOption);

    QCommandLineOption storagePathOption("storage",
            "The data will be stored and loaded from <storage>",
            "storage");
    parser.addOption(storagePathOption);

    QCommandLineOption initialFilesOption("initial-files",
            "You can provide a path to csv files for initialization",
            "initial-files");
    parser.addOption(initialFilesOption);


    parser.process(arguments);

    address = parser.value(addressOption);

    QString portString = parser.value(portOption);
    if(portString != ""){
        bool ok;
        uint portInt = portString.toUInt(&ok);
        if(!ok){
            failConfiguration("Port " + portString + " is not a number.");
        }else{
            if(portInt > 0 && portInt <= 65535){
                port = portInt;
            }else{
                failConfiguration("Port " + QString(portInt) + " is out of range (1-65535).");
            }
        }
    }

    QString authServerUrl = parser.value(authServerOption);
    if(parser.isSet(retrieveOption)){
        retrieve.reset(new bool(true));
    }else if(parser.isSet(noRetrieveOption)){
        retrieve.reset(new bool(false));
    }
    if(parser.isSet(checkOption)){
        check.reset(new bool(true));
    }else if(parser.isSet(noCheckOption)){
        check.reset(new bool(false));
    }
    //TODO check that auth server url is set after reading config file

    QString publicKeyFileString = parser.value(publicKeyOption);
    if(publicKeyFileString != "" && parser.isSet(retrieveOption)){
        failConfiguration("You can not specify --retrieve and a public key file.");
    }else if(publicKeyFileString != ""){
        QFile publicKeyFile(publicKeyFileString);
        readPublicKey(publicKeyFile);
    }

    QString issuerString = parser.value(issuerOption);
    if(issuerString != "" && parser.isSet(retrieveOption)){
        failConfiguration("You can not specify --retrieve and a custom issuer.");
    }else if(issuerString != ""){
        issuer = issuerString;
    }

    QList<QString> parsedClaims = parser.values(claimsOption);
    if(parsedClaims.size() > 0){
        requiredClaims = parsedClaims;
    }

    QString parsedInitialFiles = parser.value(initialFilesOption);
    if(parsedInitialFiles != ""){
        loadInitialFiles(parsedInitialFiles);
    }

    QString parsedStoragePath = parser.value(storagePathOption);
    if(parsedStoragePath != ""){
        loadStoragePath(parsedStoragePath);
    }

    QString parsedConfigurationFile = parser.value(configFileOption);
    if(parsedConfigurationFile == ""){
        bool found = false;
        for(auto configFilePath : defaultConfigurationFiles){
            QFile configFile(configFilePath);
            if(configFile.exists()){
                loadConfiguration(configFile);
                found = true;
                break;
            }
        }
        if(!found){
            QString defaultConfigFilesMessage;
            if(defaultConfigurationFiles.size() > 0){
                defaultConfigFilesMessage = "create one at ";
                for(uint i = 0; i < defaultConfigurationFiles.size() - 1; i++){
                    defaultConfigFilesMessage += defaultConfigurationFiles[i];
                    defaultConfigFilesMessage += ", ";
                }
                defaultConfigFilesMessage += defaultConfigurationFiles[defaultConfigurationFiles.size() - 1];
                defaultConfigFilesMessage += " or";
            }
            failConfiguration("No valid configuration file found. You can " + defaultConfigFilesMessage + " specify your configuration with the --config option.");
        }
    }else{
        loadConfiguration(parsedConfigurationFile);
    }

    checkConfiguration();
}

QString Configuration::getAddress() const
{
    return address;
}

quint16 Configuration::getPort() const
{
    return port;
}

QString Configuration::getPublicKey() const
{
    return publicKey;
}

bool Configuration::checkIssuer(const QString &issuer) const
{
    if(issuer == this->issuer){
        return true;
    }else{
        return false;
    }
}

bool Configuration::checkClaims(const QList<QString> &suppliedClaims) const
{
    for(const QString& claim : requiredClaims){
        if(!suppliedClaims.contains(claim)){
            return false;
        }
    }
    return true;
}

QJsonValue Configuration::getInitialData() const
{
    if(!initialData.isNull()){
        return *initialData;
    }else{
        return QJsonValue(QJsonValue::Undefined);
    }
}

QDir Configuration::getStoragePath() const
{
    if(!storagePath.isNull()){
        return *storagePath;
    }else{
        //TODO This should not happen. Maybe a pointer is not the best data structure
        failConfiguration("No storage path set. This should be impossible.");
        return (QDir)nullptr;
    }
}

void Configuration::loadConfiguration(const QFile& file)
{
    try{
        auto config = cpptoml::parse_file(file.fileName().toStdString());
        auto parseAddress = config->get_as<std::string>("server.address").value_or(defaultAddress);
        auto parsePort = config->get_as<uint16_t>("server.port").value_or(defaultPort);
        auto parsePublicKey = config->get_as<std::string>("security.publicKey").value_or(defaultPublicKey);
        auto parseIssuer = config->get_as<std::string>("security.issuer").value_or(defaultIssuer);
        auto parseAuthUrl = config->get_as<std::string>("security.authUrl").value_or(defaultAuthUrl);
        auto parseClaims = config->get_array_of<std::string>("security.claims").value_or(defaultRequiredClaims);
        bool parseRetrieve = config->get_as<bool>("security.retrieveSettings").value_or(defaultRetrieveSettings);
        bool parseCheck = config->get_as<bool>("security.checkSettings").value_or(defaultCheckSettings);
        auto parseInitialPath = config->get_as<std::string>("backend.initialData").value_or("");
        auto parseStoragePath = config->get_as<std::string>("backend.storagePath").value_or(defaultStoragePath);

        if(address == ""){
            address = QString().fromStdString(parseAddress);
        }
        if(port == 0){
            port = parsePort;
        }
        if(check.isNull()){
            check.reset(new bool(parseCheck));
        }
        if(retrieve.isNull()){
            retrieve.reset(new bool(parseRetrieve));
        }
        if(publicKey == "" && !retrieve){
            QFile publicKeyFile(QString().fromStdString(parsePublicKey));
            readPublicKey(publicKeyFile);
        }
        if(issuer == ""){
            issuer = QString().fromStdString(parseIssuer);
        }
        if(authUrl == ""){
            authUrl = QString().fromStdString(parseAuthUrl);
        }
        if(storagePath.isNull()){
            loadStoragePath(QString().fromStdString(parseStoragePath));
        }
        if(initialData.isNull() && parseInitialPath != ""){
            loadInitialFiles(QString().fromStdString(parseInitialPath));
        }
        if(requiredClaims.size() == 0){
            for (const auto& claim : parseClaims)
            {
                requiredClaims.append(QString().fromStdString(claim));
            }
        }

    } catch (const cpptoml::parse_exception& e){
        failConfiguration("Failed to parse config file " + file.fileName() + " :\n" + QString(e.what()) );
    }

}

void Configuration::readPublicKey(QFile &publicKeyFile)
{
    if(!publicKeyFile.exists()){
        failConfiguration("Public key file " + publicKeyFile.fileName() + " does not exist.");
    }else if(!publicKeyFile.isReadable()){
        failConfiguration("Public key file " + publicKeyFile.fileName() + " does not existis not readable.");
    }

    if(publicKeyFile.open(QFile::ReadOnly | QFile::Text)){
        QString readPublicKey = QTextStream(&publicKeyFile).readAll();
        publicKeyFile.close();
        checkPublicKey(readPublicKey);
        publicKey = readPublicKey;
    }else{
        failConfiguration("Failed to read public key from " + publicKeyFile.fileName());
    }
}

void Configuration::checkPublicKey(const QString& publicKey)
{
    if(publicKey != ""){
        failConfiguration("Public key is empty.");
    }

    QString checkPublicKeyCommand("echo -n '%1' | openssl pkey -inform PEM -pubin -in -noout >/dev/null 2>&1");

    checkPublicKeyCommand = checkPublicKeyCommand.arg(publicKey);

    int result = system(checkPublicKeyCommand.toUtf8());
    if(result != 0){
        failConfiguration("Public key file is invalid (Code " + QString(result) + ").");
    }
}

void Configuration::retrieveSettings(const QString &)
{
    warnConfiguration("Retrieving settings from server not implemented yet");
    return;
}

void Configuration::checkKeyAndIssuer(const QString &)
{
    warnConfiguration("Checking setting with server not implemented yet");
    return;
}

void Configuration::loadInitialFiles(const QString &initialPath)
{
    if(initialPath == ""){
        return;
    }else if(!QDir(initialPath).exists()){
        failConfiguration("Initial data path " + initialPath + " does not exist.");
    }else if(!QDir(initialPath).isReadable()){
        failConfiguration("Initial data path " + initialPath + " is not readable.");
    }

    PlanCsvHelper csvHelper(initialPath);
    QSharedPointer<Plan> plan = csvHelper.readPlan();
    if(plan.isNull()){
        failConfiguration("Failed to read initial plan at " + initialPath);
    }
    QJsonArray semesters{plan->toJsonObject()};
    initialData.reset(new QJsonValue(semesters));
}

void Configuration::loadStoragePath(const QString &storagePathString)
{
    if(storagePathString == ""){
        return;
    }else if(!QDir(storagePathString).exists()){
        failConfiguration("Storage path " + storagePathString + " does not exist.");
    }else if(!QDir(storagePathString).isReadable()){
        failConfiguration("Storage path " + storagePathString + " is not readable.");
    }

    storagePath.reset(new QDir(storagePathString));
}

void Configuration::failConfiguration(const QString &message) const
{
    QTextStream(stderr) << message << Qt::endl;
    exit(1);
}

void Configuration::warnConfiguration(const QString &message) const
{
    qDebug() << message;
}

void Configuration::checkConfiguration()
{
    if(port == 0){
        failConfiguration("You specified the only invalid port, which is 0.");
    }

    if(!QHostAddress().setAddress(address)){
        failConfiguration("The address " + address + " seems to be invalid.");
    }

    if(publicKey == ""){
        failConfiguration("No public key specified.");
    }

    if(issuer == ""){
        failConfiguration("No issuer specified.");
    }

    if(!storagePath->exists()){
        failConfiguration("The storage path does not exist.");
    }else if(!storagePath->isReadable()){
        //TODO check writable
        failConfiguration("The storage path is not readable.");
    }

    if(requiredClaims.size() == 0){
        warnConfiguration("You specified no required claims.");
    }

}
