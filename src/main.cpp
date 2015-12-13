/*
 * Holorobot - a control program for a holonomic Lego robot
 * Copyright (C) 2015  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdint>
#include <iostream>
#include <irreceiver.h>
#include <motor.h>

extern "C" {
#include <time.h>
#include <sys/time.h>
}

static __always_inline void swapPointers(IrValues **p1, IrValues **p2)
{
	IrValues *tmp;

	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}

static __always_inline void printSensor(IrValues &data)
{
	std::cout << "\t" << (int) data.ch[0] << "\t" << (int) data.ch[1]
		  << "\t" << (int) data.ch[2] << "\t" << (int) data.ch[3]
		  << "\t" << (int) data.ch[4] << "\t" << (int) data.ch[5]
		  << "\t" << (int) data.ch[6] << "\t" << (int) data.ch[7]
		  << "\n";
}

int main(int argc, char *argv[])
{
	Motor *motor = new Motor();
	bool succ;
	int value;
	struct timeval tv1;
	struct timeval tv2;
	int v1;
	int v2;
	unsigned int i;
	const unsigned int n = 10000;
	int vector[n];
	double duration;
	IrValues data1;
	IrValues data2;

	IrValues *olddata = &data1;
	IrValues *newdata = &data2;

	struct timespec tsdelay = { 0, 25000000 };

	succ = motor->connect(Motor::PORT_B);
	std::cout << "connect succeeded: " << succ << "\n";
	if (!succ)
		goto skip_motortest;

	std::cout << "Driver name is: " << motor->getDriverName() << "\n";
	std::cout << "getPosition() succ:" << motor->getPosition(&value)
		  << "\n";
	std::cout << "position is: " << value << "\n";
	std::cout << "psition_sp is: " << motor->getPositionSP() << "\n";
	std::cout << "getSpeed() succ: " << motor->getSpeed(&value) << "\n";
	std::cout << "speed is: " << value << "\n";
	std::cout << "count_per_rot is: " << motor->getCountPerRot() << "\n";

	succ = motor->setSpeedRegulation(true);
	std::cout << "setSpeedReuglation returned: " << succ << "\n";
	succ = motor->setSpeedSP(100);
	std::cout << "setPeedSP() returned: " << succ << "\n";
	succ = motor->setCommand(Motor::CMD_RUN_FOREVER);
	std::cout << "setCommand() returned: " << succ << "\n";

	succ = true;
	v1 = gettimeofday(&tv1, NULL);
	for (i = 0; i < n; i++) {
		succ = succ & motor->getSpeed(vector + i);
	}
	v2 = gettimeofday(&tv2, NULL);

	std::cout << "gettimeofday() 1 returned: " << v1 << "\n";
	std::cout << "gettimeofday() 2 returned: " << v2 << "\n";
	std::cout << "getSpeed() returned " << succ << "\n";

	for (i = 0; i < n; i+= 10) {
		std::cout <<
			vector[i + 0] << " " << vector[i + 1] << " " <<
			vector[i + 2] << " " << vector[i + 3] << " " <<
			vector[i + 4] << " " << vector[i + 5] << " " <<
			vector[i + 6] << " " << vector[i + 7] << " " <<
			vector[i + 8] << " " << vector[i + 9] << "\n";
	}

	succ = motor->setCommand(Motor::CMD_STOP);
	std::cout << "setCommand() returned: " << succ << "\n";

	duration = (double) (tv2.tv_sec - tv1.tv_sec);
	duration += ((double) ( tv2.tv_usec - tv1.tv_usec )) / 1000000.0;

	std::cout << "duration of speed measurement was " << duration
		  << "seconds" << "\n";

skip_motortest:
	std::cout << "Starting sensor tests!\n";
	IrReceiver receiver;

	succ = receiver.connect("in1:i2c1");
	std::cout << "connect succeeded: " << succ << "\n";
	if (!succ)
		goto skip_sensortest;

	succ = receiver.setPoll_ms(25);
	std::cout << "setPoll_ms(25) returned: " << succ << "\n";
	succ = receiver.setMode(IrReceiver::MODE_ALL_CH);
	std::cout << "setMode() returned: " << succ << "\n";

	receiver.getAllValues(newdata);
	nanosleep(&tsdelay, NULL);

	while(true) {
		swapPointers(&olddata, &newdata);
		receiver.readFromDrivers();
		receiver.getAllValues(newdata);
		if (memcmp(olddata, newdata, 8 * sizeof(int8_t))) {
			printSensor(*newdata);
		}
		nanosleep(&tsdelay, NULL);
	}
skip_sensortest:
	delete motor;
	return 0;
}
