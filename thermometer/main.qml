import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 1.4
import QtQuick.Extras 1.4 as Extras
import QtQuick.Controls.Styles 1.4

Window
{
    visible: true
	visibility: runsOnTarget? 'FullScreen' : 'Windowed'
    width: 800
    height: 480
    title: 'YA Test'

	Extras.CircularGauge
	{
		x: 70
		y: 20
		width: 300
		height: 300
		minimumValue: 0
		maximumValue: 120
		stepSize: 1
		value: thermometer.temperature

		style: CircularGaugeStyle
		{
			background: Rectangle
			{
				width: parent.width
				height: parent.height

				Image
				{
					width: 50
					height: 50
					anchors.horizontalCenter: parent.horizontalCenter
					y :50
					source: 'ya.svg'
				}
			}
		}

		Text
		{
			anchors.horizontalCenter: parent.horizontalCenter
			y: 250
			text: 'Lab temp °C'
		}
	}

	Extras.CircularGauge
	{
		x: 400
		y: 20
		width: 300
		height: 300
		minimumValue: 0
		maximumValue: 100
		stepSize: 1
		value: thermometer.rh

		style: CircularGaugeStyle
		{
			background: Rectangle
			{
				width: parent.width
				height: parent.height

				Image
				{
					width: 50
					height: 50
					anchors.horizontalCenter: parent.horizontalCenter
					y :50
					source: 'ya.svg'
				}
			}
		}

		Text
		{
			anchors.horizontalCenter: parent.horizontalCenter
			y: 250
			text: 'Lab RH %'
		}
	}

	Rectangle
	{
		x: 20
		y: 350

		width: parent.width-40
		height: 1

		color: '#444488'

/*
		gradient: Gradient
		{
			orientation: Gradient.Horizontal
			GradientStop { position: 0.0; color: '#ffffff' }
			GradientStop { position: 0.5; color: '#000000' }
			GradientStop { position: 1.0; color: '#ffffff' }
		}
*/
	}

	Text
	{
		x: 40
		y: 400

		font.pixelSize: 24
		text: 'Battery voltage: ' + thermometer.battery.toFixed(2) + ' V'
	}

	MouseArea
	{
		anchors.fill: parent
		enabled: false
		cursorShape: Qt.BlankCursor
	}
}
