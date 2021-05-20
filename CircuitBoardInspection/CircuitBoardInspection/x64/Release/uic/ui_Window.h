/********************************************************************************
** Form generated from reading UI file 'Window.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainWindowClass
{
public:
    QAction *actionRun;
    QAction *actionImageSetting;
    QAction *actionStatistics;
    QAction *actionEnlarge;
    QAction *actionClose;
    QAction *actionSignal;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QLabel *labelDisplay;
    QWidget *widget_3;
    QGridLayout *gridLayout_2;
    QLabel *labelDisplayRealTime;
    QWidget *widget_2;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBoxResultDisplay;
    QGridLayout *gridLayout_7;
    QLabel *label;
    QLabel *labelModelName;
    QLabel *label_2;
    QLabel *labelTotalCounts;
    QLabel *label_3;
    QLabel *labelOKCounts;
    QLabel *label_4;
    QLabel *labelNGCounts;
    QWidget *widget_5;
    QGridLayout *gridLayout_6;
    QLabel *labelResult;
    QLabel *labelNormPart;
    QLabel *labelErrorPart;
    QStatusBar *statusbar;
    QToolBar *toolBarAction;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QToolBar *toolBar;

    void setupUi(QMainWindow *mainWindowClass)
    {
        if (mainWindowClass->objectName().isEmpty())
            mainWindowClass->setObjectName(QString::fromUtf8("mainWindowClass"));
        mainWindowClass->resize(1200, 800);
        QFont font;
        font.setPointSize(10);
        mainWindowClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8("icon/SoftWare.png"), QSize(), QIcon::Normal, QIcon::Off);
        mainWindowClass->setWindowIcon(icon);
        actionRun = new QAction(mainWindowClass);
        actionRun->setObjectName(QString::fromUtf8("actionRun"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("icon/Run.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRun->setIcon(icon1);
        actionImageSetting = new QAction(mainWindowClass);
        actionImageSetting->setObjectName(QString::fromUtf8("actionImageSetting"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("icon/Set.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImageSetting->setIcon(icon2);
        actionStatistics = new QAction(mainWindowClass);
        actionStatistics->setObjectName(QString::fromUtf8("actionStatistics"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("icon/Table.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStatistics->setIcon(icon3);
        actionEnlarge = new QAction(mainWindowClass);
        actionEnlarge->setObjectName(QString::fromUtf8("actionEnlarge"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("icon/Enlarge.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionEnlarge->setIcon(icon4);
        actionClose = new QAction(mainWindowClass);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8("icon/Close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon5);
        actionSignal = new QAction(mainWindowClass);
        actionSignal->setObjectName(QString::fromUtf8("actionSignal"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8("icon/Signal.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSignal->setIcon(icon6);
        centralwidget = new QWidget(mainWindowClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        labelDisplay = new QLabel(widget);
        labelDisplay->setObjectName(QString::fromUtf8("labelDisplay"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelDisplay->sizePolicy().hasHeightForWidth());
        labelDisplay->setSizePolicy(sizePolicy);
        labelDisplay->setStyleSheet(QString::fromUtf8("background:gray"));
        labelDisplay->setFrameShape(QFrame::WinPanel);
        labelDisplay->setFrameShadow(QFrame::Sunken);
        labelDisplay->setPixmap(QPixmap(QString::fromUtf8("icon/Cover1.png")));
        labelDisplay->setScaledContents(true);
        labelDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(labelDisplay, 0, 0, 1, 1);


        gridLayout->addWidget(widget, 1, 0, 1, 2);

        widget_3 = new QWidget(centralwidget);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        gridLayout_2 = new QGridLayout(widget_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        labelDisplayRealTime = new QLabel(widget_3);
        labelDisplayRealTime->setObjectName(QString::fromUtf8("labelDisplayRealTime"));
        sizePolicy.setHeightForWidth(labelDisplayRealTime->sizePolicy().hasHeightForWidth());
        labelDisplayRealTime->setSizePolicy(sizePolicy);
        labelDisplayRealTime->setStyleSheet(QString::fromUtf8("background:gray"));
        labelDisplayRealTime->setFrameShape(QFrame::VLine);
        labelDisplayRealTime->setFrameShadow(QFrame::Sunken);
        labelDisplayRealTime->setScaledContents(false);
        labelDisplayRealTime->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(labelDisplayRealTime, 0, 0, 1, 1);


        gridLayout->addWidget(widget_3, 0, 1, 1, 1);

        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        gridLayout_4 = new QGridLayout(widget_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        groupBoxResultDisplay = new QGroupBox(widget_2);
        groupBoxResultDisplay->setObjectName(QString::fromUtf8("groupBoxResultDisplay"));
        groupBoxResultDisplay->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gridLayout_7 = new QGridLayout(groupBoxResultDisplay);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        label = new QLabel(groupBoxResultDisplay);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\347\255\211\347\272\277"));
        font1.setPointSize(27);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label->setLayoutDirection(Qt::LeftToRight);
        label->setFrameShape(QFrame::Box);
        label->setFrameShadow(QFrame::Raised);

        gridLayout_7->addWidget(label, 0, 0, 1, 1);

        labelModelName = new QLabel(groupBoxResultDisplay);
        labelModelName->setObjectName(QString::fromUtf8("labelModelName"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Calibri"));
        font2.setPointSize(27);
        font2.setBold(true);
        font2.setWeight(75);
        labelModelName->setFont(font2);
        labelModelName->setFrameShape(QFrame::Panel);
        labelModelName->setFrameShadow(QFrame::Sunken);
        labelModelName->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelModelName, 0, 1, 1, 1);

        label_2 = new QLabel(groupBoxResultDisplay);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);
        label_2->setLayoutDirection(Qt::LeftToRight);
        label_2->setStyleSheet(QString::fromUtf8("color:#0000aa"));
        label_2->setFrameShape(QFrame::Box);
        label_2->setFrameShadow(QFrame::Raised);
        label_2->setTextFormat(Qt::AutoText);
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_2, 1, 0, 1, 1);

        labelTotalCounts = new QLabel(groupBoxResultDisplay);
        labelTotalCounts->setObjectName(QString::fromUtf8("labelTotalCounts"));
        labelTotalCounts->setFont(font2);
        labelTotalCounts->setStyleSheet(QString::fromUtf8("color:#0000aa"));
        labelTotalCounts->setFrameShape(QFrame::Panel);
        labelTotalCounts->setFrameShadow(QFrame::Sunken);
        labelTotalCounts->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelTotalCounts, 1, 1, 1, 1);

        label_3 = new QLabel(groupBoxResultDisplay);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font1);
        label_3->setStyleSheet(QString::fromUtf8("color:#00aa00"));
        label_3->setFrameShape(QFrame::Box);
        label_3->setFrameShadow(QFrame::Raised);

        gridLayout_7->addWidget(label_3, 2, 0, 1, 1);

        labelOKCounts = new QLabel(groupBoxResultDisplay);
        labelOKCounts->setObjectName(QString::fromUtf8("labelOKCounts"));
        labelOKCounts->setFont(font2);
        labelOKCounts->setStyleSheet(QString::fromUtf8("color:#00aa00"));
        labelOKCounts->setFrameShape(QFrame::Panel);
        labelOKCounts->setFrameShadow(QFrame::Sunken);
        labelOKCounts->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelOKCounts, 2, 1, 1, 1);

        label_4 = new QLabel(groupBoxResultDisplay);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);
        label_4->setStyleSheet(QString::fromUtf8("color:#aa0000"));
        label_4->setFrameShape(QFrame::Box);
        label_4->setFrameShadow(QFrame::Raised);

        gridLayout_7->addWidget(label_4, 3, 0, 1, 1);

        labelNGCounts = new QLabel(groupBoxResultDisplay);
        labelNGCounts->setObjectName(QString::fromUtf8("labelNGCounts"));
        labelNGCounts->setFont(font2);
        labelNGCounts->setStyleSheet(QString::fromUtf8("color:#aa0000"));
        labelNGCounts->setFrameShape(QFrame::Panel);
        labelNGCounts->setFrameShadow(QFrame::Sunken);
        labelNGCounts->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelNGCounts, 3, 1, 1, 1);

        gridLayout_7->setColumnStretch(0, 6);
        gridLayout_7->setColumnStretch(1, 4);

        gridLayout_4->addWidget(groupBoxResultDisplay, 0, 0, 1, 1);

        widget_5 = new QWidget(widget_2);
        widget_5->setObjectName(QString::fromUtf8("widget_5"));
        gridLayout_6 = new QGridLayout(widget_5);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        labelResult = new QLabel(widget_5);
        labelResult->setObjectName(QString::fromUtf8("labelResult"));
        sizePolicy.setHeightForWidth(labelResult->sizePolicy().hasHeightForWidth());
        labelResult->setSizePolicy(sizePolicy);
        QFont font3;
        font3.setFamily(QString::fromUtf8("\345\215\216\346\226\207\344\270\255\345\256\213"));
        font3.setBold(true);
        font3.setItalic(false);
        font3.setWeight(75);
        labelResult->setFont(font3);
        labelResult->setStyleSheet(QString::fromUtf8("background:black"));
        labelResult->setFrameShape(QFrame::WinPanel);
        labelResult->setFrameShadow(QFrame::Raised);
        labelResult->setTextFormat(Qt::AutoText);
        labelResult->setScaledContents(false);
        labelResult->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(labelResult, 0, 0, 1, 2);

        labelNormPart = new QLabel(widget_5);
        labelNormPart->setObjectName(QString::fromUtf8("labelNormPart"));
        sizePolicy.setHeightForWidth(labelNormPart->sizePolicy().hasHeightForWidth());
        labelNormPart->setSizePolicy(sizePolicy);
        labelNormPart->setStyleSheet(QString::fromUtf8("background:gray"));
        labelNormPart->setFrameShape(QFrame::HLine);
        labelNormPart->setFrameShadow(QFrame::Sunken);
        labelNormPart->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(labelNormPart, 1, 0, 1, 1);

        labelErrorPart = new QLabel(widget_5);
        labelErrorPart->setObjectName(QString::fromUtf8("labelErrorPart"));
        sizePolicy.setHeightForWidth(labelErrorPart->sizePolicy().hasHeightForWidth());
        labelErrorPart->setSizePolicy(sizePolicy);
        labelErrorPart->setStyleSheet(QString::fromUtf8("background:gray"));
        labelErrorPart->setFrameShape(QFrame::HLine);
        labelErrorPart->setFrameShadow(QFrame::Sunken);
        labelErrorPart->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(labelErrorPart, 1, 1, 1, 1);


        gridLayout_4->addWidget(widget_5, 0, 1, 1, 1);

        gridLayout_4->setColumnStretch(0, 2);
        gridLayout_4->setColumnStretch(1, 1);

        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        gridLayout->setRowStretch(0, 4);
        gridLayout->setRowStretch(1, 6);
        gridLayout->setColumnStretch(0, 6);
        gridLayout->setColumnStretch(1, 4);
        mainWindowClass->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(mainWindowClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        statusbar->setMouseTracking(false);
        mainWindowClass->setStatusBar(statusbar);
        toolBarAction = new QToolBar(mainWindowClass);
        toolBarAction->setObjectName(QString::fromUtf8("toolBarAction"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(toolBarAction->sizePolicy().hasHeightForWidth());
        toolBarAction->setSizePolicy(sizePolicy1);
        toolBarAction->setIconSize(QSize(48, 48));
        mainWindowClass->addToolBar(Qt::TopToolBarArea, toolBarAction);
        menubar = new QMenuBar(mainWindowClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 23));
        menubar->setMouseTracking(true);
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menu_3 = new QMenu(menubar);
        menu_3->setObjectName(QString::fromUtf8("menu_3"));
        mainWindowClass->setMenuBar(menubar);
        toolBar = new QToolBar(mainWindowClass);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setLayoutDirection(Qt::RightToLeft);
        toolBar->setIconSize(QSize(48, 48));
        mainWindowClass->addToolBar(Qt::TopToolBarArea, toolBar);

        toolBarAction->addAction(actionRun);
        toolBarAction->addSeparator();
        toolBarAction->addAction(actionImageSetting);
        toolBarAction->addSeparator();
        toolBarAction->addAction(actionStatistics);
        toolBarAction->addSeparator();
        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menubar->addAction(menu_3->menuAction());
        menu->addAction(actionImageSetting);
        menu_2->addAction(actionRun);
        menu_3->addAction(actionStatistics);
        toolBar->addAction(actionClose);
        toolBar->addSeparator();
        toolBar->addAction(actionEnlarge);
        toolBar->addSeparator();
        toolBar->addSeparator();
        toolBar->addAction(actionSignal);
        toolBar->addSeparator();

        retranslateUi(mainWindowClass);

        QMetaObject::connectSlotsByName(mainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainWindowClass)
    {
        mainWindowClass->setWindowTitle(QApplication::translate("mainWindowClass", "\345\215\225\345\267\245\344\275\215\350\247\206\350\247\211\346\243\200\346\237\245\347\263\273\347\273\237", nullptr));
        actionRun->setText(QApplication::translate("mainWindowClass", "\350\277\220\350\241\214", nullptr));
        actionImageSetting->setText(QApplication::translate("mainWindowClass", "\345\233\276\345\203\217\350\256\276\347\275\256", nullptr));
        actionStatistics->setText(QApplication::translate("mainWindowClass", "\347\273\237\350\256\241", nullptr));
        actionEnlarge->setText(QApplication::translate("mainWindowClass", "\347\274\251\346\224\276", nullptr));
        actionClose->setText(QApplication::translate("mainWindowClass", "\345\205\263\351\227\255", nullptr));
        actionSignal->setText(QApplication::translate("mainWindowClass", "\345\244\226\351\203\250\350\247\246\345\217\221\344\277\241\345\217\267", nullptr));
        labelDisplay->setText(QString());
        labelDisplayRealTime->setText(QString());
        groupBoxResultDisplay->setTitle(QString());
        label->setText(QApplication::translate("mainWindowClass", "\350\277\236\346\216\245\344\273\266\345\220\215\347\247\260 :", nullptr));
        labelModelName->setText(QApplication::translate("mainWindowClass", "/", nullptr));
        label_2->setText(QApplication::translate("mainWindowClass", "\346\200\273\346\254\241\346\225\260 :", nullptr));
        labelTotalCounts->setText(QApplication::translate("mainWindowClass", "0", nullptr));
        label_3->setText(QApplication::translate("mainWindowClass", "\345\220\210\346\240\274\346\254\241\346\225\260 :", nullptr));
        labelOKCounts->setText(QApplication::translate("mainWindowClass", "0", nullptr));
        label_4->setText(QApplication::translate("mainWindowClass", "NG\346\254\241\346\225\260 :", nullptr));
        labelNGCounts->setText(QApplication::translate("mainWindowClass", "0", nullptr));
        labelResult->setText(QApplication::translate("mainWindowClass", "<html><head/><body><p><span style=\" font-size:72pt; color:#00aa00;\">OK</span></p></body></html>", nullptr));
        labelNormPart->setText(QString());
        labelErrorPart->setText(QString());
        toolBarAction->setWindowTitle(QApplication::translate("mainWindowClass", "toolBar", nullptr));
        menu->setTitle(QApplication::translate("mainWindowClass", "\350\256\276\347\275\256", nullptr));
        menu_2->setTitle(QApplication::translate("mainWindowClass", "\350\277\220\350\241\214", nullptr));
        menu_3->setTitle(QApplication::translate("mainWindowClass", "\347\273\237\350\256\241", nullptr));
        toolBar->setWindowTitle(QApplication::translate("mainWindowClass", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class mainWindowClass: public Ui_mainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
