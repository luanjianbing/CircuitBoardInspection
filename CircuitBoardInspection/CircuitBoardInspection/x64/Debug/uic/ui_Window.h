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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainWindowClass
{
public:
    QAction *actionRun;
    QAction *actionImageSetting;
    QAction *actionStatistics;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *widget_4;
    QGridLayout *gridLayout_5;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonRun;
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
    QWidget *widget_3;
    QGridLayout *gridLayout_2;
    QLabel *labelDisplayRealTime;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QLabel *labelDisplay;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *mainWindowClass)
    {
        if (mainWindowClass->objectName().isEmpty())
            mainWindowClass->setObjectName(QString::fromUtf8("mainWindowClass"));
        mainWindowClass->resize(1200, 800);
        actionRun = new QAction(mainWindowClass);
        actionRun->setObjectName(QString::fromUtf8("actionRun"));
        actionImageSetting = new QAction(mainWindowClass);
        actionImageSetting->setObjectName(QString::fromUtf8("actionImageSetting"));
        actionStatistics = new QAction(mainWindowClass);
        actionStatistics->setObjectName(QString::fromUtf8("actionStatistics"));
        centralwidget = new QWidget(mainWindowClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget_4 = new QWidget(centralwidget);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        gridLayout_5 = new QGridLayout(widget_4);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        horizontalSpacer = new QSpacerItem(1080, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer, 0, 0, 1, 1);

        buttonRun = new QPushButton(widget_4);
        buttonRun->setObjectName(QString::fromUtf8("buttonRun"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonRun->sizePolicy().hasHeightForWidth());
        buttonRun->setSizePolicy(sizePolicy);

        gridLayout_5->addWidget(buttonRun, 0, 1, 1, 1);


        gridLayout->addWidget(widget_4, 0, 0, 1, 2);

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
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(27);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setLayoutDirection(Qt::LeftToRight);

        gridLayout_7->addWidget(label, 0, 0, 1, 1);

        labelModelName = new QLabel(groupBoxResultDisplay);
        labelModelName->setObjectName(QString::fromUtf8("labelModelName"));
        QFont font1;
        font1.setPointSize(27);
        font1.setBold(true);
        font1.setWeight(75);
        labelModelName->setFont(font1);
        labelModelName->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelModelName, 0, 1, 1, 1);

        label_2 = new QLabel(groupBoxResultDisplay);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);
        label_2->setLayoutDirection(Qt::LeftToRight);
        label_2->setStyleSheet(QString::fromUtf8("color:#0000aa"));
        label_2->setTextFormat(Qt::AutoText);
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_2, 1, 0, 1, 1);

        labelTotalCounts = new QLabel(groupBoxResultDisplay);
        labelTotalCounts->setObjectName(QString::fromUtf8("labelTotalCounts"));
        labelTotalCounts->setFont(font1);
        labelTotalCounts->setStyleSheet(QString::fromUtf8("color:#0000aa"));
        labelTotalCounts->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelTotalCounts, 1, 1, 1, 1);

        label_3 = new QLabel(groupBoxResultDisplay);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font1);
        label_3->setStyleSheet(QString::fromUtf8("color:#00aa00"));

        gridLayout_7->addWidget(label_3, 2, 0, 1, 1);

        labelOKCounts = new QLabel(groupBoxResultDisplay);
        labelOKCounts->setObjectName(QString::fromUtf8("labelOKCounts"));
        labelOKCounts->setFont(font1);
        labelOKCounts->setStyleSheet(QString::fromUtf8("color:#00aa00"));
        labelOKCounts->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(labelOKCounts, 2, 1, 1, 1);

        label_4 = new QLabel(groupBoxResultDisplay);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);
        label_4->setStyleSheet(QString::fromUtf8("color:#aa0000"));

        gridLayout_7->addWidget(label_4, 3, 0, 1, 1);

        labelNGCounts = new QLabel(groupBoxResultDisplay);
        labelNGCounts->setObjectName(QString::fromUtf8("labelNGCounts"));
        labelNGCounts->setFont(font1);
        labelNGCounts->setStyleSheet(QString::fromUtf8("color:#aa0000"));
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
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelResult->sizePolicy().hasHeightForWidth());
        labelResult->setSizePolicy(sizePolicy1);
        labelResult->setStyleSheet(QString::fromUtf8("background:black"));
        labelResult->setTextFormat(Qt::AutoText);
        labelResult->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(labelResult, 0, 0, 1, 2);

        labelNormPart = new QLabel(widget_5);
        labelNormPart->setObjectName(QString::fromUtf8("labelNormPart"));
        sizePolicy1.setHeightForWidth(labelNormPart->sizePolicy().hasHeightForWidth());
        labelNormPart->setSizePolicy(sizePolicy1);
        labelNormPart->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(labelNormPart, 1, 0, 1, 1);

        labelErrorPart = new QLabel(widget_5);
        labelErrorPart->setObjectName(QString::fromUtf8("labelErrorPart"));
        sizePolicy1.setHeightForWidth(labelErrorPart->sizePolicy().hasHeightForWidth());
        labelErrorPart->setSizePolicy(sizePolicy1);
        labelErrorPart->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(labelErrorPart, 1, 1, 1, 1);


        gridLayout_4->addWidget(widget_5, 0, 1, 1, 1);

        gridLayout_4->setColumnStretch(0, 2);
        gridLayout_4->setColumnStretch(1, 1);

        gridLayout->addWidget(widget_2, 1, 0, 1, 1);

        widget_3 = new QWidget(centralwidget);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        gridLayout_2 = new QGridLayout(widget_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        labelDisplayRealTime = new QLabel(widget_3);
        labelDisplayRealTime->setObjectName(QString::fromUtf8("labelDisplayRealTime"));
        sizePolicy1.setHeightForWidth(labelDisplayRealTime->sizePolicy().hasHeightForWidth());
        labelDisplayRealTime->setSizePolicy(sizePolicy1);
        labelDisplayRealTime->setStyleSheet(QString::fromUtf8("background:black"));
        labelDisplayRealTime->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(labelDisplayRealTime, 0, 0, 1, 1);


        gridLayout->addWidget(widget_3, 1, 1, 1, 1);

        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        labelDisplay = new QLabel(widget);
        labelDisplay->setObjectName(QString::fromUtf8("labelDisplay"));
        sizePolicy1.setHeightForWidth(labelDisplay->sizePolicy().hasHeightForWidth());
        labelDisplay->setSizePolicy(sizePolicy1);
        labelDisplay->setStyleSheet(QString::fromUtf8("background:black"));
        labelDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(labelDisplay, 0, 0, 1, 1);


        gridLayout->addWidget(widget, 2, 0, 1, 2);

        gridLayout->setRowStretch(0, 1);
        gridLayout->setRowStretch(1, 4);
        gridLayout->setRowStretch(2, 5);
        gridLayout->setColumnStretch(0, 6);
        gridLayout->setColumnStretch(1, 4);
        mainWindowClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(mainWindowClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 23));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menu_3 = new QMenu(menubar);
        menu_3->setObjectName(QString::fromUtf8("menu_3"));
        mainWindowClass->setMenuBar(menubar);
        statusbar = new QStatusBar(mainWindowClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        mainWindowClass->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menubar->addAction(menu_3->menuAction());
        menu->addAction(actionImageSetting);
        menu_2->addAction(actionRun);
        menu_3->addAction(actionStatistics);

        retranslateUi(mainWindowClass);

        QMetaObject::connectSlotsByName(mainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainWindowClass)
    {
        mainWindowClass->setWindowTitle(QApplication::translate("mainWindowClass", "\347\272\277\350\267\257\346\235\277\346\243\200\346\237\245\347\263\273\347\273\237", nullptr));
        actionRun->setText(QApplication::translate("mainWindowClass", "\350\277\220\350\241\214", nullptr));
        actionImageSetting->setText(QApplication::translate("mainWindowClass", "\345\233\276\345\203\217\350\256\276\347\275\256", nullptr));
        actionStatistics->setText(QApplication::translate("mainWindowClass", "\347\273\237\350\256\241", nullptr));
        buttonRun->setText(QApplication::translate("mainWindowClass", "\345\220\257\345\212\250", nullptr));
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
        labelDisplayRealTime->setText(QApplication::translate("mainWindowClass", "labelDisplayRealTime", nullptr));
        labelDisplay->setText(QApplication::translate("mainWindowClass", "labelDisplayGrab", nullptr));
        menu->setTitle(QApplication::translate("mainWindowClass", "\350\256\276\347\275\256", nullptr));
        menu_2->setTitle(QApplication::translate("mainWindowClass", "\350\277\220\350\241\214", nullptr));
        menu_3->setTitle(QApplication::translate("mainWindowClass", "\347\273\237\350\256\241", nullptr));
    } // retranslateUi

};

namespace Ui {
    class mainWindowClass: public Ui_mainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
