#include "timerapp.h"
#include "reminderdialog.h"
#include <QMenu>
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QStyle>
#include <QPushButton>

// TimerApp::TimerApp(QWidget *parent)
//     : QMainWindow(parent)
//     , trayIcon(new QSystemTrayIcon(this))
//     , hourlyTimer(new QTimer(this))
//     , snoozeTimer(new QTimer(this))
//     , reminderDialog(nullptr)
// {
//     setWindowTitle("Hourly Timer");
//     resize(300, 200);

//     // Create central widget and layout
//     QWidget *centralWidget = new QWidget(this);
//     QVBoxLayout *layout = new QVBoxLayout(centralWidget);

//     // Add UI elements
//     statusLabel = new QLabel("Timer is running", this);
//     statusLabel->setAlignment(Qt::AlignCenter);
//     statusLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");

//     nextAlertLabel = new QLabel("Next alert: Calculating...", this);
//     nextAlertLabel->setAlignment(Qt::AlignCenter);

//     layout->addWidget(statusLabel);
//     layout->addWidget(nextAlertLabel);
//     layout->addStretch();

//     setCentralWidget(centralWidget);

//     setupTrayIcon();
//     setupTimers();

//     // Set initial alert time to next full hour
//     QDateTime now = QDateTime::currentDateTime();
//     QTime nextHour = now.time().addSecs(3600 - (now.time().minute() * 60 + now.time().second()));
//     nextAlertTime = nextHour;

//     // Update the UI
//     nextAlertLabel->setText("Next alert: " + nextAlertTime.toString("hh:mm"));

//     // Start the timer for the first alert
//     startNextHourTimer();

//     // Show initial message
//     trayIcon->showMessage("Timer Started",
//                           "Next alert at " + nextAlertTime.toString("hh:mm"),
//                           QSystemTrayIcon::Information, 3000);
// }

TimerApp::TimerApp(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , hourlyTimer(new QTimer(this))
    , snoozeTimer(new QTimer(this))
    , reminderDialog(nullptr)
{
    setWindowTitle("Hourly Timer");
    resize(300, 200);

    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Add UI elements
    statusLabel = new QLabel("Timer is running", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");

    nextAlertLabel = new QLabel("Next alert: Calculating...", this);
    nextAlertLabel->setAlignment(Qt::AlignCenter);

    // Add minimize button
    QPushButton *minimizeButton = new QPushButton("Minimize to System Tray", this);
    connect(minimizeButton, &QPushButton::clicked, this, [this]() {
        hide();
        if (trayIcon && trayIcon->isVisible()) {
            trayIcon->showMessage("Timer Running",
                                  "Application minimized to system tray",
                                  QSystemTrayIcon::Information, 2000);
        }
    });

    layout->addWidget(statusLabel);
    layout->addWidget(nextAlertLabel);
    layout->addWidget(minimizeButton);
    layout->addStretch();

    setCentralWidget(centralWidget);

    setupTrayIcon();
    setupTimers();

    // Set initial alert time to next full hour
    QDateTime now = QDateTime::currentDateTime();
    QTime nextHour = now.time().addSecs(3600 - (now.time().minute() * 60 + now.time().second()));
    nextAlertTime = nextHour;

    // Update the UI
    nextAlertLabel->setText("Next alert: " + nextAlertTime.toString("hh:mm"));

    // Start the timer for the first alert
    startNextHourTimer();

    // Show initial message
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage("Timer Started",
                              "Next alert at " + nextAlertTime.toString("hh:mm"),
                              QSystemTrayIcon::Information, 3000);
    }
}

TimerApp::~TimerApp()
{
    if (reminderDialog) {
        delete reminderDialog;
        reminderDialog = nullptr;
    }
}

void TimerApp::showMainWindow()
{
    show();
    raise();
    activateWindow();
}

// void TimerApp::setupTrayIcon()
// {
//     // Check if system tray is available
//     if (!QSystemTrayIcon::isSystemTrayAvailable()) {
//         QMessageBox::critical(this, "Error",
//                               "System tray is not available on this system.");
//         return;
//     }

//     // Create tray icon menu
//     QMenu *trayMenu = new QMenu(this);

//     QAction *showAction = new QAction("Show Window", this);
//     connect(showAction, &QAction::triggered, this, &TimerApp::showMainWindow);

//     QAction *quitAction = new QAction("Quit", this);
//     connect(quitAction, &QAction::triggered, this, &TimerApp::quitApp);

//     trayMenu->addAction(showAction);
//     trayMenu->addAction(quitAction);

//     // Set up tray icon
//     trayIcon->setContextMenu(trayMenu);

//     // Try to load the icon, use a default if it fails
//     QIcon icon(":/icons/timer_icon.png");
//     if (icon.isNull()) {
//         // Fallback to a standard icon
//         icon = style()->standardIcon(QStyle::SP_ComputerIcon);
//         qDebug() << "Could not load custom icon, using standard icon";
//     }

//     trayIcon->setIcon(icon);
//     trayIcon->setToolTip("Hourly Timer");

//     // Show the tray icon
//     trayIcon->show();

//     connect(trayIcon, &QSystemTrayIcon::activated,
//             this, &TimerApp::iconActivated);

//     // Connect a signal to handle tray icon changes
//     connect(trayIcon, &QSystemTrayIcon::messageClicked,
//             this, [this]() {
//                 show();
//                 raise();
//                 activateWindow();
//             });
// }

void TimerApp::setupTrayIcon()
{
    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // Add a status message to the main window instead
        if (statusLabel) {
            statusLabel->setText("Timer is running (System tray not available)");
        }
        qWarning() << "System tray not available on this system";
        return;
    }

    // Create tray icon menu
    QMenu *trayMenu = new QMenu(this);

    QAction *showAction = new QAction("Show Window", this);
    connect(showAction, &QAction::triggered, this, &TimerApp::showMainWindow);

    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, this, &TimerApp::quitApp);

    trayMenu->addAction(showAction);
    trayMenu->addAction(quitAction);

    // Set up tray icon
    trayIcon->setContextMenu(trayMenu);

    // Try to load the icon, use a default if it fails
    QIcon icon(":/icons/timer_icon.png");
    if (icon.isNull()) {
        // Fallback to a standard icon
        icon = style()->standardIcon(QStyle::SP_ComputerIcon);
        qDebug() << "Could not load custom icon, using standard icon";
    }

    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Hourly Timer");

    // Show the tray icon
    if (!trayIcon->isVisible()) {
        trayIcon->show();
    }

    // Verify the icon is visible
    if (!trayIcon->isVisible()) {
        qWarning() << "Failed to show system tray icon";
        if (statusLabel) {
            statusLabel->setText("Timer is running (Failed to show tray icon)");
        }
    } else {
        qDebug() << "System tray icon is visible";
    }

    connect(trayIcon, &QSystemTrayIcon::activated,
            this, &TimerApp::iconActivated);

    // Connect a signal to handle tray icon changes
    connect(trayIcon, &QSystemTrayIcon::messageClicked,
            this, [this]() {
                show();
                raise();
                activateWindow();
            });
}

