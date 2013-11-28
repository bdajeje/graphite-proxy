#ifndef GRAPHITE_PROXY_MATHS_CATEGORY_HPP
#define GRAPHITE_PROXY_MATHS_CATEGORY_HPP

#include <graphite_proxy/models/maths/math_computation.hpp>

#include <boost/regex.hpp>
#include <boost/noncopyable.hpp>

namespace graphite_proxy {
namespace maths {

/*! A MathsCategory represents a node from the maths config file
 *  It could hold one or more math computations
 */
class MathsCategory : public boost::noncopyable
{
  public:

    /*! Constructor
     *  \param name is a filter string to accept messages (will be transform to a regex object)
     *  \param except_name is a filter string to discard messages (will be transform to a regex object)
     */
    MathsCategory( const std::string filter );

    /*! Add a new computation to the category
     *  \param computation is the new computation to add
     */
    void addComputation( MathComputation& computation ) { m_maths_computations.push_back( computation ); }

    /*! Get internal computations
     *  \return the internal computations
     */
    const std::vector<MathComputation>& getComputations() const { return m_maths_computations; }

    /*! Getter for the filter
     *  \return the filter
     */
    const boost::regex& getFilter() const { return m_filter; }

    /*! Does this category empty ?
     *  \return true if the category has no computations to do
     */
    bool empty() const { return m_maths_computations.empty(); }

  private:

    /*! Math computations for this category */
    std::vector<MathComputation> m_maths_computations;

    /*! Filter for this category */
    boost::regex                 m_filter;
};

} // namespace maths
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_MATHS_CATEGORY_HPP
