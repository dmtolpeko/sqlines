/**
 * Copyright (c) 2021 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <thread>
#include <chrono>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSplashScreen>

#include "application.hpp"

Application::Application(int& argc, char** argv) noexcept
 :  QApplication(argc, argv),

    configFilePath(":/config.txt"),
    sourceModes(loadSourceModes()),
    targetModes(loadTargetModes()),
    cmdModes(loadCmdModes()),

    programIsRunning(true),

    settingsLoader("SQLines", "SQLines Studio",
                   this->settings,
                   this->snapshot),
    converter(this->settings, this->cmdModes, this->programIsRunning),
    license(this->settings, programIsRunning),

    mainWindow(this->sourceModes, this->targetModes),
    convertUtilityWidget(this->sourceModes, this->targetModes),
    settingsWidget(),

    mainWindowPresenter(this->settings, this->converter, this->mainWindow),
    convertUtilityPresenter(this->settings, this->cmdModes, this->convertUtilityWidget),
    settingsPresenter(this->settings,
                      this->settingsLoader,
                      this->license,
                      this->settingsWidget,
                      this->mainWindow)
{
    setApplicationName("SQLines Studio");
    setOrganizationName("SQLines");
    setApplicationVersion("3.0");
    setStyleSheet(loadUiStyle());
    makeCacheDir();

    this->converter.addObserver(this->mainWindowPresenter);
    this->license.addObserver(this->settingsPresenter);

    bool lastSessionSaved = this->settings["general.save-last-session"].toInt();
    if (lastSessionSaved) {
        QSplashScreen splashScreen;
        splashScreen.showMessage("Loading last session...");
        splashScreen.show();
        loadFromCheckpoint();
        splashScreen.finish(&this->mainWindow);
    }

    connect(&this->mainWindowPresenter, &presenter::MainWindowPresenter::openAboutPressed,
            &this->aboutWidget, &ui::AboutWidget::show);

    connect(&this->mainWindowPresenter, &presenter::MainWindowPresenter::openSettingsPressed,
            &this->settingsWidget, &ui::SettingsWidget::show);

    connect(&this->mainWindowPresenter, &presenter::MainWindowPresenter::openConvertUtilityPressed,
            &this->convertUtilityWidget, &ui::ConvertUtilityWidget::show);
}

Application::~Application()
{
    this->mutex.lock();
    this->programIsRunning.store(false, std::memory_order_seq_cst);

    cleanCache();
    this->snapshot.clear();
    this->converter.makeSnapshot(this->snapshot);
}

int Application::execute() noexcept
{
    std::thread thread([this]() {
        makeCheckpoint(this->programIsRunning);
    });
    thread.detach();

    return QCoreApplication::exec();
}

void Application::makeCheckpoint(std::atomic<bool>& programIsRunning) noexcept
{
    while (programIsRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(35));

        if (!this->mutex.try_lock()) {
            // Avoid accessing a null mutex when destroying an object
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }
        this->snapshot.clear();
        this->converter.makeSnapshot(this->snapshot);
        this->settingsLoader.saveAppState();

        this->mutex.unlock();
    }
}

void Application::loadFromCheckpoint() noexcept
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->converter.restoreFromSnapshot(this->snapshot);
}

void Application::makeCacheDir() noexcept
{
    QString& cacheDirPath = this->settings["general.dirs.cache-dir"];

    QDir cacheDir(cacheDirPath);
    cacheDir.mkdir("sqlines");

    cacheDirPath = QDir::toNativeSeparators(cacheDirPath + "/sqlines");
}

void Application::cleanCache() noexcept
{
    QDir cacheDir(this->settings["general.dirs.cache-dir"]);

    cacheDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    for (const auto& filePath : cacheDir.entryList()) {
        cacheDir.remove(filePath);
    }

    cacheDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    for(const auto& dirPath : cacheDir.entryList()) {
        QDir subDir(cacheDir.absoluteFilePath(dirPath));
        subDir.removeRecursively();
    }
}

std::map<QString, QString> Application::loadCmdModes() const noexcept
{
    QFile file(this->configFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    std::map<QString, QString> cmdModes;

    // Load source cmd modes
    while (!file.atEnd()) {
        QString line = file.readLine();
        if (line == "TARGETS:\n" || line == "\n") {
            break;
        }

        auto separatorIndex = line.indexOf(':');
        QString key = line;
        key.erase(key.cbegin() + separatorIndex, key.cend());
        key.remove('\n');

        QString cmdMode = line;
        cmdMode.erase(cmdMode.cbegin(), cmdMode.cbegin() + separatorIndex + 1);
        cmdMode.remove('\n');

        cmdModes[key] = cmdMode;
    }

    // Load target cmd modes
    while (!file.atEnd()) {
        QString line = file.readLine();
        auto separatorIndex = line.indexOf(':');

        QString key = line;
        key.erase(key.cbegin() + separatorIndex, key.cend());
        key.remove('\n');

        QString cmdMode = line;
        cmdMode.erase(cmdMode.cbegin(), cmdMode.cbegin() + separatorIndex + 1);
        cmdMode.remove('\n');

        cmdModes[key] = cmdMode;
    }

    return cmdModes;
}

QStringList Application::loadSourceModes() const noexcept
{
    QFile file(this->configFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return { "No config file..." };
    }

    QStringList sourceModes;
    file.readLine();
    while (!file.atEnd()) {
        QString line = file.readLine();
        if (line == "TARGETS:\n" || line == "\n") { // Read only source modes
            break;
        }

        auto separatorIndex = line.indexOf(':');
        line.erase(line.cbegin() + separatorIndex, line.cend());
        line.remove('\n');

        sourceModes.append(line);
    }

    return sourceModes;
}

QStringList Application::loadTargetModes() const noexcept
{
    QFile file(this->configFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return { "No config file..." };
    }

    QStringList targetModes;
    QString line;
    while (line != "TARGETS:\n") { // Skip source modes
        line = file.readLine();
    }

    while (!file.atEnd()) {
        line = file.readLine();

        auto separatorIndex = line.indexOf(':');
        line.erase(line.cbegin() + separatorIndex, line.cend());
        line.remove('\n');

        targetModes.append(line);
    }

    return targetModes;
}

// Load style sheets
QString Application::loadUiStyle() const noexcept
{
    QString filePath = ":/stylesheet.qss";

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString::fromLatin1(file.readAll());
    }
    return {};
}
