from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import QThread
import sys
import os
import serial
import serial.tools.list_ports
import warnings

curdir = os.path.dirname(os.path.abspath(__file__))

class DataTransferThread(QThread):
    def __init__(self, guiApp):
        QThread.__init__(self)
        self.port = getArduinoPort()
        self.guiApp = guiApp

    def __del__(self):
        self.port.close()
        self.wait()

    def run(self):
        while(True):
            self.guiApp.updateStatusBar(self.port.readline())


class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(ApplicationWindow, self).__init__()
        self.ui = uic.loadUi(os.path.join(curdir, 'DiodeTestGUI/mainwindow.ui'), self)
        self.setupTable()
        self.dataThread = DataTransferThread(self)
        self.dataThread.start()

    def setupTable(self):
        columnCount = 5
        table = self.ui.dataTable
        table.setColumnCount(columnCount)

        table.setHorizontalHeaderLabels(["stepA", "stepB", "dirA", "dirB", "value"])
        for i in range(columnCount):
            table.setColumnWidth(i, 47)        

    def addTableItem(self, stepA, stepB, dirA, dirB, lightValue):
        table = self.ui.dataTable
        table.insertRow(0)
        table.setItem(0, 0, QtWidgets.QTableWidgetItem(str(stepA)))
        table.setItem(0, 1, QtWidgets.QTableWidgetItem(str(stepB)))
        table.setItem(0, 2, QtWidgets.QTableWidgetItem(str(dirA)))
        table.setItem(0, 3, QtWidgets.QTableWidgetItem(str(dirB)))
        table.setItem(0, 4, QtWidgets.QTableWidgetItem(str(lightValue)))

    def updateProgressBar(self, value):
        self.ui.progressBar.setValue(value)

    def updateStatusBar(self, value):
        self.ui.statusBar.showMessage(str(value))

    def done(self):
        self.ui.statusBar.showMessage(str("Done!"))


def getArduinoPort():
    arduino_ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'Arduino' in p.description
    ]
    if not arduino_ports:
        raise IOError("No Arduino found")
    if len(arduino_ports) > 1:
        warnings.warn('Multiple Arduinos found - using the first')

    return serial.Serial(arduino_ports[0], 9600)

def main():
    # === INIT ===
    app = QtWidgets.QApplication(sys.argv)
    form = ApplicationWindow()
    form.show()

    # === MAIN LOOP ===
    form.updateProgressBar(50)
    form.addTableItem(1, 1, 0, 0, 52)
    
    # while(True):
    #     form.updateStatusBar(str(port.readline()))
    #     form.update()

    # === END ===
    app.exec_()

if __name__ == "__main__":
    main()