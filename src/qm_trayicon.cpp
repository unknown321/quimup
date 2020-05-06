/*
*  qm_mpdcom.h
*  QUIMUP system-tray-icon class
*  © 2008-2018 Johan Spee
*
*  This file is part of Quimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  QUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "qm_trayicon.h"

qm_trayIcon::qm_trayIcon(QMainWindow *parent) : QSystemTrayIcon(parent)
{
    if (objectName().isEmpty())
        setObjectName("tray_icon");

    pxb_st_playing = QPixmap(":/st_playing");
    pxb_st_noconn = QPixmap(":/st_noconn");
    pxb_st_paused = QPixmap(":/st_paused");
    pxb_st_stopped = QPixmap(":/st_stopped");
    pxb_st_idle = QPixmap(":/st_idle");
    pxb_prev = QPixmap(":/st_prev");
    pxb_stop = QPixmap(":/st_stop");
    pxb_play = QPixmap(":/st_play");
    pxb_pause = QPixmap(":/st_pause");
    pxb_next = QPixmap(":/st_next");
    pxb_exit = QPixmap(":/st_exit");

    mpdCom = nullptr;
    setIcon(pxb_st_noconn);

//  the context menu
    QMenu *cmenu = new QMenu(parent);

    a_previous = new QAction("", this);
    a_previous->setIcon(pxb_prev);
    a_previous->setText(tr("Previous"));
    QObject::connect(a_previous, SIGNAL(triggered()), this, SLOT(on_prev_action()));
    cmenu->addAction(a_previous);

    a_stop = new QAction("", this);
    a_stop->setIcon(pxb_stop);
    a_stop->setText(tr("Stop"));
    QObject::connect(a_stop, SIGNAL(triggered()), this, SLOT(on_stop_action()));
    cmenu->addAction(a_stop);

    a_playpause = new QAction("", this);
    a_playpause->setIcon(pxb_play);
    a_playpause->setText(tr("Play"));
    QObject::connect(a_playpause, SIGNAL(triggered()), this, SLOT(on_play_action()));
    cmenu->addAction(a_playpause);

    a_next = new QAction("", this);
    a_next->setIcon(pxb_next);
    a_next->setText(tr("Next"));
    QObject::connect(a_next, SIGNAL(triggered()), this, SLOT(on_next_action()));
    cmenu->addAction(a_next);

    cmenu->addSeparator();

    a_quit = new QAction("", this);
    a_quit->setIcon(pxb_exit);
    a_quit->setText(tr("Quit"));
    QObject::connect(a_quit, SIGNAL(triggered()), this, SLOT(on_quit_action()));
    cmenu->addAction(a_quit);

    this->setContextMenu(cmenu);
//


    QObject::connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_activated(QSystemTrayIcon::ActivationReason)));

    b_mpd_connected = false;
    setToolTip("Idle");
}


void qm_trayIcon::on_prev_action()
{
    if (b_mpd_connected)
        emit signal_tray_menu(ID_prev);
}


void qm_trayIcon::on_stop_action()
{
    if (b_mpd_connected)
        emit signal_tray_menu(ID_stop);
}


void qm_trayIcon::on_play_action()
{
    if (b_mpd_connected)
        emit signal_tray_menu(ID_play);
}


void qm_trayIcon::on_next_action()
{
    if (b_mpd_connected)
        emit signal_tray_menu(ID_next);
}


void qm_trayIcon::on_quit_action()
{
    emit signal_tray_menu(ID_exit);
}


void qm_trayIcon::on_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        {
            emit clicked();
            break;
        }
        case QSystemTrayIcon::MiddleClick:
        {
            on_play_action();
            break;
        }

        case QSystemTrayIcon::Context: // Never happens. QSystemTrayIcon intercepts and shows cmenu.
        {
            break;
        }
        default:
            break;
    }
}


void qm_trayIcon::set_tip(QString str)
{
    if (str.isNull())
    {
        setToolTip("I am a tiltoop");
        return;
    }
    else
        setToolTip(str);
}

// set a pointer to the parent's mpdCom
void qm_trayIcon::set_connected(qm_mpdCom *com, bool isconnected)
{
    mpdCom = com;
    b_mpd_connected = isconnected;
    if (!b_mpd_connected)
        set_mode(ID_nocn);
    else
        set_mode(ID_idle);
}


void qm_trayIcon::set_mode(int mode)
{
    switch (mode)
    {
		case ID_play: // playing
        {
            a_playpause->setIcon(pxb_pause);
            a_playpause->setText(tr("Pause"));
            setIcon(pxb_st_playing);
            break;
        }

		case ID_paus: // paused
        {
            a_playpause->setIcon(pxb_play);
            a_playpause->setText(tr("Play"));
            setIcon(pxb_st_paused);
            break;
        }

		case ID_stop: // stopped
        {
            a_playpause->setIcon(pxb_play);
            a_playpause->setText(tr("Play"));
            setIcon(pxb_st_stopped);
            break;
        }

		case ID_nocn: // noconnection
        {
            a_playpause->setIcon(pxb_play);
            a_playpause->setText(tr("Play"));
            setIcon(pxb_st_noconn);
            set_tip(" " + tr("Not Connected") + " ");
            break;
        }

		default:
        {
            a_playpause->setIcon(pxb_play);
            a_playpause->setText(tr("Play"));
            setIcon(pxb_st_idle);
            set_tip(" " + tr("Idle") + " ");
            break;
        }
    }
}

qm_trayIcon::~qm_trayIcon()
{}
