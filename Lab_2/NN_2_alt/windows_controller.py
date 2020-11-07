from typing import List, Optional
from PySide2.QtCore import QObject, Slot
from PySide2.QtWidgets import QWidget

class WindowsController(QObject):
    __controller__: Optional['WindowsController'] = None

    def __init__(self):
        self.__windows__: List[QWidget] = []

    @Slot(QWidget)
    def __remove_window__(self, widget: QWidget):
        self.__windows__.remove(widget)

    @staticmethod
    def controller():
        if WindowsController.__controller__ == None:
            WindowsController.__controller__ = WindowsController()
        return WindowsController.__controller__
    
    def open_window(self, widget: QWidget):
        widget.show()
        self.__windows__.append(widget)
        widget.windows_closed.connect(self.__remove_window__)
