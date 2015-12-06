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

#include <iostream>
#include <motor.h>

int main(int argc, char *argv[])
{
	Motor *motor = new Motor();
	bool succ;
	int value;

	succ = motor->connect(Motor::PORT_B);
	std::cout << "connect succeeded: " << succ << "\n";

	std::cout << "Driver name is: " << motor->getDriverName() << "\n";
	std::cout << "getPosition() succ:" << motor->getPosition(&value)
		  << "\n";
	std::cout << "position is: " << value << "\n";
	std::cout << "psition_sp is: " << motor->getPositionSP() << "\n";
	std::cout << "getSpeed() succ: " << motor->getSpeed(&value) << "\n";
	std::cout << "speed is: " << value << "\n";
	std::cout << "count_per_rot is: " << motor->getCountPerRot() << "\n";

	return 0;
}
