/*
*  qm_player.cpp
*  QUIMUP main player window
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

#include "qm_player.h"

qm_player::qm_player() {
  if (objectName().isEmpty())
    setObjectName("player");

  this->setAcceptDrops(true);

  setupUI();

  init_vars(); // incl. config

  retranslateUI();

  if (config->start_minimized)
    this->showMinimized();
  else
    this->show();

  window_W_max = this->width();
  window_H_max = this->height();

  if (config->player_maxmode) {
    window_H = window_H_max;
    setFixedSize(window_W_max, window_H_max);
  } else { // minimode
    window_H = 106;
    center_widget->hide();
    setFixedSize(window_W_max, window_H);
    config->player_maxmode = false;
  }

  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setSizePolicy(sizePolicy);

  this->setAttribute(Qt::WA_QuitOnClose, false);
  // trigger window-manager update: hide & show
  this->hide();

  if (config->start_minimized)
    this->showMinimized();
  else
    this->show();

  this->activateWindow();

  mpdCom->configure();

  // connect and set status
  if (config->auto_connect) {
    printf("Auto-connecting, as configured\n");

    if (!mpdCom->mpd_connect()) {
      printf("Failed to connect to MPD\n");
      // mpdCom->show_messagebox(tr("Could not connect to MPD!"), tr("Please
      // check the 'Connect' tab in the 'Settings' window."));
    }
  } else {
    printf("NOT Connecting, as configured\n");
    on_connect_sgnl(false);
  }
}

// from mpdCom
void qm_player::on_connect_sgnl(bool isconnected) {
  if (isconnected) {
    b_mpd_connected = true;
    settings_window->set_connected(mpdCom, true);
    browser_window->set_connected(mpdCom, true);
    browser_window->plist_view->set_connected(mpdCom, true);
    browser_window->lib_view->set_connected(mpdCom, true);
    if (b_use_trayicon)
      the_trayicon->set_connected(mpdCom, true);
    set_status(0, " ");
    if (config->mpd_musicpath_status == 1)
      context_menu->setEnabled(true);
    else
      context_menu->setEnabled(false);
  } else {
    b_mpd_connected = false;
    settings_window->set_connected(mpdCom, false);
    browser_window->set_connected(mpdCom, false);
    browser_window->plist_view->set_connected(mpdCom, false);
    browser_window->lib_view->set_connected(mpdCom, false);
    if (b_use_trayicon)
      the_trayicon->set_connected(mpdCom, false);
    set_status(-1, tr("Not connected"));
    context_menu->setEnabled(false);
  }
}

void qm_player::setupUI() {
  if (objectName().isEmpty())
    setObjectName(QString::fromUtf8("qm_player"));

  QPixmap qpx;
  qpx = QPixmap(":/mn_icon.png");
  setWindowIcon(qpx);
  setWindowTitle("Quimup");

  main_widget = new QWidget();
  vbox_all = new QVBoxLayout(main_widget);
  setCentralWidget(main_widget);
  vbox_all->setMargin(6);
  vbox_all->setSpacing(4);

  main_display = new QLabel(); // used as QFrame
  main_display->setFixedSize(324, 50);
  main_display->setFrameShape(QFrame::Panel);
  main_display->setFrameShadow(QFrame::Sunken);

  lb_extension = new QLabel(main_display);
  lb_extension->setGeometry(QRect(10, 28, 50, 20));
  lb_extension->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  lb_extension->setWordWrap(false);

  lb_time = new clickLabel(main_display);
  lb_time->setGeometry(QRect(200, 28, 114, 20));
  lb_time->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  lb_time->setWordWrap(false);
  lb_time->setTextFormat(Qt::RichText);

  title_scroller = new qm_Scroller(main_display);
  title_scroller->setGeometry(QRect(2, 2, 320, 24));
  title_scroller->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

  lb_kbps = new QLabel(main_display);
  lb_kbps->setGeometry(QRect(66, 28, 64, 20));
  lb_kbps->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  lb_kbps->setWordWrap(false);

  lb_kHz = new QLabel(main_display);
  lb_kHz->setGeometry(QRect(140, 28, 64, 20));
  lb_kHz->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  lb_kHz->setWordWrap(false);

  vbox_all->addWidget(main_display);

  ds_progress = new qm_clickprogressbar();
  ds_progress->setOrientation(Qt::Horizontal);
  ds_progress->setFixedSize(324, 8);
  ds_progress->setStyleSheet("QSlider::groove:horizontal {"
                             "margin: 0 0 0 0;"
                             "border: 1px solid #aaa;"
                             "background: transparent;"
                             "height: 3px;"
                             "border-radius: 2px;}"
                             "QSlider::sub-page:horizontal {"
                             "border-image: url(:/groove_bg_noconn.png); }");
  ds_progress->setMinimum(0);
  vbox_all->addWidget(ds_progress);

  spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
  vbox_all->addItem(spacer);

  center_widget = new QLabel();
  center_widget->setFixedSize(324, 200);

  hbox_center = new QHBoxLayout(center_widget);
  hbox_center->setMargin(0);
  hbox_center->setSpacing(2);
  hbox_center->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  lb_albumart = new QLabel();
  lb_albumart->setFixedSize(200, 200);
  lb_albumart->setScaledContents(false);
  lb_albumart->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  hbox_center->addWidget(lb_albumart);
  vbox_cright = new QVBoxLayout();
  vbox_cright->setMargin(0);
  vbox_cright->setSpacing(0);
  vbox_cright->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  vbox_cright->setGeometry(
      QRect(0, 0, 122, 200)); // lb_album + lb_comment must fit
  lb_album = new QLabel();
  lb_album->setWordWrap(true);
  lb_album->setMargin(2);
  lb_album->setMinimumSize(122, 4);
  lb_album->setTextFormat(Qt::RichText);
  lb_album->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  vbox_cright->addWidget(lb_album);
  lb_comment = new QLabel();
  lb_comment->setMargin(2);
  lb_comment->setWordWrap(true);
  // lb_comment->setTextFormat(Qt::RichText);
  lb_comment->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  vbox_cright->addWidget(lb_comment);
  hbox_center->addLayout(vbox_cright);
  vbox_all->addWidget(center_widget);

  hbox_bottom = new QHBoxLayout();
  hbox_bottom->setMargin(0);
  hbox_bottom->setSpacing(4);
  bt_prev = new QPushButton();
  bt_prev->setFixedSize(34, 28);
  bt_prev->setIcon(QIcon(":/mn_prev.png"));
  hbox_bottom->addWidget(bt_prev);
  bt_stop = new QPushButton();
  bt_stop->setFixedSize(34, 28);
  bt_stop->setIcon(QIcon(":/mn_stop.png"));
  hbox_bottom->addWidget(bt_stop);
  bt_playpause = new QPushButton();
  bt_playpause->setFixedSize(34, 28);
  bt_playpause->setIcon(QIcon(":/mn_play.png"));
  bt_playpause->setDefault(false);
  hbox_bottom->addWidget(bt_playpause);
  bt_next = new QPushButton();
  bt_next->setFixedSize(34, 28);
  bt_next->setIcon(QIcon(":/mn_next.png"));
  hbox_bottom->addWidget(bt_next);
  bt_browser = new QPushButton();
  bt_browser->setFixedSize(34, 28);
  bt_browser->setIcon(QIcon(":/mn_playlist.png"));
  hbox_bottom->addWidget(bt_browser);
  vol_slider = new QSlider(Qt::Horizontal);
  vol_slider->setRange(0, 100);
  vol_slider->setFixedSize(58, 28);
  vol_slider->setStyleSheet("QSlider::groove:horizontal {"
                            "margin: 1px 0 1px 0;"
                            "border: 1px solid #aaa;"
                            "background: transparent;"
                            "height: 3px;"
                            "border-radius: 2px; }"
                            "QSlider::handle:horizontal {"
                            "image: url(:/vol_handle.png);"
                            "height: 15px;"
                            "width: 6px;"
                            "margin: -4px 0;}");
  hbox_bottom->addWidget(vol_slider);
  bt_sizer = new QPushButton();
  bt_sizer->setFixedSize(34, 28);
  bt_sizer->setIcon(QIcon(":/mn_sizer.png"));
  hbox_bottom->addWidget(bt_sizer);
  bt_setts = new QPushButton();
  bt_setts->setFixedSize(34, 28);
  bt_setts->setIcon(QIcon(":/mn_config.png"));
  hbox_bottom->addWidget(bt_setts);
  vbox_all->addLayout(hbox_bottom);

  // buttonmapper
  button_mapper = new QSignalMapper();

  button_mapper->setMapping(bt_playpause, ID_play);
  QObject::connect(bt_playpause, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(bt_stop, ID_stop);
  QObject::connect(bt_stop, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(bt_next, ID_next);
  QObject::connect(bt_next, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(bt_prev, ID_prev);
  QObject::connect(bt_prev, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(bt_browser, ID_plst);
  QObject::connect(bt_browser, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(bt_setts, ID_sets);
  QObject::connect(bt_setts, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(lb_time, ID_time);
  QObject::connect(lb_time, SIGNAL(clicked()), button_mapper, SLOT(map()));

  button_mapper->setMapping(bt_sizer, ID_mmax);
  QObject::connect(bt_sizer, SIGNAL(clicked()), button_mapper, SLOT(map()));

  QObject::connect(button_mapper, SIGNAL(mapped(int)), this,
                   SLOT(on_signal(int)));
  // end buttonmapper

  QObject::connect(ds_progress, SIGNAL(pressed(QMouseEvent *)), this,
                   SLOT(lock_progress(QMouseEvent *)));

  QObject::connect(ds_progress, SIGNAL(clicked()), this,
                   SLOT(unlock_progress()));

  minimizer = new QTimer();
  QObject::connect(minimizer, SIGNAL(timeout()), this, SLOT(minimice()));
  maximizer = new QTimer();
  QObject::connect(maximizer, SIGNAL(timeout()), this, SLOT(lock_progress()));

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
                   SLOT(on_contextmenu()));

  // CONTEXT MENU
  context_menu = new QMenu();
  a_menuheader = new QAction(context_menu);
  a_menuheader->setText(tr("Folder actions:"));
  QFont font;
  font.setPointSize(8);
  a_menuheader->setFont(font);
  context_menu->addAction(a_menuheader);
  a_menuheader->setEnabled(false);

  a_opendir = new QAction(context_menu);
  a_opendir->setText(tr("File Manager"));
  a_opendir->setIcon(QIcon(":/menu_dir.png"));
  QObject::connect(a_opendir, SIGNAL(triggered()), this,
                   SLOT(open_directory()));
  context_menu->addAction(a_opendir);

  a_edittags = new QAction(context_menu);
  a_edittags->setText(tr("Tag Editor"));
  a_edittags->setIcon(QIcon(":/menu_edit.png"));
  QObject::connect(a_edittags, SIGNAL(triggered()), this, SLOT(edit_tags()));
  context_menu->addAction(a_edittags);

  a_openaart = new QAction(context_menu);
  a_openaart->setText(tr("Image Viewer"));
  a_openaart->setIcon(QIcon(":/menu_view.png"));
  QObject::connect(a_openaart, SIGNAL(triggered()), this,
                   SLOT(show_albumart()));
  context_menu->addAction(a_openaart);
  context_menu->addSeparator();

  a_loadtags = new QAction(context_menu);
  a_loadtags->setText(tr("Rescan tags"));
  a_loadtags->setIcon(QIcon(":/menu_load.png"));
  QObject::connect(a_loadtags, SIGNAL(triggered()), this, SLOT(reload_tags()));
  context_menu->addAction(a_loadtags);

  QObject::connect(vol_slider, SIGNAL(valueChanged(int)), this,
                   SLOT(on_volumedial(int)));
}

void qm_player::on_volumedial(int vol) {
  if (b_mpd_sets_volume || !b_mpd_connected)
    return;

  if (vol != current_volume) {
    current_volume = vol;
    mpdCom->set_volume(vol);
    if (config->show_tooltips)
      vol_slider->setToolTip("Volume " + QString::number(vol) + " %");
  }
}

void qm_player::minimice() {
  // fade the album art first
  if (b_minmax_xfade) {
    alphacycler -= 6;
    if (alphacycler < 0)
      alphacycler = 0;

    int a = alphacycler;
    QImage xfade = img_xfade_input;
    for (int j = 0; j < albumart_H; j++) {
      QRgb *px = (reinterpret_cast<QRgb *>(xfade.scanLine(j)));
      for (int k = 0; k < albumart_W; k++) {
        uchar r = uchar((int(qRed(*px)) * a + cR * (255 - a)) / 255);
        uchar g = uchar((int(qGreen(*px)) * a + cG * (255 - a)) / 255);
        uchar b = uchar((int(qBlue(*px)) * a + cB * (255 - a)) / 255);
        *px = qRgba(r, g, b, 255);
        px++;
      }
    }

    pxb_xfading = QPixmap::fromImage(xfade);

    lb_albumart->setPixmap(pxb_xfading);

    if (alphacycler == 0) {
      center_widget->hide();
      b_minmax_busy = false;
      b_minmax_xfade = false;
    }
    return;
  }

  window_H -= 16;
  if (window_H >= 106) {
    setFixedSize(window_W_max, window_H);
  } else {
    setFixedSize(window_W_max, 106);
    minimizer->stop();
  }
}

void qm_player::init_vars() {
  browser_window = new qm_browser();
  QObject::connect(browser_window, SIGNAL(sgnl_keypressed(QKeyEvent *)), this,
                   SLOT(on_browser_keypress(QKeyEvent *)));

  settings_window = new qm_settings();
  QObject::connect(settings_window, SIGNAL(sgnl_fonts()), this,
                   SLOT(set_fonts()));
  QObject::connect(settings_window, SIGNAL(sgnl_colors()), this,
                   SLOT(set_colors()));
  QObject::connect(settings_window, SIGNAL(sgnl_markplayed()),
                   browser_window->plist_view, SLOT(set_markplayed()));
  QObject::connect(settings_window, SIGNAL(sgnl_plist_auto_cols()),
                   browser_window->plist_view, SLOT(set_auto_columns()));
  QObject::connect(settings_window, SIGNAL(sgnl_lib_auto_cols()),
                   browser_window->lib_view, SLOT(set_auto_columns()));

  mpdCom = new qm_mpdCom(); // before config and current_songinfo!
  QObject::connect(mpdCom, SIGNAL(sgnl_new_song()), this, SLOT(on_new_song()));
  QObject::connect(mpdCom, SIGNAL(sgnl_status_info(mpd_status *)), this,
                   SLOT(on_new_status(mpd_status *)));
  QObject::connect(mpdCom, SIGNAL(sgnl_connected(bool)), this,
                   SLOT(on_connect_sgnl(bool)));
  QObject::connect(mpdCom, SIGNAL(sgnl_status(int, QString)), this,
                   SLOT(set_status(int, QString)));

  config = new qm_config(); // constructor loads values
  mpdCom->set_config(config);
  settings_window->set_config(config);
  browser_window->set_config(config);
  browser_window->plist_view->set_config(config);
  browser_window->lib_view->set_config(config);

  if (config->version != "1.4.4") {
    config->version = "1.4.4";
    QMessageBox msgBox;
    msgBox.setWindowTitle("Quimup");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("<b>" + tr("Welcome to Quimup") + " " + config->version +
                   "</b>");

    QString message =
        tr("Note that Quimup must be able to access MPD's music directory to "
           "find album-art and use external programs.") +
        "\n\n" +
        tr("Files can be added directly from your file manager (drag & drop or "
           "'open with'), but only if you connect to MPD using a socket.") +
        "\n\n" + tr("For more details see the tips in the 'about' tab of the "
                    "settings window.") +
        "\n\n" + tr("This message will not be shown again.");

    msgBox.setInformativeText(message);
    msgBox.exec();
  }

  setWindowTitle("Quimup " + config->version);

  current_songinfo = new qm_songInfo;
  mpdCom->set_sInfo(current_songinfo);

  kbps_count_down = 0;
  current_status = -99; // allow all
  current_volume = 0;
  current_type = TP_NOSONG;
  current_songnr = -1;
  if (config->show_tooltips)
    vol_slider->setToolTip("Volume 0 %");
  song_total_time = 0;
  song_previous_time = 0;
  b_skip_khz = false;
  b_repeat = false;
  b_random = false;
  b_single = false;
  b_consume = false;
  b_xfade = false;
  b_mpd_connected = false;
  b_mpd_sets_volume = false;
  b_stream = false;
  b_minmax_busy = false;
  b_settings_hidden = false;
  b_browser_hidden = false;
  b_nosong = false;
  b_lock_progress = false;
  b_really_close = false;
  b_need_newSong = false;
  b_reshow_browser = false;
  b_reshow_settings = false;
  current_art_path = "nopic";

  if (config->scroller_delay < 20)
    config->scroller_delay = 20;
  title_scroller->set_delay(config->scroller_delay);

  // 'load' or they will disappear after a repaint
  img_mn_play = QIcon(":/mn_play");
  img_mn_pause = QIcon(":/mn_pause");
  pxb_led_playing.load(":/led_playing");
  pxb_led_noconn.load(":/led_noconn");
  pxb_led_paused.load(":/led_paused");
  pxb_led_stopped.load(":/led_stopped");
  pxb_led_idle.load(":/led_idle");

  if (config->use_trayicon) {
    the_trayicon = new qm_trayIcon(this);

    the_trayicon->show();

    // the_trayicon->installEventFilter(new qm_eventHandler(this)); // No events
    // are filtered ever

    QObject::connect(the_trayicon, SIGNAL(clicked()), this,
                     SLOT(on_tray_clicked()));
    QObject::connect(the_trayicon, SIGNAL(signal_tray_menu(int)), this,
                     SLOT(on_signal(int)));

    the_trayicon->set_mode(ID_idle);

    b_use_trayicon = true;
  } else
    b_use_trayicon = false;

  set_colors();
  set_fonts();

  move(config->player_X, config->player_Y);

  vol_slider->setFocus(Qt::OtherFocusReason);
}

void qm_player::lock_progress() {
  // fade the album art last
  if (b_minmax_xfade) {
    alphacycler += 6;
    if (alphacycler >= 255) {
      b_minmax_busy = false;
      lb_albumart->setPixmap(pxb_albmart);
      maximizer->stop();
      return; // done
    }

    int a = alphacycler;
    QImage xfade = img_xfade_input;
    for (int j = 0; j < albumart_H; j++) {
      QRgb *px = (reinterpret_cast<QRgb *>(xfade.scanLine(j)));
      for (int k = 0; k < albumart_W; k++) {
        uchar r = uchar((int(qRed(*px)) * a + cR * (255 - a)) / 255);
        uchar g = uchar((int(qGreen(*px)) * a + cG * (255 - a)) / 255);
        uchar b = uchar((int(qBlue(*px)) * a + cB * (255 - a)) / 255);
        *px = qRgba(r, g, b, 255);
        px++;
      }
    }

    pxb_xfading = QPixmap::fromImage(xfade);
    lb_albumart->setPixmap(pxb_xfading);
    return;
  }

  window_H += 16;
  if (window_H <= window_H_max) {
    setFixedSize(window_W_max, window_H);
  } else {
    window_H = window_H_max;
    setFixedSize(window_W_max, window_H_max);
    center_widget->show();
    b_minmax_xfade = true;
  }
}

void qm_player::on_tray_clicked() {
  if (isVisible()) {
    hide();
  } else {
    showNormal();
  }
}

void qm_player::on_curentsong_deleted() {
  current_songinfo->nosong = true;
  on_new_song();
  set_status(MPD_STATE_UNKNOWN); // also sets the plist_view
}

void qm_player::on_new_song() {
  kbps_count_down = 0;
  b_skip_khz = false;

  if (current_songinfo->nosong) {
    if (b_nosong)
      return;
    // else
    b_nosong = true;
    current_type = TP_NOSONG;
    current_songnr = -1;
    browser_window->plist_view->set_newsong(-1);
    return;
  } else {
    browser_window->plist_view->set_newsong(current_songinfo->songNr);
    current_type = current_songinfo->type;
  }

  // in "idle" state we didn't set the title etc.
  if (current_status == MPD_STATE_UNKNOWN) {
    b_need_newSong = true;
    return;
  } else
    b_need_newSong = false;

  if (current_songnr != current_songinfo->songNr) {
    current_songnr = current_songinfo->songNr;
    printf("New song : %i\n", current_songnr);
  }
  b_nosong = false;

  QString fname = "";
  QString path = "";

  if (!current_songinfo->file.isEmpty()) {
    fname = current_songinfo->file;
    // current dir, from MPD's music dir, exc the last "/"
    current_mpd_dir = fname.left(fname.lastIndexOf("/"));
    // path, from MPD's music dir, inc the last "/"
    path = fname.left(fname.lastIndexOf("/") + 1);
    // remove path, keep filename only
    int i = fname.lastIndexOf('/') + 1;
    fname = fname.right(fname.length() - i);
  }

  if (current_songinfo->type == TP_SONG)
    a_loadtags->setEnabled(true);
  else
    a_loadtags->setEnabled(false);

  // streams : path.startsWith("http:")
  if (current_songinfo->type == TP_STREAM) {
    current_mpd_dir = "";
    set_albumart(path, TP_STREAM);
    ds_progress->setValue(0);
    lb_extension->setText("URL");
    lb_album->setText(tr(""));
    lb_comment->setText(tr("Audio stream"));
    // note: newSong->artist is NULL !

    if (!current_songinfo->title.isEmpty()) {
      title_scroller->set_title(current_songinfo->title);
      if (b_use_trayicon)
        the_trayicon->set_tip(current_songinfo->title);
    } else {
      if (!current_songinfo->name.isEmpty()) {
        title_scroller->set_title(current_songinfo->name);
        if (b_use_trayicon)
          the_trayicon->set_tip(current_songinfo->name);
      } else {
        title_scroller->set_title(tr("no info"));
        if (b_use_trayicon)
          the_trayicon->set_tip(tr("no info"));
      }
    }

    b_stream = true;
    return;
  } else
    b_stream = false;

  // albumart
  if ((config->mpd_musicpath_status == 1 ||
       current_songinfo->type == TP_SONGX) &&
      !config->disable_albumart)
    set_albumart(path, current_songinfo->type);
  else
    set_albumart("nopic", TP_NOSONG);

  // extension
  if (fname.isEmpty()) {
    lb_extension->setText("");
  } else {
    QString aString = fname;
    int i = aString.lastIndexOf('.') + 1;
    aString = aString.right(aString.length() - i);
    aString = aString.toUpper();
    lb_extension->setText(aString);
  }

  if (current_songinfo->time) {
    song_total_time = current_songinfo->time;
    ds_progress->setRange(0, song_total_time);
  }

  // artist
  QString artist = "?";
  if (!current_songinfo->artist.isEmpty())
    artist = current_songinfo->artist;

  // title
  QString title = "?";
  if (!current_songinfo->title.isEmpty())
    title = current_songinfo->title;

  if (current_songinfo->artist.isEmpty() && current_songinfo->title.isEmpty()) {
    if (fname.isEmpty()) {
      title_scroller->set_title("[?]");
      if (b_use_trayicon)
        the_trayicon->set_tip(" " + tr("No Info") + " ");
    } else {
      title_scroller->set_title(fname);
      if (b_use_trayicon)
        the_trayicon->set_tip(" " + fname + " ");
    }
  } else {
    title_scroller->set_title(artist, title);
    if (b_use_trayicon) {
      QString artist_tip = "&nbsp;<b>" + artist + "</b>&nbsp;";
      QString title_tip = "&nbsp;<i>" + title + "&nbsp;</i>";
      the_trayicon->set_tip(artist_tip + "<br>" + title_tip);
    }
  }

  // album
  QString album = "<b>?</b>";
  if (!current_songinfo->album.isEmpty())
    album = "<b>" + current_songinfo->album + "</b>";
  // year
  if (!current_songinfo->date.isEmpty())
    album.append("<br>" + current_songinfo->date);

  lb_album->setText(album);
  // comment
  lb_comment->setFixedSize(
      122, 200 - (lb_album->height() + 4)); // 4 = 2 x lb_album margin
  lb_comment->setText(current_songinfo->comment);
}

void qm_player::on_new_status(mpd_status *sInfo) {
  if (sInfo == nullptr)
    return;

  // set the playback modes in the playlist menu
  if (mpd_status_get_repeat(sInfo) != b_repeat) {
    browser_window->set_menu_repeat(mpd_status_get_repeat(sInfo));
    b_repeat = mpd_status_get_repeat(sInfo);
  }

  if (mpd_status_get_random(sInfo) != b_random) {
    browser_window->set_menu_random(mpd_status_get_random(sInfo));
    b_random = mpd_status_get_random(sInfo);
  }

  if (mpd_status_get_single(sInfo) != b_single) {
    browser_window->set_menu_single(mpd_status_get_single(sInfo));
    b_single = mpd_status_get_single(sInfo);
  }

  if (mpd_status_get_consume(sInfo) != b_consume) {
    browser_window->set_menu_consume(mpd_status_get_consume(sInfo));
    b_consume = mpd_status_get_consume(sInfo);
  }

  if (mpd_status_get_crossfade(sInfo) > 0 && !b_xfade) {
    b_xfade = true;
    browser_window->set_menu_xfade(b_xfade);
  }

  if (mpd_status_get_crossfade(sInfo) == 0 && b_xfade) {
    b_xfade = false;
    browser_window->set_menu_xfade(b_xfade);
  }

  // if status has changed...
  if (b_nosong && current_status != MPD_STATE_UNKNOWN)
    set_status(MPD_STATE_UNKNOWN);
  else if (mpd_status_get_state(sInfo) != current_status)
    set_status(mpd_status_get_state(sInfo));

  // set the other status info
  if (mpd_status_get_state(sInfo) != MPD_STATE_UNKNOWN &&
      current_status != MPD_STATE_UNKNOWN) {
    int timenow = static_cast<int>(mpd_status_get_elapsed_time(sInfo));

    if (!b_stream && !b_lock_progress && song_total_time > 0) {
      if (timenow != song_previous_time) {
        song_previous_time = timenow;
        // some tracks play a bit too long (honestly)
        if (timenow > song_total_time)
          timenow = song_total_time;
        ds_progress->setValue(timenow);
      }
    }

    if (b_stream || song_total_time == 0) {
      lb_time->setText("[&#187;]");
    } else {
      if (config->use_timeleft)
        timenow = song_total_time - timenow;

      QString nowtime = "";
      if (config->use_timeleft)
        nowtime += "-";
      nowtime += into_time(static_cast<int>(timenow));
      lb_time->setText(nowtime + " [" +
                       into_time(static_cast<int>(song_total_time)) + "]");
    }

    if (kbps_count_down == 0) {
      kbps_count_down = 4;

      uint kbps = mpd_status_get_kbit_rate(sInfo);
      if (kbps) // 0 means unknown. Never NULL
        lb_kbps->setText(QString::number(kbps) + " kb/s ");
      else
        lb_kbps->setText("");

      if (!b_skip_khz) {
        b_skip_khz = true;
        if (mpd_status_get_audio_format(sInfo) !=
            nullptr) // 0 means unknown. Can return NULL, esp. with streams!
        {
          double dbl_khz =
              static_cast<double>(
                  mpd_status_get_audio_format(sInfo)->sample_rate) /
              1000;
          lb_kHz->setText(QString::number(dbl_khz) + " kHz");
          b_skip_khz = true;
        } else
          lb_kHz->setText("");
      } else
        b_skip_khz = false;
    } else
      kbps_count_down--;
  }

  if (current_volume != mpd_status_get_volume(sInfo)) {
    b_mpd_sets_volume = true;
    /* slider Scale is set 0 to 100
        MPD also reports 0 to 100 */
    current_volume = mpd_status_get_volume(sInfo);
    vol_slider->setValue(current_volume);
    if (config->show_tooltips)
      vol_slider->setToolTip(tr("Volume") + " " +
                             QString::number(current_volume) + " %");
    b_mpd_sets_volume = false;
  }
}

