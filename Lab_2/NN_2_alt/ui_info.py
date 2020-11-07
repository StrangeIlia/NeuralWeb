# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'info.ui'
##
## Created by: Qt User Interface Compiler version 5.15.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_Info(object):
    def setupUi(self, Info):
        if not Info.objectName():
            Info.setObjectName(u"Info")
        Info.resize(400, 300)
        self.verticalLayout = QVBoxLayout(Info)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.console = QTextEdit(Info)
        self.console.setObjectName(u"console")

        self.verticalLayout.addWidget(self.console)


        self.retranslateUi(Info)

        QMetaObject.connectSlotsByName(Info)
    # setupUi

    def retranslateUi(self, Info):
        Info.setWindowTitle(QCoreApplication.translate("Info", u"\u0421\u043b\u0443\u0436\u0435\u0431\u043d\u0430\u044f \u0438\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044f", None))
    # retranslateUi

