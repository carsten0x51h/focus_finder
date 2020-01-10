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

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/range/adaptors.hpp>

/**
 * See https://stackoverflow.com/questions/45198739/iterate-through-multilevel-boost-tree
 *
 * For a perfect solution based on boost range.
 */
using boost::property_tree::ptree;

namespace PtreeTools {

namespace detail {
using boost::adaptors::filtered;
using boost::adaptors::transformed;

using Value = ptree::value_type;

struct KeyName {
	std::string const _target;
	bool operator()(Value const& v) const {
		return v.first == _target;
	}
};

struct HaveProperty {
	std::string const _target;
	bool operator()(Value const& v) const {
		return v.second.get_optional < std::string > (_target).is_initialized();
	}
};

template<typename T>
struct As {
	T operator()(Value const& v) const {
		return T { v.second };
	}
};
}

auto named(std::string const& name) {
	return detail::filtered(detail::KeyName { name });
}

auto having(std::string const& name) {
	return detail::filtered(detail::HaveProperty { name });
}

template<typename T>
auto as() {
	return detail::transformed(detail::As<T> { });
}
}

const ptree& empty_ptree() {
	static ptree t;
	return t;
}

struct IndiDriverInfoT {
	ptree data_;

	IndiDriverInfoT(const std::string & filePath) {
		std::ifstream ifs;
		ifs.open(filePath, std::ifstream::in);

		read_xml(ifs, data_);
	}

	struct Driver {
		ptree const& data_;

		std::string name() const {
			return data_.get < std::string > ("<xmlattr>.name", "UNKNOWN");
		}
	};

	struct Device {
		ptree const& data_;

		std::string label() const {
			return data_.get < std::string > ("<xmlattr>.label", "UNKNOWN");
		}

		Driver driver() const {
			using namespace PtreeTools;
			return Driver { data_.get_child("driver") };
		}

	};

	struct IndiDevGroupT {
		ptree const& data_;

		std::string name() const {
			return data_.get < std::string > ("<xmlattr>.group");
		}

		auto devices() const {
			using namespace PtreeTools;
			return data_ | named("device") | as<Device>();
		}
	};

	auto devGroups() const {
		using namespace PtreeTools;
		return data_.get_child("driversList") | named("devGroup")
				| as<IndiDevGroupT>();
	}
};
