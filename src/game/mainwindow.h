// This file is part of Dust Rallycross (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

class Renderer;

/*! \class MainWindow
 *  \brief The main window of the game.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    //! Constructor
    MainWindow();
    
    //! Destructor
    ~MainWindow();

    //! Return the singleton MainWindow.
    static MainWindow * instance();

    //! Run the game
    void runGame();

protected:

    //! \reimp
    void closeEvent(QCloseEvent * event);

private:

    void populateMenuBar();

    Renderer * m_renderer;

    static MainWindow * m_instance;
};

#endif // MAINWINDOW_H

