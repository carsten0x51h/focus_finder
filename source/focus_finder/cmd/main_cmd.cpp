/*
 * main_cmd.C
 *
 *  Created on: Mar 30, 2019
 *      Author: devnull
 */

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



