import sys
from PySide2.QtWidgets import QApplication
from main_window import MainWindow
from windows_controller import WindowsController

if __name__ == "__main__":
    app = QApplication()
    win = MainWindow()
    
    WindowsController.controller().open_window(win)
    
    sys.exit(app.exec_())
