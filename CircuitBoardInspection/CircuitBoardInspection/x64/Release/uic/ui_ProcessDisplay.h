/********************************************************************************
** Form generated from reading UI file 'ProcessDisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSDISPLAY_H
#define UI_PROCESSDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProcessResultDisplayClass
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QLabel *labelDisplay;
    QWidget *widget_2;
    QGridLayout *gridLayout_3;
    QLineEdit *lineEditX1;
    QLineEdit *lineEditY1;
    QLineEdit *lineEditX2;
    QLineEdit *lineEditY2;
    QPushButton *buttonSure;
    QLabel *labelDetectCate;
    QLabel *labelBlockIndex;
    QPushButton *buttonModify;
    QLabel *label_8;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *label;

    void setupUi(QWidget *ProcessResultDisplayClass)
    {
        if (ProcessResultDisplayClass->objectName().isEmpty())
            ProcessResultDisplayClass->setObjectName(QString::fromUtf8("ProcessResultDisplayClass"));
        ProcessResultDisplayClass->resize(600, 450);
        gridLayout = new QGridLayout(ProcessResultDisplayClass);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget = new QWidget(ProcessResultDisplayClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
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
        labelDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(labelDisplay, 0, 0, 1, 1);


        gridLayout->addWidget(widget, 0, 0, 1, 1);

        widget_2 = new QWidget(ProcessResultDisplayClass);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        QFont font;
        font.setFamily(QString::fromUtf8("\347\255\211\347\272\277"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        widget_2->setFont(font);
        gridLayout_3 = new QGridLayout(widget_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        lineEditX1 = new QLineEdit(widget_2);
        lineEditX1->setObjectName(QString::fromUtf8("lineEditX1"));
        lineEditX1->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEditX1->sizePolicy().hasHeightForWidth());
        lineEditX1->setSizePolicy(sizePolicy1);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\347\255\211\347\272\277"));
        font1.setPointSize(16);
        font1.setBold(true);
        font1.setWeight(75);
        lineEditX1->setFont(font1);
        lineEditX1->setEchoMode(QLineEdit::Normal);
        lineEditX1->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lineEditX1, 2, 2, 1, 1);

        lineEditY1 = new QLineEdit(widget_2);
        lineEditY1->setObjectName(QString::fromUtf8("lineEditY1"));
        lineEditY1->setEnabled(false);
        lineEditY1->setFont(font1);
        lineEditY1->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lineEditY1, 3, 2, 1, 1);

        lineEditX2 = new QLineEdit(widget_2);
        lineEditX2->setObjectName(QString::fromUtf8("lineEditX2"));
        lineEditX2->setEnabled(false);
        lineEditX2->setFont(font1);
        lineEditX2->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lineEditX2, 4, 2, 1, 1);

        lineEditY2 = new QLineEdit(widget_2);
        lineEditY2->setObjectName(QString::fromUtf8("lineEditY2"));
        lineEditY2->setEnabled(false);
        lineEditY2->setFont(font1);
        lineEditY2->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lineEditY2, 5, 2, 1, 1);

        buttonSure = new QPushButton(widget_2);
        buttonSure->setObjectName(QString::fromUtf8("buttonSure"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(buttonSure->sizePolicy().hasHeightForWidth());
        buttonSure->setSizePolicy(sizePolicy2);
        buttonSure->setFont(font1);

        gridLayout_3->addWidget(buttonSure, 6, 2, 1, 1);

        labelDetectCate = new QLabel(widget_2);
        labelDetectCate->setObjectName(QString::fromUtf8("labelDetectCate"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\347\255\211\347\272\277"));
        font2.setPointSize(20);
        font2.setBold(true);
        font2.setWeight(75);
        labelDetectCate->setFont(font2);
        labelDetectCate->setFrameShape(QFrame::Panel);
        labelDetectCate->setFrameShadow(QFrame::Sunken);
        labelDetectCate->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(labelDetectCate, 1, 2, 1, 1);

        labelBlockIndex = new QLabel(widget_2);
        labelBlockIndex->setObjectName(QString::fromUtf8("labelBlockIndex"));
        labelBlockIndex->setFont(font2);
        labelBlockIndex->setFrameShape(QFrame::Panel);
        labelBlockIndex->setFrameShadow(QFrame::Sunken);
        labelBlockIndex->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(labelBlockIndex, 0, 2, 1, 1);

        buttonModify = new QPushButton(widget_2);
        buttonModify->setObjectName(QString::fromUtf8("buttonModify"));
        sizePolicy2.setHeightForWidth(buttonModify->sizePolicy().hasHeightForWidth());
        buttonModify->setSizePolicy(sizePolicy2);
        buttonModify->setFont(font1);

        gridLayout_3->addWidget(buttonModify, 6, 1, 1, 1);

        label_8 = new QLabel(widget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setFrameShape(QFrame::Box);
        label_8->setFrameShadow(QFrame::Raised);
        label_8->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_8, 5, 1, 1, 1);

        label_6 = new QLabel(widget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFrameShape(QFrame::Box);
        label_6->setFrameShadow(QFrame::Raised);
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_6, 4, 1, 1, 1);

        label_7 = new QLabel(widget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFrameShape(QFrame::Box);
        label_7->setFrameShadow(QFrame::Raised);
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_7, 3, 1, 1, 1);

        label_5 = new QLabel(widget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFrameShape(QFrame::Box);
        label_5->setFrameShadow(QFrame::Raised);
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_5, 2, 1, 1, 1);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font2);
        label_3->setFrameShape(QFrame::Box);
        label_3->setFrameShadow(QFrame::Raised);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_3, 1, 1, 1, 1);

        label = new QLabel(widget_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font2);
        label->setFrameShape(QFrame::Box);
        label->setFrameShadow(QFrame::Raised);
        label->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label, 0, 1, 1, 1);

        gridLayout_3->setRowStretch(0, 2);
        gridLayout_3->setRowStretch(1, 2);
        gridLayout_3->setRowStretch(2, 1);
        gridLayout_3->setRowStretch(3, 1);
        gridLayout_3->setRowStretch(4, 1);
        gridLayout_3->setRowStretch(5, 1);
        gridLayout_3->setRowStretch(6, 2);
        gridLayout_3->setColumnStretch(1, 1);
        gridLayout_3->setColumnStretch(2, 1);

        gridLayout->addWidget(widget_2, 0, 1, 1, 1);

        gridLayout->setColumnStretch(0, 6);
        gridLayout->setColumnStretch(1, 4);

        retranslateUi(ProcessResultDisplayClass);

        QMetaObject::connectSlotsByName(ProcessResultDisplayClass);
    } // setupUi

    void retranslateUi(QWidget *ProcessResultDisplayClass)
    {
        ProcessResultDisplayClass->setWindowTitle(QApplication::translate("ProcessResultDisplayClass", "\346\225\210\346\236\234\346\230\276\347\244\272", nullptr));
        labelDisplay->setText(QString());
        lineEditX1->setText(QApplication::translate("ProcessResultDisplayClass", "0", nullptr));
        lineEditY1->setText(QApplication::translate("ProcessResultDisplayClass", "0", nullptr));
        lineEditX2->setText(QApplication::translate("ProcessResultDisplayClass", "0", nullptr));
        lineEditY2->setText(QApplication::translate("ProcessResultDisplayClass", "0", nullptr));
        buttonSure->setText(QApplication::translate("ProcessResultDisplayClass", "\347\241\256\350\256\244", nullptr));
        labelDetectCate->setText(QApplication::translate("ProcessResultDisplayClass", "/", nullptr));
        labelBlockIndex->setText(QApplication::translate("ProcessResultDisplayClass", "0", nullptr));
        buttonModify->setText(QApplication::translate("ProcessResultDisplayClass", "\344\277\256\346\224\271", nullptr));
        label_8->setText(QApplication::translate("ProcessResultDisplayClass", "<html><head/><body><p><span style=\" font-size:16pt;\">Y2\357\274\232</span></p></body></html>", nullptr));
        label_6->setText(QApplication::translate("ProcessResultDisplayClass", "<html><head/><body><p><span style=\" font-size:16pt;\">X2\357\274\232</span></p></body></html>", nullptr));
        label_7->setText(QApplication::translate("ProcessResultDisplayClass", "<html><head/><body><p><span style=\" font-size:16pt;\">Y1\357\274\232</span></p></body></html>", nullptr));
        label_5->setText(QApplication::translate("ProcessResultDisplayClass", "<html><head/><body><p><span style=\" font-size:16pt;\">X1\357\274\232</span></p></body></html>", nullptr));
        label_3->setText(QApplication::translate("ProcessResultDisplayClass", "\346\243\200\346\265\213\357\274\232", nullptr));
        label->setText(QApplication::translate("ProcessResultDisplayClass", "\347\274\226\345\217\267\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProcessResultDisplayClass: public Ui_ProcessResultDisplayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSDISPLAY_H