void qm_player::retranslateUI() {
  if (config->show_tooltips) {
    lb_time->setToolTip(tr("Click to toggle time mode"));
    bt_sizer->setToolTip(tr("Mini-Maxi mode"));
    bt_browser->setToolTip(tr("Media Browser & Playlist"));
    bt_setts->setToolTip(tr("Settings"));
  }
}

// Signal handler.
void qm_player::on_signal(int sigID) {
  switch (sigID) {
  case ID_prev: {
    if (current_status != MPD_STATE_PLAY || !b_mpd_connected)
      break;
    mpdCom->prev();
    break;
  }

  case ID_stop: {
    kbps_count_down = 0;
    b_skip_khz = false;

    if (current_status == MPD_STATE_UNKNOWN ||
        current_status == MPD_STATE_STOP || !b_mpd_connected)
      break;

    set_status(MPD_STATE_STOP, "");
    title_scroller->pause(true);
    mpdCom->stop();
    break;
  }

  case ID_play: {
    kbps_count_down = 0;
    b_skip_khz = false;

    if (!b_mpd_connected)
      break;

    switch (current_status) {
    case MPD_STATE_PLAY: {
      set_status(MPD_STATE_PAUSE, "");
      mpdCom->pause();
      break;
    }

    case MPD_STATE_PAUSE: {
      set_status(MPD_STATE_PLAY, "");
      mpdCom->resume();
      break;
    }

    case MPD_STATE_STOP: {
      set_status(MPD_STATE_PLAY, "");
      mpdCom->play();
      break;
    }

    case MPD_STATE_UNKNOWN: {
      set_status(MPD_STATE_PLAY, "");
      mpdCom->play();
      break;
    }

    default:
      break;
    }
    break;
  }

  case ID_next: {
    if (current_status != MPD_STATE_PLAY || !b_mpd_connected)
      break;
    mpdCom->next();
    break;
  }

  case ID_sets: {
    if (settings_window->isVisible())
      settings_window->hide();
    else {
      settings_window->set_config(config); // reset unsaved changes
      settings_window->show();
      settings_window->setWindowState(Qt::WindowNoState);
    }
    break;
  }

  case ID_plst: {
    if (browser_window->isVisible())
      browser_window->hide();
    else {
      browser_window->show();
      browser_window->setWindowState(Qt::WindowNoState);
    }
    break;
  }

  case ID_mmax: {
    if (b_minmax_busy)
      break;

    if (config->player_maxmode) {
      img_xfade_input = pxb_albmart.toImage();
      b_minmax_xfade = true;
      b_minmax_busy = true;
      alphacycler = 255;
      minimizer->start(12);
      config->player_maxmode = false;
    } else {
      img_xfade_input = pxb_albmart.toImage();
      b_minmax_xfade = false;
      b_minmax_busy = true;
      alphacycler = 0;
      maximizer->start(12);
      config->player_maxmode = true;
    }
    break;
  }

  case ID_time: {
    config->use_timeleft = !config->use_timeleft;
    break;
  }

  case ID_exit: {
    b_really_close = true;
    close();
    break;
  }

  default:
    break;
  }
  // focus goes back to the volume slider
  vol_slider->setFocus(Qt::OtherFocusReason);
}

