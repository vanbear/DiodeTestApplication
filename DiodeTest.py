from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import QThread, pyqtSignal, pyqtSlot
import sys
import os
import serial
import serial.tools.list_ports
import warnings

global curdir, serialPort
curdir = os.path.dirname(os.path.abspath(__file__))

class DataTransferThread(QThread):
    signalStatusBarUpdate = pyqtSignal('QString')

    def __init__(self, guiApp):
        QThread.__init__(self)
        self.guiApp = guiApp
        self.getPort()
        
    def __del__(self):
        if hasattr(self, 'port'):
            self.port.close()
        self.wait()

    def run(self):
        while True:
            if self.isPortAvailable:
                try:
                    self.guiApp.setStatusBarStyleSheet_Normal()
                    data = self.port.readline()
                    if (data):
                        self.signalStatusBarUpdate.emit(data.decode())
                        self.parseMessage(data)
                except serial.SerialException:
                    self.isPortAvailable = False
                    pass
            else:
                self.guiApp.setStatusBarStyleSheet_Error()
                self.signalStatusBarUpdate.emit("ARDUINO NOT CONNECTED!")
                self.getPort()

    def parseMessage(self, msg):
        valDict = {}
        msgRaw = msg[msg.find(b"{")+1 : msg.find(b"}")]
        [msgName, msgValues] = msgRaw.split(b":")
        if msgName.decode()=='Data':
            valNames = ['stepA', 'stepB', 'dirA', 'dirB', 'lightVal']
            valList = msgValues.split(b";")
            for i in range(len(valList)):
                valDict[valNames[i]] = valList[i].decode()
            print(valDict)

    def getPort(self):
        try:
            self.port = getArduinoPort()
            self.isPortAvailable = True 
        except IOError:
            self.isPortAvailable = False
            pass

class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(ApplicationWindow, self).__init__()
        self.ui = uic.loadUi(os.path.join(curdir, 'DiodeTestGUI/mainwindow.ui'), self)
        self.setupTable()
        # == thread
        self.dataThread = DataTransferThread(self)
        self.dataThread.signalStatusBarUpdate.connect(self.updateStatusBar)
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

    @pyqtSlot('QString')
    def updateStatusBar(self, value):
        self.ui.statusBar.showMessage(str(value))

    def setStatusBarStyleSheet_Error(self):
        self.ui.statusBar.setStyleSheet(
            'color: rgb(255,0,0);'
            'font-weight: bold;'
        )

    def setStatusBarStyleSheet_Normal(self):
        self.ui.statusBar.setStyleSheet(
            'color: rgb(0,0,0);'
            'font-weight: normal;'
        ) 

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

    # === END ===
    app.exec_()

if __name__ == "__main__":
    main()