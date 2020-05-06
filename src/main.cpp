/*
*  main.cpp
*  QUIMUP main program file
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

#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QTextCodec>
#include <QDir>
#include "qtsingleapplication.h"
#include "qm_core.h"


int main(int argc, char **argv)
{
    QtSingleApplication app("quimup", argc, argv);
    bool b_multiple_instances = false;
    QString locale = "xx";
    QString message;
    int filecount = 0;
    // handle command line args
    for (int a = 1; a < argc; ++a)
    {
        QString msg = QString::fromUtf8(argv[a]);

        if ( msg == "-p" || msg == "-play")
        {
            message = "-play:" + message;
        }
        else
        if ( msg == "-i" || msg == "-instance")
        {
            b_multiple_instances = true;
        }
        else
        if ( msg == "-l"  || msg == "-locale" )
        {
            if (a < (argc-1) )
            {
                locale = argv[a+1];
                // possible stray '-l'
                if ( locale.startsWith("-") || locale.startsWith("file") )
                    locale = "xx";
                else
                    a++;
            }
        }
        else
        if (msg == "-h" || msg == "-help")
        {
            printf ("----\nQuimup version 1.4.4\n"); // VERSION //
            printf ("© 2008-2018 Johan Spee <quimup@coonsden.com>\n");
            printf ("This program is licensed by the GPL and distributed in the hope that it will be useful, but without any warranty.\n");
            printf ("----\ncommand line parameters:\n");
            printf (" -h(elp)           show this information and exit\n");
            printf (" -i(nstance)       force a new instance of Quimup\n");
            printf (" -l(ocale) xx      use locale 'xx' (fr, po, en_GB, etc) or 'none' to force internal locale\n");
            printf (" -l(ocale) file    use locale from full path to quimup_xx.qm file\n");
            printf (" -p(lay) %%U        play files in %%U (or %%F) in new playlist\n");
            printf ("  %%U               append files in %%U (or %%F) to the playlist\n----\n");
            return 0;
        }
        else // if URL
        if ( msg.startsWith("file://"))
        {
            // we have a string in URL format, so convert special chars:
            msg = ( QUrl::fromEncoded (static_cast<const char*>(msg.toUtf8())) ).toString();
            message.append(msg);
            filecount++;
        }
        else //  if plain path
        if (msg.startsWith("/") )
        {
            msg = "file://" + msg; // used as separator
            message.append(msg);
            filecount++;
        }
    }

    // send the args (and find out if a previous instance exists)
    if (!b_multiple_instances && app.sendMessage(message))
    {
        printf ("Quimup : already running (use '-i' to force a new instance)\n");
        return 0;
    }

    printf ("Quimup : starting new instance\n");

    //// Localization start >
    QTranslator trnsltr;
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    bool b_locale_loaded = false;
    bool b_locale_default = true;
    if (locale == "xx") // no -l parameter was passed
        locale = QLocale::system().name();
    else
        b_locale_default = false;

    if (locale != "none")
    {
        if (!b_locale_default)
        {
            printf ("Locale : '%s': ", static_cast<const char*>(locale.toUtf8()));
            if ( locale.startsWith("/") && locale.endsWith(".qm")) // full path to a file
            {
                if ( trnsltr.load(locale) )
                {
                   printf ("OK\n");
                   b_locale_loaded = true;
                }
                else
                   printf ("failed! (using none)\n");
            }
            else // first try installation path, next default path
            {
                QDir dir(QApplication::applicationDirPath());

                if ( trnsltr.load(QString("quimup_") + locale, dir.absolutePath()) )
                {
                    printf ("OK (from %s)\n", static_cast<const char*>(dir.absolutePath().toUtf8()));
                    b_locale_loaded = true;
                }
                else
                {
                    if ( trnsltr.load(QString("quimup_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
                    {
                        printf ("OK (from default location)\n");
                        b_locale_loaded = true;
                    }
                    else
                         printf ("failed! (using none)\n");
                }
            }

        }
        else // locale file in defaut system location
        {
            printf ("Locale : ");
            if ( trnsltr.load(QString("quimup_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
            {
                printf ("using default (%s)\n", static_cast<const char*>(locale.toUtf8()));
                b_locale_loaded = true;
            }
            else
                printf ("using none\n");
        }
    }
    else
       printf ("Locale : using none (requested)\n");

    if (b_locale_loaded)
        app.installTranslator(&trnsltr);

    //// < end Localization

    qm_core *core = new qm_core();

    // handle files
    if (filecount > 0)
    {
        core->player->browser_window->plist_view->on_open_with_request(message);
    }

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)), core, SLOT(on_message_from_2nd_instance(const QString&)));
    QObject::connect(&app, SIGNAL(aboutToQuit()), core, SLOT(on_system_quit()));

    return app.exec();
}