void qm_player::set_status(int status, QString message) {
  if (!message.isEmpty())
    title_scroller->set_title(message);

  if (status == current_status)
    return;

  // in "idle" mode only respond to play and not connected
  if (current_status == MPD_STATE_UNKNOWN && status != MPD_STATE_PLAY &&
      status != -1)
    return;

  if (b_need_newSong)
    mpdCom->reset_current_song();

  browser_window->plist_view->set_status(status);

  switch (status) {
  case -1: // not connected
  {
    bt_playpause->setIcon(img_mn_play);
    lb_time->setText("");
    lb_kHz->setText("");
    lb_extension->setText("");
    lb_kbps->setText("");
    lb_album->setText("");
    lb_comment->setText("");
    set_albumart("nopic", TP_NOSONG);
    ds_progress->setStyleSheet("QSlider::groove:horizontal {"
                               "margin: 0 0 0 0;"
                               "border: 1px solid #aaa;"
                               "background-image: url(:/groove_bg_noconn.png);"
                               "height: 3px;"
                               "border-radius: 2px; }");
    ds_progress->setValue(0);
    if (b_use_trayicon) {
      the_trayicon->set_mode(ID_nocn);
    }
    break;
  }

  case MPD_STATE_STOP: // 1
  {
    bt_playpause->setIcon(img_mn_play);
    title_scroller->pause(true);
    ds_progress->setStyleSheet("QSlider::groove:horizontal {"
                               "margin: 0 0 0 0;"
                               "border: 1px solid #aaa;"
                               "background-image: url(:/groove_bg_conn.png);"
                               "height: 3px;"
                               "border-radius: 2px; }");
    ds_progress->setValue(0);
    if (b_use_trayicon)
      the_trayicon->set_mode(ID_stop);
    break;
  }

  case MPD_STATE_PLAY: // 2
  {
    bt_playpause->setIcon(img_mn_pause);
    title_scroller->pause(false);
    ds_progress->setStyleSheet("QSlider::groove:horizontal {"
                               "margin: 0 0 0 0;"
                               "border: 1px solid #aaa;"
                               "background-image: url(:/groove_bg_conn.png);"
                               "height: 3px;"
                               "border-radius: 2px;}"
                               "QSlider::sub-page:horizontal {"
                               "border-image: url(:/groove_sub_play.png); }");
    if (b_use_trayicon)
      the_trayicon->set_mode(ID_play);
    break;
  }

  case MPD_STATE_PAUSE: // 3
  {
    bt_playpause->setIcon(img_mn_play);
    title_scroller->pause(true);
    ds_progress->setStyleSheet("QSlider::groove:horizontal {"
                               "margin: 0 0 0 0;"
                               "border: 1px solid #aaa;"
                               "background-image: url(:/groove_bg_conn.png);"
                               "height: 3px;"
                               "border-radius: 2px;}"
                               "QSlider::sub-page:horizontal {"
                               "border-image: url(:/groove_sub_play.png); }");
    if (b_use_trayicon)
      the_trayicon->set_mode(ID_paus);
    break;
  }

  default: // 0 (MPD_STATE_UNKNOWN) "Idle"
  {
    title_scroller->set_title(" ");
    bt_playpause->setIcon(img_mn_play);
    lb_time->setText("");
    lb_kHz->setText("");
    lb_extension->setText("");
    lb_kbps->setText("");
    lb_album->setText("");
    lb_comment->setText("");
    set_albumart("nopic", TP_NOSONG);
    ds_progress->setStyleSheet("QSlider::groove:horizontal {"
                               "margin: 0 0 0 0;"
                               "border: 1px solid #aaa;"
                               "background-image: url(:/groove_bg_conn.png);"
                               "height: 3px;"
                               "border-radius: 2px; }");
    ds_progress->setValue(0);
    if (b_use_trayicon) {
      the_trayicon->set_mode(ID_idle);
    }
    break;
  }
  }
  current_status = status;
}

