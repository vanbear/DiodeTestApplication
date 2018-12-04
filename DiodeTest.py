import os
import sys
import time

import matplotlib.pyplot as plt
import numpy as np
import serial
import serial.tools.list_ports
from matplotlib.backends.backend_qt5agg import \
    FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import QThread, pyqtSignal, pyqtSlot
from PyQt5.QtWidgets import QSizePolicy

#=========== GLOBAL VARIABLES
curdir = os.path.dirname(os.path.abspath(__file__))
startingNames = ["StartingPoint", "maxStepsA", "maxStepsB"]
valueNames = ["stepA", "stepB", "dirA", "dirB", "value"]
overallSteps = 0
madeSteps = 0
acquiredData = []
previousMilis = 0
plotInterval = 3000


#=========== DATA TRANSFER
class DataTransferThread(QThread):
    signalStatusBarUpdate = pyqtSignal('QString')
    signalStatusBarStyleNormal = pyqtSignal()
    signalStatusBarStyleError = pyqtSignal()
    signalAddValuesToTable = pyqtSignal(list)
    signalStateLabelSetText = pyqtSignal('QString')
    signalUpdateProgressBar = pyqtSignal(int)
    signalUpdatePlot = pyqtSignal(list)

    def __init__(self):
        QThread.__init__(self)
        self.getPort()
        
    def __del__(self):
        if hasattr(self, 'port'):
            self.port.close()
        self.wait()

    def run(self):
        global overallSteps, madeSteps, acquiredData
        overallSteps = 0
        madeSteps = 0
        acquiredData = []
        while True:
            if self.isPortAvailable:
                try:
                    self.signalStatusBarStyleNormal.emit()
                    data = self.port.readline()
                    if (data):
                        self.signalStatusBarUpdate.emit(data.decode())
                        self.interpretMessage(data)
                except serial.SerialException:
                    self.isPortAvailable = False
                    pass
            else:
                self.signalStatusBarStyleError.emit()
                self.signalStatusBarUpdate.emit("ARDUINO NOT CONNECTED!")
                self.getPort()

    def interpretMessage(self, msg):
        msgRaw = msg[msg.find(b"{")+1 : msg.find(b"}")]
        [msgName, msgValues] = msgRaw.split(b":")
        if msgName==b'CalibrationStart':
            self.signalStateLabelSetText.emit('Calibrating')
        elif msgName==b'MeasureStart':
            self.parseStartingData(msgValues)
        elif msgName==b'Data':
            self.parseData(msgValues)
        elif msgName==b'MeasureEnd':
            self.signalStateLabelSetText.emit('Measurement completed')
        elif msgName==b'Error':
            self.parseError(msgValues)

    def parseStartingData(self, data):
        global overallSteps, acquiredData
        valuesList = data.split(b";")
        [startingPoint, maxA, maxB] = valuesList
        overallSteps = int(maxA)*int(maxB)
        self.signalStateLabelSetText.emit('Starting at point ' + startingPoint.decode() + ' with ' + str(overallSteps) + ' overall steps')
        

    def parseData(self, data):
        global madeSteps, previousMilis, plotInterval
        self.signalStateLabelSetText.emit('Measuring (' + str(madeSteps) + ' / ' + str(overallSteps) + ')')
        valuesList = data.split(b";")
        madeSteps = madeSteps + 1
        self.signalUpdateProgressBar.emit(int(madeSteps/800*100))
        self.signalAddValuesToTable.emit(valuesList)
        acquiredData.append(valuesList[-1].decode().replace('\r', ''))
        currentMilis = getTime()
        if (currentMilis - previousMilis > plotInterval):
            previousMilis = currentMilis
            self.signalUpdatePlot.emit(list(map(int, acquiredData)))

    def parseError(self, data):
        self.signalStateLabelSetText.emit(data.decode())

    def getPort(self):
        try:
            self.port = getArduinoPort()
            self.isPortAvailable = True
        except IOError:
            self.isPortAvailable = False
            pass

#=========== PLOT CANVAS
class PlotCanvas(FigureCanvas):
    def __init__(self, parent=None, width = 5, height = 4, dpi = 100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
 
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)
 
        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        self.plot([])

    def plot(self, data):
        self.axes.clear()
        self.axes.plot(data, 'r-')
        self.draw()

#=========== MAIN APP
class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(ApplicationWindow, self).__init__()
        self.setFixedSize(890,560)

        self.ui = uic.loadUi(os.path.join(curdir, 'DiodeTestGUI/mainwindow.ui'), self)
        self.setupTable()
        self.plotCanvas = PlotCanvas(self.plotView)
        self.plotCanvas.move(0,0)
        # == thread
        self.dataThread = DataTransferThread()
        self.setupSignals()
        self.dataThread.start()

    def setupSignals(self):
        self.dataThread.signalStatusBarUpdate.connect(self.updateStatusBar)
        self.dataThread.signalStatusBarStyleError.connect(self.setStatusBarStyleSheet_Error)
        self.dataThread.signalStatusBarStyleNormal.connect(self.setStatusBarStyleSheet_Normal)
        self.dataThread.signalAddValuesToTable.connect(self.addTableItem)
        self.dataThread.signalStateLabelSetText.connect(self.setStateLabelText)
        self.dataThread.signalUpdateProgressBar.connect(self.updateProgressBar)
        self.dataThread.signalUpdatePlot.connect(self.updatePlot)

    def setupTable(self):
        columnCount = 5
        table = self.ui.dataTable
        table.setColumnCount(columnCount)

        table.setHorizontalHeaderLabels(valueNames)
        for i in range(columnCount):
            table.setColumnWidth(i, 40)     

    @pyqtSlot(list)
    def addTableItem(self, values):
        table = self.ui.dataTable
        table.insertRow(0)
        for i in range(len(values)):
            table.setItem(0, i, QtWidgets.QTableWidgetItem(values[i].decode()))

    @pyqtSlot(int)
    def updateProgressBar(self, value):
        self.ui.progressBar.setValue(value)

    @pyqtSlot('QString')
    def updateStatusBar(self, value):
        self.ui.statusBar.showMessage(str(value))

    @pyqtSlot()
    def setStatusBarStyleSheet_Error(self):
        self.ui.statusBar.setStyleSheet(
            'color: rgb(255,0,0);'
            'font-weight: bold;'
        )

    @pyqtSlot()
    def setStatusBarStyleSheet_Normal(self):
        self.ui.statusBar.setStyleSheet(
            'color: rgb(0,0,0);'
            'font-weight: normal;'
        ) 

    @pyqtSlot('QString')
    def setStateLabelText(self, value):
        self.ui.stateLabel.setText(value)

    @pyqtSlot(list)
    def updatePlot(self, values):
        self.plotCanvas.plot(values)

#=========== GLOBAL METHODS
def getArduinoPort():
    arduino_ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'Arduino' in p.description
    ]
    if not arduino_ports:
        raise IOError("No Arduino found")

    return serial.Serial(arduino_ports[0], 9600)

def getTime():
    return int(round(time.time() * 1000))

def main():
    # === INIT ===
    app = QtWidgets.QApplication(sys.argv)
    form = ApplicationWindow()
    form.show()

    # === MAIN LOOP ===
    form.updateProgressBar(0)

    # === END ===
    app.exec_()

if __name__ == "__main__":
    main()
