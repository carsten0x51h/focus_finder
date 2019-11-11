/*****************************************************************************
 *
 *  libindiclient++
 *
 *  Copyright(C) 2015 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#ifndef __WAIT_FOR__
#define __WAIT_FOR__ __WAIT_FOR__

#include <chrono>
#include <thread>

#include "exception.h"
#include "logging.h"


DEF_Exception(Timeout);


static void wait_for(std::function<bool()> conditionToWaitFor,
		std::chrono::milliseconds maxWaitMillis) {

	using namespace std::chrono_literals;

	std::chrono::time_point < std::chrono::system_clock > start =
			std::chrono::system_clock::now();

	bool hitTimeout;

	do {
		auto waitingSinceMillis = std::chrono::duration_cast
				< std::chrono::milliseconds
				> (std::chrono::system_clock::now() - start);

		hitTimeout = (waitingSinceMillis >= maxWaitMillis);

		LOG(debug) << "wait_for - hitTimeout: " << hitTimeout
				<< ", conditionToWaitFor: " << conditionToWaitFor() << " -> "
				<< (!conditionToWaitFor() && !hitTimeout) << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(100ms));

	} while (!conditionToWaitFor() && !hitTimeout);

	LOG(debug) << "wait_for - hitTimeout: " << hitTimeout
			   << ", conditionToWaitFor: " << conditionToWaitFor() << std::endl;

	if (hitTimeout) {
		std::stringstream ss;
		ss << "Hit timeout after '" << maxWaitMillis.count() << "' ms."
				<< std::endl;
		throw TimeoutExceptionT(ss.str());
	}
}

#endif /*__WAIT_FOR__*/
