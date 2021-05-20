/********************************************************************************
** Form generated from reading UI file 'Statistics.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATISTICS_H
#define UI_STATISTICS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StatisticClass
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QComboBox *comboBoxName;
    QSpacerItem *horizontalSpacer;
    QTableWidget *tableWidgetMsg;

    void setupUi(QWidget *StatisticClass)
    {
        if (StatisticClass->objectName().isEmpty())
            StatisticClass->setObjectName(QString::fromUtf8("StatisticClass"));
        StatisticClass->resize(600, 400);
        gridLayout = new QGridLayout(StatisticClass);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget = new QWidget(StatisticClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        comboBoxName = new QComboBox(widget);
        comboBoxName->setObjectName(QString::fromUtf8("comboBoxName"));

        gridLayout_2->addWidget(comboBoxName, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(383, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 2, 1, 1);

        tableWidgetMsg = new QTableWidget(widget);
        if (tableWidgetMsg->columnCount() < 3)
            tableWidgetMsg->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetMsg->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetMsg->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetMsg->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidgetMsg->setObjectName(QString::fromUtf8("tableWidgetMsg"));
        tableWidgetMsg->setAcceptDrops(false);
        tableWidgetMsg->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetMsg->horizontalHeader()->setVisible(true);
        tableWidgetMsg->horizontalHeader()->setHighlightSections(false);
        tableWidgetMsg->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableWidgetMsg->horizontalHeader()->setStretchLastSection(false);
        tableWidgetMsg->verticalHeader()->setCascadingSectionResizes(false);
        tableWidgetMsg->verticalHeader()->setProperty("showSortIndicator", QVariant(false));

        gridLayout_2->addWidget(tableWidgetMsg, 1, 0, 1, 3);

        gridLayout_2->setColumnStretch(0, 1);
        gridLayout_2->setColumnStretch(1, 2);
        gridLayout_2->setColumnStretch(2, 7);

        gridLayout->addWidget(widget, 0, 0, 1, 1);


        retranslateUi(StatisticClass);

        QMetaObject::connectSlotsByName(StatisticClass);
    } // setupUi

    void retranslateUi(QWidget *StatisticClass)
    {
        StatisticClass->setWindowTitle(QApplication::translate("StatisticClass", "\347\273\237\350\256\241", nullptr));
        label->setText(QApplication::translate("StatisticClass", "\345\236\213\345\217\267\357\274\232", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetMsg->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("StatisticClass", "\346\227\266\351\227\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetMsg->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("StatisticClass", "\345\236\213\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetMsg->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("StatisticClass", "\344\277\241\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StatisticClass: public Ui_StatisticClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATISTICS_H
