from typing import Optional
from PySide2.QtGui import QCloseEvent
from PySide2.QtWidgets import QWidget
from PySide2.QtCore import Signal, Slot
from ui_info import Ui_Info

class Info(QWidget):
    windows_closed = Signal(QWidget)

    def __init__(self, parent: Optional[QWidget] = None):
        super(Info, self).__init__(parent)
        self.ui = Ui_Info()
        self.ui.setupUi(self)
    
    def closeEvent(self, event: QCloseEvent):
        self.windows_closed.emit(self)

    @Slot(str)
    def print_html(self, html: str):
        self.ui.console.setHtml(html)
