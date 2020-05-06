/*
*  qm_core.cpp
*  QUIMUP core class
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

/*
        ***************************************************************************
        The only purpose of the 'core' is to provide KDE with a main program
   window
    (albeit invisible) that can be closed on shutdown (or logout).

    The player window ignores the close-event to allow close-to-tray behavior.

    But it will also ignore the close-event evoked by KDE on shutdown, which
    consequently aborts the logout procedure.

    This 'core' however accepts the close-event, and it's child, the player
    window, is closed with it.

        The player can close itself by calling qApp->quit().
        ***************************************************************************
*/

#include "qm_core.h"

qm_core::qm_core() {
  if (objectName().isEmpty())
    setObjectName("qm_core");

  player = new qm_player();
}

// called from main.cpp
void qm_core::on_message_from_2nd_instance(const QString &msg) {
  if (msg.startsWith("file://") || msg.startsWith("-play:file:")) {
    player->wakeup_call(true);
    player->browser_window->plist_view->on_open_with_request(msg);
  } else
    player->wakeup_call(false);
}

// called from main.cpp
void qm_core::on_system_quit() { player->on_shudown(); }

qm_core::~qm_core() { player->on_shudown(); }