QString qm_player::into_time(int time) {
  QString formattedTime = "";
  int mins = time / 60;
  formattedTime += QString::number((mins)) + ":";
  int secs = time % 60;
  if (secs < 10)
    formattedTime += "0";
  formattedTime += QString::number((secs));
  return formattedTime;
}

void qm_player::set_colors() {
  QPalette pal_main(this->palette());
  QPalette pal_album(this->palette());
  QPalette pal_default(this->palette());

  if (config->rb_qmcool_checked) {
    // display
    QColor fg_color = QColor("#e4f3fc");
    QColor bg_color = QColor("#505f72");
    pal_main.setColor(QPalette::Window, QColor(bg_color));
    pal_main.setColor(QPalette::Foreground, QColor(fg_color));
    // siblings are transparent by default
    main_display->setPalette(pal_main);
    main_display->setAutoFillBackground(true);
    lb_kbps->setPalette(pal_main);
    lb_kbps->setAutoFillBackground(true);
    lb_time->setPalette(pal_main);
    lb_time->setAutoFillBackground(true);
    title_scroller->setPalette(pal_main);
    title_scroller->setAutoFillBackground(true);
    lb_kHz->setPalette(pal_main);
    lb_kHz->setAutoFillBackground(true);
    lb_extension->setPalette(pal_main);
    lb_extension->setAutoFillBackground(true);
    // browser titles
    if (config->color_browser)
      browser_window->set_colors(pal_main);
    else
      browser_window->set_colors(pal_default);
    // album info
    if (config->color_albuminfo) {
      fg_color.setHsv(fg_color.hue(), fg_color.saturation(), 64);
      bg_color.setHsv(bg_color.hue(), bg_color.saturation() / 3, 200);
      pal_album.setColor(QPalette::Window, bg_color);
      pal_album.setColor(QPalette::Foreground, fg_color);
      center_widget->setAutoFillBackground(true);
      center_widget->setPalette(pal_album);
      // to fill transparency in (album art) images:
      cR = bg_color.red();
      cG = bg_color.green();
      cB = bg_color.blue();
    }
  } else if (config->rb_custom_checked) {
    // display
    QColor fg_color = QColor(config->color_fg);
    QColor bg_color = QColor(config->color_bg);
    pal_main.setColor(QPalette::Window, QColor(bg_color));
    pal_main.setColor(QPalette::Foreground, QColor(fg_color));
    // siblings are transparent by default
    main_display->setPalette(pal_main);
    main_display->setAutoFillBackground(true);
    lb_kbps->setPalette(pal_main);
    lb_kbps->setAutoFillBackground(true);
    lb_time->setPalette(pal_main);
    lb_time->setAutoFillBackground(true);
    title_scroller->setPalette(pal_main);
    title_scroller->setAutoFillBackground(true);
    lb_kHz->setPalette(pal_main);
    lb_kHz->setAutoFillBackground(true);
    lb_extension->setPalette(pal_main);
    lb_extension->setAutoFillBackground(true);
    // browser titles
    if (config->color_browser)
      browser_window->set_colors(pal_main);
    else
      browser_window->set_colors(pal_default);
    // album info
    if (config->color_albuminfo) {
      fg_color.setHsv(fg_color.hue(), fg_color.saturation(), 64);
      bg_color.setHsv(bg_color.hue(), bg_color.saturation() / 3, 200);
      pal_album.setColor(QPalette::Window, bg_color);
      pal_album.setColor(QPalette::Foreground, fg_color);
      center_widget->setAutoFillBackground(true);
      center_widget->setPalette(pal_album);
      // to fill transparency in (album art) images:
      cR = bg_color.red();
      cG = bg_color.green();
      cB = bg_color.blue();
    }
  } else // config->rb_sysdef_checked
  {
    // display
    main_display->setPalette(pal_default);
    main_display->setAutoFillBackground(false);
    lb_kbps->setPalette(pal_default);
    lb_kbps->setAutoFillBackground(false);
    lb_time->setPalette(pal_default);
    lb_time->setAutoFillBackground(false);
    title_scroller->setPalette(pal_default);
    title_scroller->setAutoFillBackground(false);
    lb_kHz->setPalette(pal_default);
    lb_kHz->setAutoFillBackground(false);
    lb_extension->setPalette(pal_default);
    lb_extension->setAutoFillBackground(false);
    center_widget->setAutoFillBackground(false);
    // browser titles
    browser_window->set_colors(pal_default);
    // album info
    if (config->color_albuminfo) {
      center_widget->setAutoFillBackground(false);
      center_widget->setPalette(pal_default);
      // to fill transparency in (album art) images:
      QColor bg_color = pal_default.color(QPalette::Window);
      cR = bg_color.red();
      cG = bg_color.green();
      cB = bg_color.blue();
    }
  }

  if (!config->color_albuminfo) {
    QColor bg_color = pal_default.color(QPalette::Window);
    cR = bg_color.red();
    cG = bg_color.green();
    cB = bg_color.blue();
    center_widget->setAutoFillBackground(false);
    center_widget->setPalette(pal_default);
  }

  set_albumart(current_art_path, current_type);
}

