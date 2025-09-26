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

    layout->addWidget(statusLabel);
    layout->addWidget(nextAlertLabel);
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
    trayIcon->showMessage("Timer Started",
                          "Next alert at " + nextAlertTime.toString("hh:mm"),
                          QSystemTrayIcon::Information, 3000);
}

TimerApp::~TimerApp()
{
    if (reminderDialog) {
        delete reminderDialog;
        reminderDialog = nullptr;
    }
}

void TimerApp::setupTrayIcon()
{
    // Create tray icon menu
    QMenu *trayMenu = new QMenu(this);

    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, this, &TimerApp::quitApp);

    trayMenu->addAction(quitAction);

    // Set up tray icon
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(QIcon(":/icons/timer_icon.png"));
    trayIcon->setToolTip("Hourly Timer");
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated,
            this, &TimerApp::iconActivated);
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
