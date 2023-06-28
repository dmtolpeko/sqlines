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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <mutex>
#include <atomic>
#include <map>
#include <QApplication>
#include <QString>
#include <QStringList>

#include "settings.hpp"
#include "snapshot.hpp"
#include "settingsloader.hpp"
#include "converter.hpp"
#include "license.hpp"
#include "mainwindowpresenter.hpp"
#include "convertutilitypresenter.hpp"
#include "settingspresenter.hpp"
#include "mainwindow.hpp"
#include "convertutilitywidget.hpp"
#include "settingswidget.hpp"
#include "aboutwidget.hpp"

class Application final : private QApplication {
public:
    Application(int& argc, char** argv) noexcept;
    ~Application() final;

    int execute() noexcept;

private:
    void makeCheckpoint(std::atomic<bool>& programIsRunning) noexcept;
    void loadFromCheckpoint() noexcept;

    void makeCacheDir() noexcept;
    void cleanCache() noexcept;

    std::map<QString, QString> loadCmdModes() const noexcept;
    QStringList loadSourceModes() const noexcept;
    QStringList loadTargetModes() const noexcept;
    QString loadUiStyle() const noexcept;

private:
    QString configFilePath;
    QStringList sourceModes;
    QStringList targetModes;
    std::map<QString, QString> cmdModes;

    std::atomic<bool> programIsRunning;
    std::mutex mutex;

    model::Settings settings;
    model::Snapshot snapshot;
    model::SettingsLoader settingsLoader;
    model::Converter converter;
    model::License license;

    ui::MainWindow mainWindow;
    ui::ConvertUtilityWidget convertUtilityWidget;
    ui::SettingsWidget settingsWidget;
    ui::AboutWidget aboutWidget;

    presenter::MainWindowPresenter mainWindowPresenter;
    presenter::ConvertUtilityPresenter convertUtilityPresenter;
    presenter::SettingsPresenter settingsPresenter;
};

#endif // APPLICATION_HPP
