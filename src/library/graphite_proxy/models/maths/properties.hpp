#ifndef GRAPHITE_PROXY_MATHS_PROPERTIES_HPP
#define GRAPHITE_PROXY_MATHS_PROPERTIES_HPP

// Maths property file

namespace graphite_proxy {
namespace maths {

// Nodes
#define NODE_ROOT      "maths"
#define NODE_SUM       "sum"
#define NODE_AVERAGE   "average"
#define NODE_MIN       "min"
#define NODE_MAX       "max"
#define NODE_MEDIAN    "median"
#define NODE_TILES     "tiles"
#define NODE_VARIANCE  "variance"
#define NODE_DEVIATION "deviation"
#define NODE_CATEGORY  "category"

// Attributes
#define ATTRIBUTE_NAME           "name"
#define ATTRIBUTE_BELOW          "below"
#define ATTRIBUTE_MULTIPLICATOR  "multiplicator"
#define ATTRIBUTE_VALUE          "value"
#define ATTRIBUTE_MIN_VALUE      1
#define ATTRIBUTE_TIME_MIN_VALUE 0

// Default attributes values
#define ATTRIBUTE_DEFAULT_BELOW         "true"
#define ATTRIBUTE_DEFAULT_MULTIPLICATOR "100"

// Helpers
#define XML_UNKNOWN   "unknown"
#define REGEX_TIME    "^([0-9]*[hH]*)([0-9]*[mM]*)([0-9]*[sS]*)$"
#define REGEX_INTEGER "[0-9]+"

} // namespace maths
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_MATHS_PROPERTIES_HPP
