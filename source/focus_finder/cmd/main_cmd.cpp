/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
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

#include <iostream>
#include <thread>
#include <chrono>

#include <functional>
#include <utility>

#include <boost/signals2.hpp>

using namespace boost;
using namespace std::chrono_literals;

class MyWorkerT {
	using ListenersT = signals2::signal<void (int)>;

	private:
	 	ListenersT listeners;
	 	int counter;

		void publishEvent(int value) {
			std::cout << "Publish event..." << std::endl;
			listeners(value);
		}

	public:
		MyWorkerT() :counter(0) {
		}

		signals2::connection registerListener(const ListenersT::slot_type & inCallBack) {
			return listeners.connect(inCallBack);
		}

		template <class T>
		void unregisterListener(const T & inCallBack) {
			listeners.disconnect(inCallBack);
		}

		void runMe() {
			std::cout << "WorkerFunction\n";

			while(true) {
				std::this_thread::sleep_for(2s);
				std::cout << "Calling event handler..." << std::endl;
				publishEvent(counter++);
			}
		}
};

// TODO: Next step: introduce Re-introduce INDI...

int main(int /*argc*/, char** /**argv[]*/)
{
	/**
	 * Listener function...
	 */
	auto listenerFunction1 = [](int value){
		std::cout << "Listener 1 called - value: " << value << std::endl;
	};
	auto listenerFunction2 = [](int value){
		std::cout << "Listener 2 called - value: " << value << std::endl;
	};

	MyWorkerT myWorker;

	myWorker.registerListener(listenerFunction1); // TODO: In QT connect slots instead...
	myWorker.registerListener(listenerFunction2);

	std::thread t(&MyWorkerT::runMe, &myWorker);
	t.join();

	myWorker.unregisterListener(listenerFunction1);
	myWorker.unregisterListener(listenerFunction2);


	return 0;
}



