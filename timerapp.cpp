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
#include <QPushButton>
#include <QStyle>

TimerApp::TimerApp(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , hourlyTimer(new QTimer(this))
    , snoozeTimer(new QTimer(this))
    , countdownTimer(new QTimer(this))
    , reminderDialog(nullptr)
{
    setWindowTitle("Hourly Timer");

    // Set a standard system icon for the window
    setWindowIcon(style()->standardIcon(QStyle::SP_ComputerIcon));

    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Set layout margins and spacing
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Add UI elements with larger, bold fonts
    statusLabel = new QLabel("Timer is running", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 20px; font-weight: bold;");

    nextAlertLabel = new QLabel("Next alert: Calculating...", this);
    nextAlertLabel->setAlignment(Qt::AlignCenter);
    nextAlertLabel->setStyleSheet("font-size: 18px; font-weight: bold;");

    // Add countdown label with larger, bold font
    countdownLabel = new QLabel("Time remaining: --:--:--", this);
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

    // Add minimize button with larger, bold font
    QPushButton *minimizeButton = new QPushButton("Minimize to System Tray", this);
    minimizeButton->setStyleSheet("font-size: 16px; font-weight: bold;");
    minimizeButton->setMinimumHeight(50);
    connect(minimizeButton, &QPushButton::clicked, this, [this]() {
        hide();
        if (trayIcon && trayIcon->isVisible()) {
            trayIcon->showMessage("Timer Running",
                                  "Application minimized to system tray",
                                  QSystemTrayIcon::Information, 2000);
        }
    });

    // Add widgets to layout
    layout->addWidget(statusLabel);
    layout->addWidget(nextAlertLabel);
    layout->addWidget(countdownLabel);
    layout->addWidget(minimizeButton);

    // Add stretch to push everything up
    layout->addStretch();

    setCentralWidget(centralWidget);

    // Set up system tray with a standard icon
    setupTrayIcon();
    setupTimers();

    // Set initial alert time to next full hour
    QDateTime now = QDateTime::currentDateTime();
    QTime nextHour = now.time().addSecs(3600 - (now.time().minute() * 60 + now.time().second()));
    nextAlertTime = nextHour;

    // Update the UI
    updateNextAlertLabel();

    // Start the timers
    startNextHourTimer();
    startCountdown();

    // Show initial message
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage("Timer Started",
                              "Next alert at " + nextAlertTime.toString("hh:mm"),
                              QSystemTrayIcon::Information, 3000);
    }

    // Adjust window size to fit contents
    adjustSize();

    // Set minimum size to prevent shrinking too much
    setMinimumSize(size());

    // Set maximum size to prevent expanding unnecessarily
    setMaximumSize(size());
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
    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        if (statusLabel) {
            statusLabel->setText("Timer is running (System tray not available)");
        }
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

    // Set up tray icon with a standard system icon
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip("Hourly Timer");

    // Show the tray icon
    trayIcon->show();

    // Verify the icon is visible
    if (!trayIcon->isVisible()) {
        if (statusLabel) {
            statusLabel->setText("Timer is running (Failed to show tray icon)");
        }
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

    // Setup countdown timer
    countdownTimer->setInterval(1000);
    connect(countdownTimer, &QTimer::timeout, this, &TimerApp::updateCountdown);
}

void TimerApp::startCountdown()
{
    countdownTimer->start();
    updateCountdown();
}

void TimerApp::updateCountdown()
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime nextAlert = QDateTime(now.date(), nextAlertTime);

    // If nextAlert is earlier than now, add one day
    if (nextAlert <= now) {
        nextAlert = nextAlert.addDays(1);
    }

    qint64 secondsRemaining = now.secsTo(nextAlert);

    if (secondsRemaining <= 0) {
        countdownLabel->setText("Time remaining: 00:00:00");
        return;
    }

    int hours = secondsRemaining / 3600;
    int minutes = (secondsRemaining % 3600) / 60;
    int seconds = secondsRemaining % 60;

    QString timeText = QString("Time remaining: %1:%2:%3")
                           .arg(hours, 2, 10, QLatin1Char('0'))
                           .arg(minutes, 2, 10, QLatin1Char('0'))
                           .arg(seconds, 2, 10, QLatin1Char('0'));

    countdownLabel->setText(timeText);
}

void TimerApp::updateNextAlertLabel()
{
    nextAlertLabel->setText("Next alert: " + nextAlertTime.toString("hh:mm"));
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
    updateNextAlertLabel();
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
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage("Hourly Reminder",
                              "Time: " + QDateTime::currentDateTime().toString("hh:mm"),
                              QSystemTrayIcon::Warning, 5000);
    }
}

void TimerApp::snoozeReminder()
{
    if (reminderDialog) {
        reminderDialog->hide();
    }

    // Set snooze timer for 5 minutes
    snoozeTimer->start(5 * 60 * 1000);

    // Update next alert time for snooze
    QDateTime now = QDateTime::currentDateTime();
    nextAlertTime = now.time().addSecs(5 * 60);

    // Update UI
    updateNextAlertLabel();

    // Show notification
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage("Reminder Snoozed",
                              "Next alert in 5 minutes",
                              QSystemTrayIcon::Information, 3000);
    }
}

void TimerApp::dismissReminder()
{
    if (reminderDialog) {
        reminderDialog->hide();
    }

    // Restart the hourly timer
    startNextHourTimer();

    // Show notification
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage("Reminder Dismissed",
                              "Next alert at " + nextAlertTime.toString("hh:mm"),
                              QSystemTrayIcon::Information, 3000);
    }
}

void TimerApp::showMainWindow()
{
    show();
    raise();
    activateWindow();
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

void TimerApp::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
