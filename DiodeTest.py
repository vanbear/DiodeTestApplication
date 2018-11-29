from PyQt5 import QtWidgets
from mainwindow import Ui_MainWindow
import sys

class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(ApplicationWindow, self).__init__()

        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

    def asasasd(self):
        self.progressBar()


def main():
    app = QtWidgets.QApplication(sys.argv)
    application = ApplicationWindow()
    application.show()
    application.asasasd()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()