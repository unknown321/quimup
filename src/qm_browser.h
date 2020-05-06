/*
*  qm_browser.h
*  QUIMUP library & playlist window
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

#ifndef qm_browser_H
#define qm_browser_H

#include "qm_browser_ids.h"
#include "qm_commandlist.h"
#include "qm_config.h"
#include "qm_libraryview.h"
#include "qm_playlistview.h"
#include "qm_streamloader.h"
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QSpacerItem>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

class qm_browser : public QMainWindow {
  Q_OBJECT

protected:
  void keyPressEvent(QKeyEvent *);

public:
  qm_browser();
  virtual ~qm_browser();

  qm_plistview *plist_view;
  qm_plistview *plist_view2;
  qm_libview *lib_view;

public slots:
  void set_config(qm_config *);
  void set_connected(qm_mpdCom *, bool);
  void set_menu_random(bool);
  void set_menu_repeat(bool);
  void set_menu_single(bool);
  void set_menu_consume(bool);
  void set_menu_xfade(bool);
  void set_colors(QPalette);

private slots:
  void set_searchmode(int);
  void set_selectmode(int);
  void on_dbase_newmode(int);
  void search_in_lib();
  void select_in_plist();
  void on_update_dbase();
  void on_update_ready();
  void on_options_clicked();
  void set_plist_stats(QString);
  void set_dbase_stats(QString);
  void get_the_stream();
  void splittermoved();
  void on_streamdownload_done(QStringList);
  void on_playlist_toggle();
  void on_library_toggle();
  void set_random();
  void set_repeat();
  void set_single();
  void set_consume();
  void set_xfade();

private:
  QWidget *main_widget;
  QVBoxLayout *vbox_all;
  QSplitter *theSplitter;

  QLabel *left_group;
  QLabel *lb_left_title;
  QVBoxLayout *vbox_lefthalf;
  QHBoxLayout *hbox_leftop;
  QComboBox *cb_dbasemode;
  QSpacerItem *spacer1;
  QPushButton *bt_library;
  QSpacerItem *spacer2;
  QPushButton *bt_update;
  QLabel *lb_dbasestats;
  QHBoxLayout *hbox_leftbottom;
  QComboBox *cb_searchmode;
  QLineEdit *le_search;
  QPushButton *bt_search;

  QLabel *right_group;
  QLabel *lb_right_title;
  QVBoxLayout *vbox_righthalf;
  QHBoxLayout *hbox_rightop;
  QPushButton *bt_stream;
  QSpacerItem *spacer3;
  QPushButton *bt_playlist;
  QSpacerItem *spacer4;
  QPushButton *bt_options;
  QLabel *lb_pliststats;
  QHBoxLayout *hbox_rightbottom;
  QComboBox *cb_selectmode;
  QLineEdit *le_select;
  QPushButton *bt_select;

  QLabel *right_group2;
  QLabel *lb_right_title2;
  QVBoxLayout *vbox_righthalf2;
  QHBoxLayout *hbox_rightop2;
  QSpacerItem *spacer5;
  QPushButton *bt_playlist2;
  QSpacerItem *spacer6;
  QPushButton *bt_options2;
  QLabel *lb_pliststats2;
  QHBoxLayout *hbox_rightbottom2;
  QComboBox *cb_selectmode2;
  QLineEdit *le_select2;
  QPushButton *bt_select2;

  bool b_mpd_connected, b_startup;
  qm_config *config;
  qm_mpdCom *mpdCom;
  qm_stream_loader *sloader;
  QMenu *options_menu;
  QAction *a_random, *a_repeat, *a_single, *a_consume, *a_xfade;

  int splitter_left, splitter_right, db_mode_initial;
  void setupUI();
  void retranslateUI();

protected:
  void closeEvent(QCloseEvent *);
  void hideEvent(QHideEvent *);
  void showEvent(QShowEvent *);
  void resizeEvent(QResizeEvent *);

signals:
  void sgnl_keypressed(QKeyEvent *);
};

#endif // qm_browser_H