//  Mouse pressed on progress slider
void qm_player::lock_progress(QMouseEvent *e) {
  if (!b_stream && current_status == MPD_STATE_PLAY && b_mpd_connected) {
    b_lock_progress = true;
    // Get mouse x pos within widget
    int x = e->x();
    x = (x * song_total_time) / 324; // 324 is width of widget
    ds_progress->setValue(x);
  }
}

// Mouse released on progress slider.
void qm_player::unlock_progress() {
  if (!b_stream && current_status == MPD_STATE_PLAY && b_mpd_connected) {
    mpdCom->set_seek(static_cast<uint>(ds_progress->value()));
    b_lock_progress = false;
  }
}

void qm_player::set_fonts() {
  QFont font;

  font.setPointSize(config->maintitle_fontsize);
  title_scroller->setFont(font);

  font.setPointSize(config->codecinfo_fontsize);
  lb_extension->setFont(font);
  lb_kHz->setFont(font);
  lb_kbps->setFont(font);

  QLabel testlabel;
  testlabel.setFont(font);
  testlabel.setText("1024 kb/s");
  int w_kbps = (testlabel.minimumSizeHint()).width();

  bool ok_to_show_khz = true;
  // hide kHz label when kb/s needs the space
  if (w_kbps > 64) {
    lb_kHz->hide();
    ok_to_show_khz = false;
    lb_kbps->setGeometry(66, 28, w_kbps, 20);
  } else {
    lb_kHz->show();
    lb_kbps->setGeometry(66, 28, 64, 20);
  }

  font.setPointSize(config->time_fontsize);
  lb_time->setFont(font);

  testlabel.setFont(font);
  testlabel.setText("-00:00 [00:00]");
  int w_time = (testlabel.minimumSizeHint()).width();

  // hide kHz label when time needs the space
  if (w_time > 114) {
    lb_kHz->hide();
    lb_time->setGeometry(QRect(200 - (w_time - 114), 28, w_time, 20));
  } else {
    if (ok_to_show_khz)
      lb_kHz->show();
    lb_time->setGeometry(QRect(200, 28, 114, 20));
  }

  font.setPointSize(config->album_fontsize);
  lb_album->setFont(font);

  font.setPointSize(config->comment_fontsize);
  lb_comment->setFont(font);

  font.setPointSize(config->liblist_fontsize);
  browser_window->plist_view->setFont(font);
  browser_window->lib_view->setFont(font);
}

