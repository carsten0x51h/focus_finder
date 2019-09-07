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
