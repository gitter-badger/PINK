/**
 * @file   SelfOrganizingMapLib/Data.h
 * @date   Oct 12, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <vector>

#ifdef __CUDACC__
    #include <thrust/device_vector.h>
#endif

namespace pink {

//! Primary template for generic Data
template <typename Layout, typename T>
class Data
{
public:

    typedef T ValueType;
    typedef Data<Layout, T> SelfType;
    typedef Layout LayoutType;

    /// Default construction
    Data()
     : layout{0}
    {}

    /// Construction without initialization
    Data(LayoutType const& layout)
     : layout(layout),
       data(layout.get_size())
    {}

    /// Construction and initialize all elements to value
    Data(LayoutType const& layout, T value)
     : layout(layout),
       data(layout.get_size(), value)
    {}

    /// Construction and copy data
    Data(LayoutType const& layout, std::vector<T> const& data)
     : layout(layout),
       data(data)
    {}

    /// Construction and move data
    Data(LayoutType const& layout, std::vector<T>&& data)
     : layout(layout),
       data(data)
    {}

    auto operator == (SelfType const& other) const
    {
        return layout == other.layout and
               data == other.data;
    }

    auto operator [] (uint32_t position) -> T& { return data[position]; }
    auto operator [] (uint32_t position) const -> T const& { return data[position]; }

//    auto get_data() { return data; }
//    auto get_data() const { return data; }

    auto get_data_pointer() { return &data[0]; }
    auto get_data_pointer() const { return &data[0]; }

    auto get_dimension() -> typename LayoutType::DimensionType { return layout.dimension; }
    auto get_dimension() const -> typename LayoutType::DimensionType const { return layout.dimension; }

#ifdef __CUDACC__
    /// Return SOM device vector
    auto get_device_vector() -> thrust::device_vector<T>& { return d_data; }
    auto get_device_vector() const -> thrust::device_vector<T> const& { return d_data; }
#endif

private:

    template <typename A, typename B>
    friend void write(Data<A, B> const& data, std::string const& filename);

    LayoutType layout;

    std::vector<T> data;

#ifdef __CUDACC__
    thrust::device_vector<T> d_data;
#endif
};

} // namespace pink
