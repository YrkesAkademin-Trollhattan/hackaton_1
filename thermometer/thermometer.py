#!/usr/bin/env python3

import sys
import signal
import getopt

from PySide2.QtCore import Qt, QObject, QMetaObject
from PySide2.QtCore import Property
from PySide2.QtCore import SIGNAL, SLOT
from PySide2.QtCore import Signal, Slot
from PySide2.QtCore import QAbstractListModel, QModelIndex
from PySide2.QtCore import QThread

from PySide2.QtWidgets import QApplication

from PySide2.QtQml import QQmlApplicationEngine
from PySide2.QtQml import ListProperty

from btlistener import BtListener

def sigint_handler(*args):
	QApplication.quit()


class Thermometer(QObject):

	def __init__(self, parent=None):
		super().__init__(parent)
		self.battery_value = None
		self.rh_value = None
		self.temperature_value = None

	def get_temperature(self):
		return self.temperature_value

	@Signal
	def temperature_changed():
		pass

	def get_rh(self):
		return self.rh_value

	@Signal
	def rh_changed():
		pass

	def get_battery(self):
		return self.battery_value

	@Signal
	def battery_changed():
		pass

	@Slot(list)
	def update(self, data):
		battery = data[0] / 255 * 1.2 * 3.2
		temperature = (256*data[2] + data[1])/100 - 100
		rh = (256*data[4] + data[3])/100

		self.battery_value = battery
		self.temperature_value = temperature
		self.rh_value = rh

		self.battery_changed.emit()
		self.temperature_changed.emit()
		self.rh_changed.emit()

	battery = Property(float, get_battery, notify=battery_changed)
	temperature = Property(float, get_temperature, notify=temperature_changed)
	rh = Property(float, get_rh, notify=temperature_changed)


if(__name__ == '__main__'):

	# Parse command line options
	shortopts = 'd'
	opts, args = getopt.getopt(sys.argv[1:], shortopts)

	runs_on_target = True
	for o, a in opts:
		if(o == '-d'):
			runs_on_target = False

	# Setup Qt application
	app = QApplication(sys.argv)
	signal.signal(signal.SIGINT, sigint_handler)

	bt_thread = QThread()
	bt_thread.start()

	bt_listener = BtListener()
	bt_listener.moveToThread(bt_thread)
	QMetaObject.invokeMethod(bt_listener, 'run')

	thermometer = Thermometer()
	bt_listener.data_updated.connect(thermometer.update)

	engine = QQmlApplicationEngine()
	root = engine.rootContext()
	root.setContextProperty('thermometer', thermometer)
	root.setContextProperty('runsOnTarget', runs_on_target)

	engine.load('./main.qml')

	exit_code = app.exec_()

	# TODO: stopping asyncio thread does not work
#	bt_listener.stop()
#	bt_thread.wait()
	sys.exit(exit_code)