void qm_player::set_albumart(QString sub_path, int type) {
  QImage temp;
  bool b_file_from_disk_or_tag = true;
  bool b_file_ok = false;

  if (type == TP_NOSONG || sub_path == "nopic" || sub_path.isEmpty()) {
    img_nopic.load(":/nopic");
    { // fill transparency with background color (for fading)
      for (int j = 0; j < 200; j++) {
        QRgb *px = (reinterpret_cast<QRgb *>(img_nopic.scanLine(j)));
        for (int k = 0; k < 200; k++) {
          int a = qAlpha(*px);
          uchar r = uchar((int(qRed(*px)) * a + cR * (255 - a)) / 255);
          uchar g = uchar((int(qGreen(*px)) * a + cG * (255 - a)) / 255);
          uchar b = uchar((int(qBlue(*px)) * a + cB * (255 - a)) / 255);
          *px = qRgba(r, g, b, 255);
          px++;
        }
      }
    }
    temp = QImage(img_nopic);
    current_art_path = "nopic";
    b_file_from_disk_or_tag = false;
    b_file_ok = true;
  }

  if (type == TP_STREAM || sub_path == "onair") {
    img_stream.load(":/onair");
    { // fill transparency with background color (for fading)
      for (int j = 0; j < 200; j++) {
        QRgb *px = (reinterpret_cast<QRgb *>(img_stream.scanLine(j)));
        for (int k = 0; k < 200; k++) {
          int a = qAlpha(*px);
          uchar r = uchar((int(qRed(*px)) * a + cR * (255 - a)) / 255);
          uchar g = uchar((int(qGreen(*px)) * a + cG * (255 - a)) / 255);
          uchar b = uchar((int(qBlue(*px)) * a + cB * (255 - a)) / 255);
          *px = qRgba(r, g, b, 255);
          px++;
        }
      }
    }
    temp = QImage(img_stream);
    current_art_path = "onair";
    b_file_from_disk_or_tag = false;
    b_file_ok = true;
  }

  // ALBUM-ART:

  QString artPath;
  if (type == TP_SONGX) {
    if (sub_path.startsWith("/"))
      artPath = sub_path;
    else
      artPath = "/" + sub_path;
  } else // type == TP_SONG
    artPath = config->mpd_musicpath + sub_path;

  // first try tag
  bool b_image_from_tag = false;
  if (b_file_from_disk_or_tag) {
    QString filepath = current_songinfo->file;
    int i = filepath.lastIndexOf('/');
    filepath = filepath.right(filepath.length() - i);
    filepath = artPath + filepath;

    // try to load the image
    temp = getTagImage(filepath);
    if (temp.isNull()) {
      b_file_ok = false;
    } else {
      b_file_ok = true;
      b_image_from_tag = true;
      current_art_path = sub_path;
    }
  }

  // next try image file if tag failed
  if (b_file_from_disk_or_tag && !b_file_ok) {
    QDir dir(artPath);
    if (!dir.exists())
      printf("Album art directory : not found\n");
    else if (!dir.isReadable())
      printf("Album art directory : not readable\n");
    else {
      b_file_ok = go_find_art(artPath, "*albumart*");
      if (!b_file_ok)
        b_file_ok = go_find_art(artPath, "*folder*");
      if (!b_file_ok)
        b_file_ok = go_find_art(artPath, "*front*");
      if (!b_file_ok)
        b_file_ok = go_find_art(artPath, "*cover*");
      if (!b_file_ok)
        b_file_ok = go_find_art(artPath, "*"); // anything else?

      if (!b_file_ok)
        artPath = "nopic";

      current_art_path = sub_path;

      // load the image
      if (b_file_ok) {
        temp.load(artPath);

        if (temp.isNull()) {
          b_file_ok = false;
          printf("Album art : error loading file\n");
        }
      }
    }
  }

  if (!b_file_ok) {
    current_art_path = "nopic";
    temp = QImage(img_nopic);
    b_file_from_disk_or_tag = false;
  }

  // resize the image
  if (b_file_from_disk_or_tag)
    temp = temp.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  albumart_H = temp.height();
  albumart_W = temp.width();

  if (b_file_from_disk_or_tag) {
    //  add alpha for xfading
    if (!temp.hasAlphaChannel()) {
      QImage aChannel = QImage(albumart_W, albumart_H, QImage::Format_RGB32);
      aChannel.fill(0xFFFFFFFF);
      temp.setAlphaChannel(aChannel);
    } else { //  fill transparency with background color (for fading)
      for (int j = 0; j < albumart_H; j++) {
        QRgb *px = (reinterpret_cast<QRgb *>(temp.scanLine(j)));
        for (int k = 0; k < albumart_W; k++) {
          int a = qAlpha(*px);
          uchar r = uchar((int(qRed(*px)) * a + cR * (255 - a)) / 255);
          uchar g = uchar((int(qGreen(*px)) * a + cG * (255 - a)) / 255);
          uchar b = uchar((int(qBlue(*px)) * a + cB * (255 - a)) / 255);
          *px = qRgba(r, g, b, 255);
          px++;
        }
      }
    }
  }

  // add border
  if (b_file_from_disk_or_tag) {
    for (int h = 0; h < albumart_H; h++) {
      for (int w = 0; w < albumart_W; w++) {
        if (h == 0 || h == albumart_H - 1 || w == 0 || w == albumart_W - 1) {
          temp.setPixel(w, h, 0xFF768B96);
        }
      }
    }
  }

  // finally show the image
  if (config->player_maxmode) {
    pxb_albmart = QPixmap::fromImage(temp);
    pxb_xfading = QPixmap::fromImage(temp);
    lb_albumart->setPixmap(pxb_albmart);
  } else {
    pxb_albmart = QPixmap::fromImage(temp);
    // set alpha to 0
    for (int h = 0; h < albumart_H; h++) {
      for (int w = 0; w < albumart_W; w++) {
        QRgb RGB = temp.pixel(w, h);
        temp.setPixel(w, h, qRgba(qRed(RGB), qGreen(RGB), qBlue(RGB), 0));
      }
    }
    pxb_xfading = QPixmap::fromImage(temp);
    lb_albumart->setPixmap(pxb_xfading);
  }

  // and set tooltip
  if (config->show_tooltips && b_file_ok) {
    if (b_image_from_tag)
      lb_albumart->setToolTip(tr("Album art (from tag)"));
    else
      lb_albumart->setToolTip(tr("Album art (from file)"));
  }
  if (config->show_tooltips && !b_file_ok)
    lb_albumart->setToolTip(tr("No album art"));
}

bool qm_player::go_find_art(QString &artPath, QString findthis) {
  QDir dir(artPath);
  dir.setSorting(QDir::Type);
  QStringList namefilter;

  namefilter << findthis + ".jpg" << findthis + ".png" << findthis + ".gif"
             << findthis + ".bmp" << findthis + ".jpeg";

  QStringList files = dir.entryList(namefilter, QDir::Files);
  if (files.isEmpty())
    return false;

  artPath.append(files.at(0));
  return true;
}

void qm_player::on_contextmenu() {
  // no song, stream or not connected -> no menu
  if (b_nosong || b_stream || !b_mpd_connected)
    return;
  context_menu->exec(QCursor::pos());
}

void qm_player::show_albumart() {
  QString full_path;
  if (current_type == TP_SONGX) {
    if (current_mpd_dir.startsWith("/"))
      full_path = current_mpd_dir;
    else
      full_path = "/" + current_mpd_dir;
  } else if (current_type == TP_SONG) {
    full_path = config->mpd_musicpath + current_mpd_dir;
  } else
    return;

  QProcess *proc = nullptr;
  proc->startDetached(config->art_viewer, QStringList() << full_path);
}

