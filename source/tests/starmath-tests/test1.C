/*
 * test1.C
 *
 *  Created on: Mar 30, 2019
 *      Author: devnull
 */




#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>

//using namespace focus_finder;


//BOOST_GLOBAL_FIXTURE( StartIndiTestServer );


BOOST_AUTO_TEST_SUITE(indi_client_tests)

BOOST_AUTO_TEST_CASE(set_and_get_host_info_from_client_test)
{
  // const std::string hostname = "localhost";
  // const unsigned int port = 1234;

  // IndiClientT indiClient(hostname, port);

  // BOOST_CHECK_EQUAL(indiClient.getHostName(), hostname);
  // BOOST_CHECK_EQUAL(indiClient.getPort(), port);
	BOOST_CHECK_EQUAL(2, 2);
}

BOOST_AUTO_TEST_SUITE_END()
