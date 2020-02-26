/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "qchartview.h"
QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *label;
    QComboBox *portNameBox;
    QLabel *label_2;
    QComboBox *baudrateBox;
    QPushButton *pushButton;
    QChartView *widget;
    QPushButton *pushButton_2;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(650, 700);
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 30, 51, 25));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(9);
        label->setFont(font);
        portNameBox = new QComboBox(Widget);
        portNameBox->setObjectName(QStringLiteral("portNameBox"));
        portNameBox->setEnabled(true);
        portNameBox->setGeometry(QRect(80, 30, 71, 25));
        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(180, 30, 51, 25));
        baudrateBox = new QComboBox(Widget);
        baudrateBox->setObjectName(QStringLiteral("baudrateBox"));
        baudrateBox->setGeometry(QRect(230, 30, 91, 25));
        baudrateBox->setEditable(true);
        pushButton = new QPushButton(Widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(350, 29, 71, 27));
        widget = new QChartView(Widget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 60, 630, 630));
        widget->setMouseTracking(true);
        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(460, 29, 71, 27));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", Q_NULLPTR));
        label->setText(QApplication::translate("Widget", "\347\253\257\345\217\243\345\217\267", Q_NULLPTR));
        label_2->setText(QApplication::translate("Widget", "\346\263\242\347\211\271\347\216\207", Q_NULLPTR));
        baudrateBox->clear();
        baudrateBox->insertItems(0, QStringList()
         << QApplication::translate("Widget", "1200", Q_NULLPTR)
         << QApplication::translate("Widget", "4800", Q_NULLPTR)
         << QApplication::translate("Widget", "9600", Q_NULLPTR)
         << QApplication::translate("Widget", "38400", Q_NULLPTR)
         << QApplication::translate("Widget", "115200", Q_NULLPTR)
         << QApplication::translate("Widget", "256000", Q_NULLPTR)
         << QApplication::translate("Widget", "500000", Q_NULLPTR)
        );
        pushButton->setText(QApplication::translate("Widget", "\346\211\223\345\274\200\344\270\262\345\217\243", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("Widget", "\346\270\205\351\231\244\347\254\224\350\277\271", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