void qm_player::reload_tags() {
  if (current_type != TP_SONG)
    return;

  if (config->mpd_rescan_allowed) {
    qm_commandList cmdList;
    qm_mpd_command newCommand;

    newCommand.file = current_mpd_dir;
    newCommand.cmd = CMD_SCN;
    cmdList.push_back(newCommand);

    QString msg = "Rescanning : " + current_mpd_dir + "\n";
    printf("%s", msg.toUtf8().constData());
    mpdCom->execute_cmds(cmdList, false);
  } else
    printf("Rescan not allowed\n");
}

void qm_player::edit_tags() {
  QString full_path;
  if (current_type == TP_SONGX) {
    if (current_mpd_dir.startsWith("/"))
      full_path = current_mpd_dir;
    else
      full_path = "/" + current_mpd_dir;
  } else if (current_type == TP_SONG) {
    full_path = config->mpd_musicpath + current_mpd_dir;
  } else
    return;

  QProcess *proc = nullptr;
  proc->startDetached(config->tag_editor, QStringList() << full_path);
}

void qm_player::open_directory() {
  QString full_path;
  if (current_type == TP_SONGX) {
    if (current_mpd_dir.startsWith("/"))
      full_path = current_mpd_dir;
    else
      full_path = "/" + current_mpd_dir;
  } else if (current_type == TP_SONG) {
    full_path = config->mpd_musicpath + current_mpd_dir;
  } else
    return;

  QProcess *proc = nullptr;
  proc->startDetached(config->file_manager, QStringList() << full_path);
}

// called by the core
void qm_player::wakeup_call(bool show_browser) {

  this->showNormal();
  if (show_browser) {
    browser_window->showNormal();
    browser_window->activateWindow();
  } else
    this->activateWindow();
}

void qm_player::showEvent(QShowEvent *se) {
  if (b_reshow_browser)
    browser_window->showNormal();
  if (b_reshow_settings)
    settings_window->showNormal();
  se->accept();
}

void qm_player::hideEvent(QHideEvent *he) {
  if (browser_window->isVisible()) {
    b_reshow_browser = true;
    browser_window->hide();
  } else
    b_reshow_browser = false;

  if (settings_window->isVisible()) {
    b_reshow_settings = true;
    settings_window->hide();
  } else
    b_reshow_settings = false;

  config->player_X = x();
  config->player_Y = y();
  he->accept();
}

void qm_player::closeEvent(QCloseEvent *ce) {
  if (!b_really_close && b_use_trayicon) {
    this->hide();
  } else {
    if (this->isVisible()) {
      config->player_X = x();
      config->player_Y = y();
    }
    if (browser_window->isVisible()) {
      config->browser_X = browser_window->x();
      config->browser_Y = browser_window->y();
      config->browser_W = browser_window->width();
      config->browser_H = browser_window->height();
    }
    if (settings_window->isVisible()) {
      config->settings_X = settings_window->x();
      config->settings_Y = settings_window->y();
      config->settings_W = settings_window->width();
      config->settings_H = settings_window->height();
    }

    if (config->quitMPD_onquit) {
      printf("Closing MPD, as configured\n");
      QProcess *proc = nullptr;
      proc->startDetached(config->onquit_mpd_command);
    }

    config->save_config();
    printf("Thank you and goodbye!\n");
    qApp->quit();
  }

  ce->accept();
}

void qm_player::on_shudown() {
  b_really_close = true;
  close();
}

// connecting signal directly to keyPressEvent() does not work, so:
void qm_player::on_browser_keypress(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_MediaPause:
  case Qt::Key_MediaPlay:
  case Qt::Key_MediaNext:
  case Qt::Key_MediaPrevious:
  case Qt::Key_MediaStop: {
    qm_player::keyPressEvent(event);
    break;
  }
  default:
    break;
  }
}

void qm_player::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Q: {
    if (event->modifiers() & Qt::ControlModifier)
      on_shudown();
    break;
  }
  case Qt::Key_MediaPause: {
    printf("Key_MediaPause pressed\n");
    on_signal(ID_play);
    break;
  }
  case Qt::Key_MediaPlay: {
    printf("Key_MediaPlay pressed\n");
    on_signal(ID_play);
    break;
  }
  case Qt::Key_MediaNext: {
    printf("Key_MediaNext pressed\n");
    on_signal(ID_next);
    break;
  }
  case Qt::Key_MediaPrevious: {
    printf("Key_MediaPrevious pressed\n");
    on_signal(ID_prev);
    break;
  }
  case Qt::Key_MediaStop: {
    printf("Key_MediaStop pressed\n");
    on_signal(ID_stop);
    break;
  }
  default:
    QMainWindow::keyPressEvent(event);
  }
}

///////////////// D&D Stuff ///////////////////////////////
void qm_player::dragEnterEvent(QDragEnterEvent *e) {
  if (config->mpd_socket_conn && e->source() == nullptr &&
      e->mimeData()->hasUrls())
    e->accept();
  else
    e->ignore();
}

void qm_player::dragMoveEvent(QDragMoveEvent *e) {
  if (config->mpd_socket_conn && e->source() == nullptr &&
      e->mimeData()->hasUrls())
    e->accept();
  else
    e->ignore();
}

void qm_player::dragLeaveEvent(QDragLeaveEvent *e) { e->accept(); }

void qm_player::dropEvent(QDropEvent *e) {
  if (config->mpd_socket_conn && e->source() == nullptr &&
      e->mimeData()->hasUrls())
    browser_window->plist_view->on_open_with_droprequest(e);

  e->ignore();
}
///////////////// end D&D Stuff ///////////////////////////

