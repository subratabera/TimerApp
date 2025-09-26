#include "timerapp.h"
#include "reminderdialog.h"
#include "timesetdialog.h"
#include "intervalsetdialog.h"
#include <QMenu>
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStyle>

TimerApp::TimerApp(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , alertTimer(new QTimer(this))
    , snoozeTimer(new QTimer(this))
    , countdownTimer(new QTimer(this))
    , reminderDialog(nullptr)
    , timeSetDialog(nullptr)
    , intervalSetDialog(nullptr)
    , customMinute(-1)
    , customInterval(60)  // Default to 60 minutes (1 hour)
{
    setWindowTitle("Hourly Timer");

    // Set a standard system icon for the window
    setWindowIcon(style()->standardIcon(QStyle::SP_ComputerIcon));

    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Set layout margins and spacing
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Add UI elements with larger, bold fonts
    statusLabel = new QLabel("Timer is running", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 20px; font-weight: bold;");

    nextAlertLabel = new QLabel("Next alert: Calculating...", this);
    nextAlertLabel->setAlignment(Qt::AlignCenter);
    nextAlertLabel->setStyleSheet("font-size: 32px; font-weight: bold;");

    // Add countdown label with larger, bold font
    countdownLabel = new QLabel("Time remaining: --:--:--", this);
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setStyleSheet("font-size: 20px; font-weight: bold;");

    // Create first horizontal layout for setting buttons
    QHBoxLayout *settingButtonsLayout = new QHBoxLayout();
    settingButtonsLayout->setSpacing(10);

    // Add set time button
    setTimeButton = new QPushButton("Set Alert Time", this);
    setTimeButton->setStyleSheet("font-size: 14px; font-weight: bold;");
    setTimeButton->setMinimumHeight(40);
    setTimeButton->setMinimumWidth(120);
    setTimeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(setTimeButton, &QPushButton::clicked, this, &TimerApp::showTimeSetDialog);

    // Add set interval button
    setIntervalButton = new QPushButton("Set Interval", this);
    setIntervalButton->setStyleSheet("font-size: 14px; font-weight: bold;");
    setIntervalButton->setMinimumHeight(40);
    setIntervalButton->setMinimumWidth(120);
    setIntervalButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(setIntervalButton, &QPushButton::clicked, this, &TimerApp::showIntervalSetDialog);

    // Add setting buttons to first layout
    settingButtonsLayout->addWidget(setTimeButton);
    settingButtonsLayout->addWidget(setIntervalButton);

    // Set stretch factors to make buttons equal width
    settingButtonsLayout->setStretchFactor(setTimeButton, 1);
    settingButtonsLayout->setStretchFactor(setIntervalButton, 1);

    // Create second horizontal layout for action buttons
    QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
    actionButtonsLayout->setSpacing(10);

    // Add minimize button
    minimizeButton = new QPushButton("Minimize", this);
    minimizeButton->setStyleSheet("font-size: 14px; font-weight: bold;");
    minimizeButton->setMinimumHeight(40);
    minimizeButton->setMinimumWidth(120);
    minimizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(minimizeButton, &QPushButton::clicked, this, [this]() {
        hide();
        if (trayIcon && trayIcon->isVisible()) {
            trayIcon->showMessage("Timer Running",
                                  "Application minimized to system tray",
                                  QSystemTrayIcon::Information, 2000);
        }
    });

    // Add quit button
    quitButton = new QPushButton("Quit", this);
    quitButton->setStyleSheet("font-size: 14px; font-weight: bold;");
    quitButton->setMinimumHeight(40);
    quitButton->setMinimumWidth(120);
    quitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(quitButton, &QPushButton::clicked, this, &TimerApp::quitApp);

    // Add action buttons to second layout
    actionButtonsLayout->addWidget(minimizeButton);
    actionButtonsLayout->addWidget(quitButton);

    // Set stretch factors to make buttons equal width
    actionButtonsLayout->setStretchFactor(minimizeButton, 1);
    actionButtonsLayout->setStretchFactor(quitButton, 1);

    // Add all widgets to main layout
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(nextAlertLabel);
    mainLayout->addWidget(countdownLabel);
    mainLayout->addLayout(settingButtonsLayout);
    mainLayout->addLayout(actionButtonsLayout);

    // Add stretch to push everything up
    mainLayout->addStretch();

    setCentralWidget(centralWidget);

    // Set up system tray with a standard icon
    setupTrayIcon();
    setupTimers();

    // Set initial alert time
    startNextAlertTimer();

    // Update the UI
    updateNextAlertLabel();

    // Start the timers
    startCountdown();

    // Show initial message
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage("Timer Started",
                              "Next alert at " + nextAlertTime.toString("hh:mm"),
                              QSystemTrayIcon::Information, 3000);
    }

    // Adjust window size to fit contents
    adjustSize();

    // Set minimum width to accommodate all buttons
    setMinimumWidth(450);

    // Set minimum height to prevent shrinking too much
    setMinimumHeight(size().height());

    // Set maximum size to prevent expanding unnecessarily
    setMaximumSize(450, size().height());
}

TimerApp::~TimerApp()
{
    if (reminderDialog) {
        delete reminderDialog;
        reminderDialog = nullptr;
    }

    if (timeSetDialog) {
        delete timeSetDialog;
        timeSetDialog = nullptr;
    }

    if (intervalSetDialog) {
        delete intervalSetDialog;
        intervalSetDialog = nullptr;
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
    alertTimer->setSingleShot(true);
    connect(alertTimer, &QTimer::timeout, this, &TimerApp::showReminder);

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

void TimerApp::startNextAlertTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    QTime nowTime = now.time();

    QTime targetTime;

    if (customMinute >= 0) {
        // Use custom minute
        if (nowTime.minute() <= customMinute) {
            // Next alert is at the current hour with custom minute
            targetTime = QTime(nowTime.hour(), customMinute);
        } else {
            // Next alert is at the next hour with custom minute
            targetTime = QTime(nowTime.hour() + 1, customMinute);
        }
    } else {
        // Default behavior: next full interval
        int minutesToAdd = customInterval - (nowTime.minute() % customInterval);
        if (minutesToAdd == customInterval) {
            minutesToAdd = 0; // We're exactly at an interval boundary
        }
        targetTime = nowTime.addSecs(minutesToAdd * 60 - nowTime.second());
    }

    // Calculate milliseconds until the target time
    QDateTime targetDateTime = QDateTime(now.date(), targetTime);
    if (targetDateTime <= now) {
        targetDateTime = targetDateTime.addDays(1);
    }

    qint64 msecsToTarget = now.msecsTo(targetDateTime);
    alertTimer->start(msecsToTarget);

    // Update next alert time
    nextAlertTime = targetTime;

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
        trayIcon->showMessage("Timer Alert",
                              "Time: " + QDateTime::currentDateTime().toString("hh:mm"),
                              QSystemTrayIcon::Warning, 5000);
    }

    // Start timer for next alert
    startNextAlertTimer();
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

    // Start timer for next alert
    startNextAlertTimer();

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

void TimerApp::showTimeSetDialog()
{
    if (!timeSetDialog) {
        timeSetDialog = new TimeSetDialog(nextAlertTime, this);
    } else {
        timeSetDialog->setTime(nextAlertTime);
    }

    if (timeSetDialog->exec() == QDialog::Accepted) {
        // User clicked OK, update the custom minute
        QTime selectedTime = timeSetDialog->selectedTime();
        customMinute = selectedTime.minute();  // Store only the minute

        // Calculate the next alert time with the custom minute
        QDateTime now = QDateTime::currentDateTime();
        QTime nowTime = now.time();

        QTime targetTime;
        if (nowTime.minute() <= customMinute) {
            // Next alert is at the current hour with custom minute
            targetTime = QTime(nowTime.hour(), customMinute);
        } else {
            // Next alert is at the next hour with custom minute
            targetTime = QTime(nowTime.hour() + 1, customMinute);
        }

        // Calculate milliseconds until the target time
        QDateTime targetDateTime = QDateTime(now.date(), targetTime);
        if (targetDateTime <= now) {
            targetDateTime = targetDateTime.addDays(1);
        }

        qint64 msecsToTarget = now.msecsTo(targetDateTime);
        alertTimer->stop();
        alertTimer->start(msecsToTarget);

        // Update next alert time
        nextAlertTime = targetTime;

        // Update the UI
        updateNextAlertLabel();

        // Show notification
        if (trayIcon && trayIcon->isVisible()) {
            trayIcon->showMessage("Alert Time Updated",
                                  "Next alert at " + nextAlertTime.toString("hh:mm"),
                                  QSystemTrayIcon::Information, 3000);
        }
    }
}

void TimerApp::showIntervalSetDialog()
{
    if (!intervalSetDialog) {
        intervalSetDialog = new IntervalSetDialog(customInterval, this);
    } else {
        intervalSetDialog->setValue(customInterval);
    }

    if (intervalSetDialog->exec() == QDialog::Accepted) {
        // User clicked OK, update the custom interval
        customInterval = intervalSetDialog->selectedInterval();

        // Reset custom minute since we're now using interval-based alerts
        customMinute = -1;

        // Restart the alert timer with the new interval
        startNextAlertTimer();

        // Update status label to show the new interval
        if (customInterval == 60) {
            statusLabel->setText("Timer is running (1 hour interval)");
        } else {
            statusLabel->setText(QString("Timer is running (%1 minute interval)").arg(customInterval));
        }

        // Show notification
        if (trayIcon && trayIcon->isVisible()) {
            QString intervalText;
            if (customInterval == 60) {
                intervalText = "1 hour";
            } else if (customInterval < 60) {
                intervalText = QString("%1 minutes").arg(customInterval);
            } else {
                int hours = customInterval / 60;
                int minutes = customInterval % 60;
                if (minutes == 0) {
                    intervalText = QString("%1 hours").arg(hours);
                } else {
                    intervalText = QString("%1 hours %2 minutes").arg(hours).arg(minutes);
                }
            }

            trayIcon->showMessage("Interval Updated",
                                  "Alert interval set to " + intervalText,
                                  QSystemTrayIcon::Information, 3000);
        }
    }
}
