# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainwindow.ui'
##
## Created by: Qt User Interface Compiler version 5.15.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(904, 600)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout_6 = QHBoxLayout(self.centralwidget)
        self.horizontalLayout_6.setObjectName(u"horizontalLayout_6")
        self.widget_9 = QWidget(self.centralwidget)
        self.widget_9.setObjectName(u"widget_9")
        self.verticalLayout_4 = QVBoxLayout(self.widget_9)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.imageInfo = QLabel(self.widget_9)
        self.imageInfo.setObjectName(u"imageInfo")

        self.verticalLayout_4.addWidget(self.imageInfo)

        self.table = QTableWidget(self.widget_9)
        if (self.table.columnCount() < 5):
            self.table.setColumnCount(5)
        if (self.table.rowCount() < 5):
            self.table.setRowCount(5)
        self.table.setObjectName(u"table")
        self.table.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.table.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.table.setSizeAdjustPolicy(QAbstractScrollArea.AdjustIgnored)
        self.table.setEditTriggers(QAbstractItemView.AnyKeyPressed|QAbstractItemView.EditKeyPressed|QAbstractItemView.SelectedClicked)
        self.table.setSelectionMode(QAbstractItemView.SingleSelection)
        self.table.setVerticalScrollMode(QAbstractItemView.ScrollPerItem)
        self.table.setRowCount(5)
        self.table.setColumnCount(5)
        self.table.horizontalHeader().setVisible(False)
        self.table.horizontalHeader().setCascadingSectionResizes(False)
        self.table.horizontalHeader().setStretchLastSection(False)
        self.table.verticalHeader().setVisible(False)
        self.table.verticalHeader().setStretchLastSection(False)

        self.verticalLayout_4.addWidget(self.table)

        self.widget_10 = QWidget(self.widget_9)
        self.widget_10.setObjectName(u"widget_10")
        self.horizontalLayout_7 = QHBoxLayout(self.widget_10)
        self.horizontalLayout_7.setObjectName(u"horizontalLayout_7")
        self.clear = QPushButton(self.widget_10)
        self.clear.setObjectName(u"clear")

        self.horizontalLayout_7.addWidget(self.clear)

        self.setImageSize = QPushButton(self.widget_10)
        self.setImageSize.setObjectName(u"setImageSize")

        self.horizontalLayout_7.addWidget(self.setImageSize)


        self.verticalLayout_4.addWidget(self.widget_10)


        self.horizontalLayout_6.addWidget(self.widget_9)

        self.widget_7 = QWidget(self.centralwidget)
        self.widget_7.setObjectName(u"widget_7")
        self.verticalLayout_3 = QVBoxLayout(self.widget_7)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.widget_6 = QWidget(self.widget_7)
        self.widget_6.setObjectName(u"widget_6")
        self.verticalLayout_2 = QVBoxLayout(self.widget_6)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.widget_2 = QWidget(self.widget_6)
        self.widget_2.setObjectName(u"widget_2")
        self.horizontalLayout = QHBoxLayout(self.widget_2)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label = QLabel(self.widget_2)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.imageGroup = QComboBox(self.widget_2)
        self.imageGroup.setObjectName(u"imageGroup")
        self.imageGroup.setEditable(True)
        self.imageGroup.setInsertPolicy(QComboBox.InsertAtCurrent)
        self.imageGroup.setSizeAdjustPolicy(QComboBox.AdjustToMinimumContentsLengthWithIcon)

        self.horizontalLayout.addWidget(self.imageGroup)


        self.verticalLayout_2.addWidget(self.widget_2)

        self.widget_4 = QWidget(self.widget_6)
        self.widget_4.setObjectName(u"widget_4")
        self.horizontalLayout_4 = QHBoxLayout(self.widget_4)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.addGroup = QPushButton(self.widget_4)
        self.addGroup.setObjectName(u"addGroup")

        self.horizontalLayout_4.addWidget(self.addGroup)

        self.removeGroup = QPushButton(self.widget_4)
        self.removeGroup.setObjectName(u"removeGroup")

        self.horizontalLayout_4.addWidget(self.removeGroup)


        self.verticalLayout_2.addWidget(self.widget_4)


        self.verticalLayout_3.addWidget(self.widget_6)

        self.widget_3 = QWidget(self.widget_7)
        self.widget_3.setObjectName(u"widget_3")
        self.verticalLayout = QVBoxLayout(self.widget_3)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.widget_5 = QWidget(self.widget_3)
        self.widget_5.setObjectName(u"widget_5")
        self.horizontalLayout_3 = QHBoxLayout(self.widget_5)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.label_5 = QLabel(self.widget_5)
        self.label_5.setObjectName(u"label_5")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy)

        self.horizontalLayout_3.addWidget(self.label_5)

        self.image = QComboBox(self.widget_5)
        self.image.setObjectName(u"image")
        self.image.setEditable(True)
        self.image.setInsertPolicy(QComboBox.InsertAtCurrent)
        self.image.setSizeAdjustPolicy(QComboBox.AdjustToMinimumContentsLengthWithIcon)

        self.horizontalLayout_3.addWidget(self.image)


        self.verticalLayout.addWidget(self.widget_5)

        self.widget = QWidget(self.widget_3)
        self.widget.setObjectName(u"widget")
        self.horizontalLayout_2 = QHBoxLayout(self.widget)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.saveImage = QPushButton(self.widget)
        self.saveImage.setObjectName(u"saveImage")

        self.horizontalLayout_2.addWidget(self.saveImage)

        self.addImage = QPushButton(self.widget)
        self.addImage.setObjectName(u"addImage")

        self.horizontalLayout_2.addWidget(self.addImage)

        self.removeImage = QPushButton(self.widget)
        self.removeImage.setObjectName(u"removeImage")

        self.horizontalLayout_2.addWidget(self.removeImage)


        self.verticalLayout.addWidget(self.widget)


        self.verticalLayout_3.addWidget(self.widget_3)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer_2)

        self.widget_13 = QWidget(self.widget_7)
        self.widget_13.setObjectName(u"widget_13")
        self.horizontalLayout_9 = QHBoxLayout(self.widget_13)
        self.horizontalLayout_9.setObjectName(u"horizontalLayout_9")
        self.label_2 = QLabel(self.widget_13)
        self.label_2.setObjectName(u"label_2")
        self.label_2.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.horizontalLayout_9.addWidget(self.label_2)

        self.learningFactor = QDoubleSpinBox(self.widget_13)
        self.learningFactor.setObjectName(u"learningFactor")
        sizePolicy1 = QSizePolicy(QSizePolicy.Maximum, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.learningFactor.sizePolicy().hasHeightForWidth())
        self.learningFactor.setSizePolicy(sizePolicy1)
        self.learningFactor.setMinimum(0.010000000000000)
        self.learningFactor.setMaximum(1.000000000000000)
        self.learningFactor.setSingleStep(0.100000000000000)
        self.learningFactor.setValue(1.000000000000000)

        self.horizontalLayout_9.addWidget(self.learningFactor)


        self.verticalLayout_3.addWidget(self.widget_13)

        self.result = QLabel(self.widget_7)
        self.result.setObjectName(u"result")

        self.verticalLayout_3.addWidget(self.result)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer)

        self.widget_12 = QWidget(self.widget_7)
        self.widget_12.setObjectName(u"widget_12")
        self.verticalLayout_6 = QVBoxLayout(self.widget_12)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.widget_8 = QWidget(self.widget_12)
        self.widget_8.setObjectName(u"widget_8")
        self.horizontalLayout_5 = QHBoxLayout(self.widget_8)
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.horizontalLayout_5.setContentsMargins(9, -1, 9, -1)
        self.learning = QPushButton(self.widget_8)
        self.learning.setObjectName(u"learning")

        self.horizontalLayout_5.addWidget(self.learning)

        self.recognizeImage = QPushButton(self.widget_8)
        self.recognizeImage.setObjectName(u"recognizeImage")

        self.horizontalLayout_5.addWidget(self.recognizeImage)


        self.verticalLayout_6.addWidget(self.widget_8)

        self.checkBox = QCheckBox(self.widget_12)
        self.checkBox.setObjectName(u"checkBox")
        self.checkBox.setLayoutDirection(Qt.RightToLeft)

        self.verticalLayout_6.addWidget(self.checkBox)


        self.verticalLayout_3.addWidget(self.widget_12)


        self.horizontalLayout_6.addWidget(self.widget_7)

        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"\u0411\u043e\u0439\u0447\u0443\u043a \u0418\u043b\u044c\u044f. \u041d\u0435\u0439\u0440\u043e\u043d\u043d\u044b\u0435 \u0441\u0435\u0442\u0438. \u041b\u0430\u0431 3.", None))
        self.imageInfo.setText("")
        self.clear.setText(QCoreApplication.translate("MainWindow", u"\u041e\u0447\u0438\u0441\u0442\u0438\u0442\u044c", None))
        self.setImageSize.setText(QCoreApplication.translate("MainWindow", u"\u0417\u0430\u0434\u0430\u0442\u044c \u0440\u0430\u0437\u043c\u0435\u0440\u044b \u0438\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0439", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"\u0413\u0440\u0443\u043f\u043f\u0430 \u0438\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0439", None))
        self.addGroup.setText(QCoreApplication.translate("MainWindow", u"\u0414\u043e\u0431\u0430\u0432\u0438\u0442\u044c \u0433\u0440\u0443\u043f\u043f\u0443", None))
        self.removeGroup.setText(QCoreApplication.translate("MainWindow", u"\u0423\u0434\u0430\u043b\u0438\u0442\u044c \u0433\u0440\u0443\u043f\u043f\u0443", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"\u0418\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0435", None))
        self.saveImage.setText(QCoreApplication.translate("MainWindow", u"\u0417\u0430\u043f\u043e\u043c\u043d\u0438\u0442\u044c \u0438\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0435", None))
        self.addImage.setText(QCoreApplication.translate("MainWindow", u"\u0414\u043e\u0431\u0430\u0432\u0438\u0442\u044c \u0438\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0435", None))
        self.removeImage.setText(QCoreApplication.translate("MainWindow", u"\u0423\u0434\u0430\u043b\u0438\u0442\u044c \u0438\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0435", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"\u041a\u043e\u044d\u0444\u0444\u0438\u0446\u0438\u0435\u043d\u0442 \u043e\u0431\u0443\u0447\u0435\u043d\u0438\u044f", None))
        self.result.setText("")
        self.learning.setText(QCoreApplication.translate("MainWindow", u"\u041e\u0431\u0443\u0447\u0438\u0442\u044c \u043d\u0435\u0439\u0440\u043e\u043d\u043d\u0443\u044e \u0441\u0435\u0442\u044c", None))
        self.recognizeImage.setText(QCoreApplication.translate("MainWindow", u"\u0420\u0430\u0441\u043f\u043e\u0437\u043d\u0430\u0442\u044c \u0438\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u0435", None))
        self.checkBox.setText(QCoreApplication.translate("MainWindow", u"\u0412\u044b\u0432\u043e\u0434\u0438\u0442\u044c \u0441\u043b\u0443\u0436\u0435\u0431\u043d\u0443\u044e \u0438\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044e", None))
    # retranslateUi

