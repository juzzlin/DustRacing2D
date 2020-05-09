// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "difficultyprofile.hpp"
#include "inputhandler.hpp"

#include <QString>
#include <map>

class Track;

//! Singleton settings class that wraps the use of QSettings.
class Settings
{
public:
    //! Constructor.
    Settings();

    static Settings & instance();

    void saveResolution(int hRes, int vRes, bool fullScreen);

    void loadResolution(int & hRes, int & vRes, bool & fullScreen);

    void saveKeyMapping(int player, InputHandler::Action action, int key);

    int loadKeyMapping(int player, InputHandler::Action action);

    void saveDifficulty(DifficultyProfile::Difficulty difficulty);

    DifficultyProfile::Difficulty loadDifficulty() const;

    void saveVSync(int value);

    int loadVSync();

    void saveValue(QString key, int value);

    int loadValue(QString key, int defaultValue = 0);

    static QString difficultyKey();

    static QString fpsKey();

    static QString lapCountKey();

    static QString soundsKey();

    static QString screenKey();

    static QString vsyncKey();

private:
    QString combineActionAndPlayer(int player, InputHandler::Action action);

    static Settings * m_instance;
    std::map<InputHandler::Action, QString> m_actionToStringMap;
};

#endif // SETTINGS_HPP
