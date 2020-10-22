#!/usr/bin/env python3

import asyncio
import bleak
import dbus

from PySide2.QtCore import Qt, QObject
from PySide2.QtCore import Signal, Slot


class BtListener(QObject):

	def __init__(self, parent=None):
		super().__init__(parent)
		self.loop = None

	data_updated = Signal(list)

	@Slot()
	def run(self):
		async def async_run(listener):
			scanner = bleak.BleakScanner()
			scanner.register_detection_callback(lambda e: listener.ble_event(e))
			await scanner.start()

		self.loop = asyncio.new_event_loop()
		asyncio.set_event_loop(self.loop)
		self.loop.create_task(async_run(self))
		self.loop.run_forever()

	def stop(self):
		if(self.loop):
			# TODO: does not work
			self.loop.call_soon_threadsafe(self.loop.stop)

	def get_device(self, path):
		# This is Linux (DBus) specific
		system_bus = dbus.SystemBus()
		device = system_bus.get_object('org.bluez', path)
		interface = dbus.Interface(device, dbus_interface='org.freedesktop.DBus.Properties')
		return interface.GetAll('org.bluez.Device1')


	def ble_event(self, event):
		try:
			device = self.get_device(event.path)
		except:
			return

		address = device['Address']
		alias = device['Alias']

		if(address == 'D7:84:0C:76:A2:1C'):
			data = [int(x) for x in device['ManufacturerData'][0xffff]]
			self.data_updated.emit(data)
