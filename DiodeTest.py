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
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QSizePolicy

#=========== GLOBAL VARIABLES
windowHeight, windowWidth = [890, 580]
curdir = os.path.dirname(os.path.abspath(__file__))
startingNames = ["StartingPoint", "maxStepsA", "maxStepsB"]
valueNames = ["stepA", "stepB", "dirA", "dirB", "value"]
overallSteps = 0
madeSteps = 0
maxA = 0
maxB = 0
acquiredData = []
acquiredData2D = []
previousMilis = 0
plotInterval = 1000
arduinoPort = serial.Serial()
measuringStarted = False


#=========== DATA TRANSFER
class DataTransferThread(QThread):
    signalStatusBarUpdate = pyqtSignal('QString')
    signalStatusBarStyleNormal = pyqtSignal()
    signalStatusBarStyleError = pyqtSignal()
    signalAddValuesToTable = pyqtSignal(list)
    signalStateLabelSetText = pyqtSignal('QString')
    signalUpdateProgressBar = pyqtSignal(int)
    signalUpdateLinearPlot = pyqtSignal(list)
    signalUpdateSquarePlot = pyqtSignal(np.ndarray)

    def __init__(self):
        QThread.__init__(self)
        self.getPort()
        
    def __del__(self):
        if arduinoPort.isOpen():
            arduinoPort.close()
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
                    data = arduinoPort.readline()
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
        global measuringStarted
        msgRaw = msg[msg.find(b"{")+1 : msg.find(b"}")]
        try:
            [msgName, msgValues] = msgRaw.split(b":")
        except ValueError:
            self.signalStatusBarStyleError.emit()
            self.signalStatusBarUpdate.emit("Received invalid message.")
        else:
            if msgName==b'CalibrationStart':
                self.signalStateLabelSetText.emit('Calibrating')
            elif msgName==b'MeasureStart':
                measuringStarted = True
                self.parseStartingData(msgValues)
            elif msgName==b'Data' and measuringStarted == True:
                self.parseData(msgValues)
            elif msgName==b'MeasureEnd':
                self.signalStateLabelSetText.emit('Measurement completed')
            elif msgName==b'Error':
                self.parseError(msgValues)

    def parseStartingData(self, data):
        global overallSteps, acquiredData, acquiredData2D, maxA, maxB
        valuesList = data.split(b";")
        [startingPoint, maxA, maxB] = valuesList
        acquiredData2D = np.zeros((int(maxA), int(maxB)))
        overallSteps = int(maxA)*int(maxB)
        self.signalStateLabelSetText.emit('Starting at point ' + startingPoint.decode() + ' with ' + str(overallSteps) + ' overall steps')
        

    def parseData(self, data):
        global madeSteps
        self.signalStateLabelSetText.emit('Measuring (' + str(madeSteps) + ' / ' + str(overallSteps) + ')')
        valuesList = data.split(b";")
        madeSteps = madeSteps + 1
        self.signalUpdateProgressBar.emit(int(madeSteps/800*100))
        self.signalAddValuesToTable.emit(valuesList)
        dataToAppend = int(valuesList[-1].decode().replace('\r', ''))
        acquiredData.append(dataToAppend)
        acquiredData2D[int(valuesList[0])][int(valuesList[1])] = dataToAppend
        self.initiatePlots()

    def initiatePlots(self):
        global previousMilis, plotInterval
        acquiredData2Drotated = np.rot90(acquiredData2D, 3)
        currentMilis = getTime()
        if (currentMilis - previousMilis > plotInterval):
            previousMilis = currentMilis
            self.signalUpdateLinearPlot.emit(acquiredData)
            self.signalUpdateSquarePlot.emit(acquiredData2Drotated)

    def parseError(self, data):
        self.signalStateLabelSetText.emit(data.decode())

    def getPort(self):
        global arduinoPort
        try:
            arduinoPort = getArduinoPort()
            self.isPortAvailable = True
        except IOError:
            self.isPortAvailable = False
            pass

#=========== PLOT CANVAS
class LinearPlotCanvas(FigureCanvas):
    def __init__(self, parent=None, width = 4.8, height = 3.8, dpi = 100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
 
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)
 
        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

    def plot(self, data):
        self.axes.clear()
        self.axes.plot(data, 'r-')
        self.draw()

class SquarePlotCanvas(LinearPlotCanvas):
    def plot(self, data):
        self.axes.clear()
        self.axes.imshow(data, cmap='gray', vmin=0, vmax=255)
        self.draw()

class PolarPlotCanvas(FigureCanvas):
    def __init__(self, parent=None, width = 4.8, height = 3.8, dpi = 100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111, polar='True')
 
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)
 
        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        self.axes.set_yticklabels([])

    def plot(self, data):
        self.axes.clear()
        theta, r = np.mgrid[0:2*np.pi:32j, 0:1:25j]
        z = data.reshape(theta.shape)
        self.axes.pcolormesh(theta, r, z, cmap='gray', vmin=0, vmax=1023)
        self.axes.set_yticklabels([])
        self.draw()


#=========== MAIN APP
class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        global windowHeight, windowWidth
        super(ApplicationWindow, self).__init__()
        self.setFixedSize(windowHeight, windowWidth)

        self.ui = uic.loadUi(os.path.join(curdir, 'DiodeTestGUI/mainwindow.ui'), self)
        self.setupTable()
        self.linearPlotCanvas = LinearPlotCanvas(self.linearPlotView)
        self.squarePlotCanvas = SquarePlotCanvas(self.squarePlotView)
        self.polarPlotCanvas = PolarPlotCanvas(self.polarPlotView)
        # == thread
        self.dataThread = DataTransferThread()
        self.setupSignals()
        self.dataThread.start()

    def setupSignals(self):
        #status bar
        self.dataThread.signalStatusBarUpdate.connect(self.updateStatusBar)
        self.dataThread.signalStatusBarStyleError.connect(self.setStatusBarStyleSheet_Error)
        self.dataThread.signalStatusBarStyleNormal.connect(self.setStatusBarStyleSheet_Normal)
        # plots
        self.dataThread.signalUpdateLinearPlot.connect(self.updateLinearPlot)
        self.dataThread.signalUpdateSquarePlot.connect(self.updateSquarePlot)
        # other
        self.dataThread.signalAddValuesToTable.connect(self.addTableItem)
        self.dataThread.signalStateLabelSetText.connect(self.setStateLabelText)
        self.dataThread.signalUpdateProgressBar.connect(self.updateProgressBar)
        self.ui.buttonStart.clicked.connect(self.startMeasurement)

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
    def updateLinearPlot(self, values):
        self.linearPlotCanvas.plot(values)

    @pyqtSlot(np.ndarray)
    def updateSquarePlot(self, values):
        self.squarePlotCanvas.plot(values)
        self.polarPlotCanvas.plot(values)

    @pyqtSlot()
    def startMeasurement(self):
        if arduinoPort.isOpen():
            arduinoPort.write(b'{Start}')

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
