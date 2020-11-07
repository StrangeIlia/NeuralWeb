# This Python file uses the following encoding: utf-8
from typing import Optional

from PySide2.QtGui import QCloseEvent
from windows_controller import WindowsController
from info import Info
from PySide2.QtCore import SIGNAL, Signal
from PySide2.QtWidgets import QMainWindow, QWidget
from ui_mainwindow import Ui_MainWindow

class MainWindow(QMainWindow):
    html_sended = Signal(str)
    windows_closed = Signal(QWidget)

    def __init__(self, parent: Optional[QWidget] = None):
        super(MainWindow, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)     

    def closeEvent(self, event: QCloseEvent):
        self.windows_closed.emit(self)

    def __create_reciever__(self):
        # Немного странно, но ладно
        count_receivers = self.receivers(SIGNAL('html_sended(QString)'))
        if count_receivers == 0:
            info = Info()
            self.html_sended.connect(info.print_html)
            WindowsController.controller().open_window(info)

    def print_html(self, html: str):
        self.__create_reciever__()
        self.html_sended.emit(html)