// get coverart from tag
QImage qm_player::getTagImage(QString qs_file) {

  QImage image = QImage();

  // check if file (still) exists
  QFile chkfile(qs_file);
  if (!chkfile.exists()) {
    printf("image from TAG: file not found\n");
    return image;
  }

  // QByteArray byteArray = qs_file.toUtf8();
  // const char* file = byteArray.constData();

  const char *file = qs_file.toUtf8().constData();

  if (qs_file.endsWith(".mp3", Qt::CaseInsensitive)) {
    TagLib::MPEG::File mpgfile(file);
    if (mpgfile.isValid()) {
      if (mpgfile.ID3v2Tag()) // has ID3v2 tag
      {
        TagLib::ID3v2::FrameList flist = mpgfile.ID3v2Tag()->frameList("APIC");

        if (!flist.isEmpty()) {
          TagLib::ID3v2::AttachedPictureFrame *pframe =
              static_cast<TagLib::ID3v2::AttachedPictureFrame *>(flist.front());

          if (image.loadFromData(
                  reinterpret_cast<const uchar *>(pframe->picture().data()),
                  static_cast<int>(pframe->picture().size()))) {
            // printf ("Cover art from mp3 tag\n");
            return image;
          }
        }
      } else if (mpgfile.APETag()) {
        TagLib::APE::Tag *tag =
            static_cast<TagLib::APE::Tag *>(mpgfile.APETag());
        const TagLib::APE::ItemListMap &listMap = tag->itemListMap();

        if (listMap.contains("COVER ART (FRONT)")) {
          const TagLib::ByteVector nullStringTerminator(1, 0);

          TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
          int pos = item.find(nullStringTerminator); // Skip the filename

          if (++pos > 0) {
            const TagLib::ByteVector &picvect =
                item.mid(static_cast<uint>(pos));
            QByteArray image_data(picvect.data(),
                                  static_cast<int>(picvect.size()));
            if (image.loadFromData(image_data)) {
              // printf ("Cover art from ape tag\n");
              return image;
            }
          }
        }
      }
    }
  } else if (qs_file.endsWith(".ogg", Qt::CaseInsensitive) ||
             qs_file.endsWith(".oga", Qt::CaseInsensitive)) {
    TagLib::Ogg::Vorbis::File oggfile(file);
    if (oggfile.isValid()) {
      if (oggfile.tag()) // has XiphComment tag
      {
        TagLib::Ogg::XiphComment *m_tag =
            static_cast<TagLib::Ogg::XiphComment *>(oggfile.tag());
/////// First we try the flac-type picturelist
#if taglib_piclist
        TagLib::List<TagLib::FLAC::Picture *> picList = m_tag->pictureList();

        if (!picList.isEmpty()) {
          TagLib::FLAC::Picture *fpic = picList[0];

          QByteArray image_data(fpic->data().data(),
                                static_cast<int>(fpic->data().size()));

          if (image.loadFromData(image_data)) {
            // printf ("Cover art from ogg-flac tag\n");
            return image;
          }
        }
#endif

        /////// If the above fails we try the fieldListMap

        TagLib::StringList block;

        const TagLib::Ogg::FieldListMap &flMap = m_tag->fieldListMap();

        if (flMap.contains("METADATA_BLOCK_PICTURE")) {
          block = m_tag->fieldListMap()["METADATA_BLOCK_PICTURE"];
        } else if (flMap.contains("COVERART")) {
          block = m_tag->fieldListMap()["COVERART"];
        }

        if (!block.isEmpty()) {

          for (TagLib::StringList::ConstIterator i = block.begin();
               i != block.end(); ++i) {
            QByteArray data(QByteArray::fromBase64(i->to8Bit().c_str()));
            TagLib::ByteVector tdata(data.data(),
                                     static_cast<uint>(data.size()));

            TagLib::FLAC::Picture fpic;

            if (!fpic.parse(tdata)) {
              continue;
            }

            QByteArray image_data(fpic.data().data(),
                                  static_cast<int>(fpic.data().size()));

            if (image.loadFromData(image_data)) {
              // printf ("Cover art from ogg tag\n");
              return image;
            }
          }
        }
      }
    }
  } else if (qs_file.endsWith(".flac", Qt::CaseInsensitive)) {
    TagLib::FLAC::File flacfile(file);
    if (flacfile.isValid()) {
      if (flacfile.ID3v2Tag()) // has ID3v2 tag
      {
        TagLib::ID3v2::FrameList flist = flacfile.ID3v2Tag()->frameList("APIC");

        if (!flist.isEmpty()) {
          TagLib::ID3v2::AttachedPictureFrame *pframe =
              static_cast<TagLib::ID3v2::AttachedPictureFrame *>(flist.front());

          if (image.loadFromData(
                  reinterpret_cast<const uchar *>(pframe->picture().data()),
                  static_cast<int>(pframe->picture().size()))) {
            // printf ("Cover art from flac (ID3v2) tag\n");
            return image;
          }
        }
      }

#if taglib_piclist
      if (flacfile.tag()) // has flac tag
      {
        const TagLib::List<TagLib::FLAC::Picture *> &picList =
            flacfile.pictureList();

        if (!picList.isEmpty()) {
          TagLib::FLAC::Picture *fpic = picList[0];

          QByteArray image_data(fpic->data().data(),
                                static_cast<int>(fpic->data().size()));

          if (image.loadFromData(image_data)) {
            // printf ("Cover art from flac tag\n");
            return image;
          }
        }
      }
#endif
    }
  } else if (qs_file.endsWith(".mp4", Qt::CaseInsensitive) ||
             qs_file.endsWith(".m4a", Qt::CaseInsensitive) ||
             qs_file.endsWith(".aac", Qt::CaseInsensitive)) {
    TagLib::MP4::File mp4file(file);
    if (mp4file.isValid()) {
      if (mp4file.tag()) // has MP4 tag
      {
        TagLib::MP4::Tag *tag = static_cast<TagLib::MP4::Tag *>(mp4file.tag());
        TagLib::MP4::ItemListMap itemsListMap = tag->itemListMap();
        if (tag->itemListMap().contains("covr")) {
          TagLib::MP4::Item coverItem = itemsListMap["covr"];
          TagLib::MP4::CoverArtList coverArtList = coverItem.toCoverArtList();
          TagLib::MP4::CoverArt coverArt = coverArtList.front();
          if (image.loadFromData(
                  reinterpret_cast<const uchar *>(coverArt.data().data()),
                  static_cast<int>(coverArt.data().size()))) {
            // printf ("Cover art from mp4 tag\n");
            return image;
          }
        }
      }
    }
  } else if (qs_file.endsWith(".mpc", Qt::CaseInsensitive) ||
             qs_file.endsWith(".mpp", Qt::CaseInsensitive)) {
    TagLib::MPC::File mpcfile(file);
    if (mpcfile.isValid()) {
      if (mpcfile.APETag()) {
        TagLib::APE::Tag *tag =
            static_cast<TagLib::APE::Tag *>(mpcfile.APETag());
        const TagLib::APE::ItemListMap &listMap = tag->itemListMap();

        if (listMap.contains("COVER ART (FRONT)")) {
          const TagLib::ByteVector nullStringTerminator(1, 0);

          TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
          int pos = item.find(nullStringTerminator); // Skip the filename

          if (++pos > 0) {
            const TagLib::ByteVector &picvect =
                item.mid(static_cast<uint>(pos));
            QByteArray image_data(picvect.data(),
                                  static_cast<int>(picvect.size()));
            if (image.loadFromData(image_data)) {
              // printf ("Cover art from mpc (ape) tag\n");
              return image;
            }
          }
        }
      }
    }
  } else if (qs_file.endsWith(".wv", Qt::CaseInsensitive)) {
    TagLib::WavPack::File wvpfile(file);
    if (wvpfile.isValid()) {
      if (wvpfile.APETag()) {
        TagLib::APE::Tag *tag =
            static_cast<TagLib::APE::Tag *>(wvpfile.APETag());
        const TagLib::APE::ItemListMap &listMap = tag->itemListMap();

        if (listMap.contains("COVER ART (FRONT)")) {
          printf("APE tag  COVER ART found\n");
          const TagLib::ByteVector nullStringTerminator(1, 0);

          TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
          int pos = item.find(nullStringTerminator); // Skip the filename

          if (++pos > 0) {
            const TagLib::ByteVector &picvect =
                item.mid(static_cast<uint>(pos));
            QByteArray image_data(picvect.data(),
                                  static_cast<int>(picvect.size()));
            if (image.loadFromData(image_data)) {
              // printf ("Cover art from wv (ape) tag\n");
              return image;
            }
          }
        }
      }
    }
  } else if (qs_file.endsWith(".ape", Qt::CaseInsensitive)) {
    TagLib::WavPack::File apefile(file);
    if (apefile.isValid()) {
      if (apefile.APETag()) {
        TagLib::APE::Tag *tag =
            static_cast<TagLib::APE::Tag *>(apefile.APETag());
        const TagLib::APE::ItemListMap &listMap = tag->itemListMap();

        /* // DEBUG: List all tags
        for( TagLib::APE::ItemListMap::ConstIterator it = listMap.begin(); it !=
        listMap.end(); ++it )
        {
            QString value = TStringToQString( it->first );
            const char* entry = value.toUtf8().constData();
            printf ("Ape_tag: [%s]\n",entry);
        }  */

        if (listMap.contains("COVER ART (FRONT)")) {
          const TagLib::ByteVector nullStringTerminator(1, 0);

          TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
          int pos = item.find(nullStringTerminator); // Skip the filename

          if (++pos > 0) {
            const TagLib::ByteVector &picvect =
                item.mid(static_cast<uint>(pos));
            QByteArray image_data(picvect.data(),
                                  static_cast<int>(picvect.size()));
            if (image.loadFromData(image_data)) {
              // printf ("Cover art from ape tag\n");
              return image;
            }
          }
        }
      }
    }
  } else if (qs_file.endsWith(".asf", Qt::CaseInsensitive)) {
    TagLib::ASF::File asffile(file);
    if (asffile.isValid()) {
      if (asffile.tag()) // has asf tag
      {
        TagLib::ASF::Tag *tag = static_cast<TagLib::ASF::Tag *>(asffile.tag());
        const TagLib::ASF::AttributeListMap &attrListMap =
            tag->attributeListMap();
        if (attrListMap.contains("WM/Picture")) {
          const TagLib::ASF::AttributeList &attrList =
              attrListMap["WM/Picture"];

          if (!attrList.isEmpty()) {
            // grab the first cover.
            TagLib::ASF::Picture pic = attrList[0].toPicture();
            if (pic.isValid()) {
              const QByteArray image_data(
                  pic.picture().data(), static_cast<int>(pic.picture().size()));

              if (image.loadFromData(image_data)) {
                // printf ("Cover art from asf tag\n");
                return image;
              }
            }
          }
        }
      }
    }
  }

  return image;
}
/*
bool qm_player::eventFilter( QObject* ob, QEvent *event )
{
    printf ("DEBUG1 evenFilter systray\n"); // never happens

    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    int keyValue = ke->key();

    if(event->type() == QEvent::KeyPress )
    {
      printf ("KeyPressed: %i\n", keyValue);
      // Qt::Key_0  Qt::Key_1 e Qt::Key_2
      return true;
    }

    return QObject::eventFilter(ob, event);
}
*/
qm_player::~qm_player() {
  delete (mpdCom);
  delete (browser_window);
  delete (settings_window);
  delete (config);
  delete (the_trayicon);
  delete (ds_progress);
  delete (lb_time);
  delete (vol_slider);
}