void TimerApp::setupTimers()
{
    hourlyTimer->setSingleShot(true);
    connect(hourlyTimer, &QTimer::timeout, this, &TimerApp::showReminder);

    snoozeTimer->setSingleShot(true);
    connect(snoozeTimer, &QTimer::timeout, this, &TimerApp::showReminder);
}

void TimerApp::startNextHourTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    QTime nowTime = now.time();

    // Calculate milliseconds until next full hour
    int msecsToNextHour = (3600 - (nowTime.minute() * 60 + nowTime.second())) * 1000;

    hourlyTimer->start(msecsToNextHour);

    // Update next alert time
    nextAlertTime = nowTime.addSecs(3600 - (nowTime.minute() * 60 + nowTime.second()));

    // Update UI
    if (nextAlertLabel) {
        nextAlertLabel->setText("Next alert: " + nextAlertTime.toString("hh:mm"));
    }
}

void TimerApp::showReminder()
{
    if (!reminderDialog) {
        reminderDialog = new ReminderDialog(this);
        connect(reminderDialog, &ReminderDialog::snoozeClicked, this, &TimerApp::snoozeReminder);
        connect(reminderDialog, &ReminderDialog::dismissClicked, this, &TimerApp::dismissReminder);
    }

    reminderDialog->show();
    reminderDialog->raise();
    reminderDialog->activateWindow();

    // Show system notification
    trayIcon->showMessage("Hourly Reminder",
                          "Time: " + QDateTime::currentDateTime().toString("hh:mm"),
                          QSystemTrayIcon::Warning, 5000);
}

void TimerApp::snoozeReminder()
{
    if (reminderDialog) {
        reminderDialog->hide();
    }

    // Set snooze timer for 5 minutes
    snoozeTimer->start(5 * 60 * 1000); // 5 minutes in milliseconds

    // Update UI
    if (nextAlertLabel) {
        nextAlertLabel->setText("Next alert: In 5 minutes");
    }

    trayIcon->showMessage("Reminder Snoozed",
                          "Next alert in 5 minutes",
                          QSystemTrayIcon::Information, 3000);
}

void TimerApp::dismissReminder()
{
    if (reminderDialog) {
        reminderDialog->hide();
    }

    // Restart the hourly timer
    startNextHourTimer();

    trayIcon->showMessage("Reminder Dismissed",
                          "Next alert at " + nextAlertTime.toString("hh:mm"),
                          QSystemTrayIcon::Information, 3000);
}

void TimerApp::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (!isVisible()) {
            show();
            raise();
            activateWindow();
        } else {
            hide();
        }
    }
}

void TimerApp::quitApp()
{
    QApplication::quit();
}

// Now this function matches the declaration in the header
void TimerApp::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
