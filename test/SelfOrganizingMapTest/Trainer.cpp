/**
 * @file   SelfOrganizingMapTest/Trainer.cpp
 * @brief  Unit tests for image processing.
 * @date   Sep 17, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#include <cmath>
#include "gtest/gtest.h"

#include "SelfOrganizingMapLib/CartesianLayout.h"
#include <SelfOrganizingMapLib/Data.h>
#include <SelfOrganizingMapLib/SOM.h>
#include <SelfOrganizingMapLib/TrainerCPU.h>
#include "UtilitiesLib/DistributionFunction.h"

using namespace pink;

TEST(SelfOrganizingMapTest, trainer_num_rot)
{
	typedef Trainer<CartesianLayout<2>, CartesianLayout<2>, float, false> MyTrainer;

    EXPECT_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0,  -4, true, 0.1, false), std::runtime_error);
    EXPECT_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0,  -1, true, 0.1, false), std::runtime_error);
    EXPECT_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0,   0, true, 0.1, false), std::runtime_error);
    EXPECT_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0,  90, true, 0.1, false), std::runtime_error);

    EXPECT_NO_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0,   1, true, 0.1, false));
    EXPECT_NO_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0,   4, true, 0.1, false));
    EXPECT_NO_THROW(MyTrainer(GaussianFunctor(1.1, 0.2), 0, 720, true, 0.1, false));
}

TEST(SelfOrganizingMapTest, trainer_cartesian_2d)
{
    typedef Data<CartesianLayout<2>, float> DataType;
    typedef SOM<CartesianLayout<2>, CartesianLayout<2>, float> SOMType;
	typedef Trainer<CartesianLayout<2>, CartesianLayout<2>, float, false> MyTrainer;

    uint32_t som_size = 2;
    uint32_t image_size = 2;
    uint32_t neuron_size = 2;

    std::vector<float> data{1, 1, 1, 1};
    DataType image({image_size, image_size}, &data[0]);
    SOMType som({som_size, som_size}, {neuron_size, neuron_size}, 0.0);

    MyTrainer trainer(
        GaussianFunctor(1.1, 0.2),  // std::function<float(float)> distribution_function
        0,                          // int verbosity
        4,                          // int number_of_rotations
        true,                       // bool use_flip
        0.1,                        // float progress_factor
        0                           // int max_update_distance
    );

    trainer(som, image);

    float v1 = GaussianFunctor(1.1, 0.2)(0.0);
    float v2 = GaussianFunctor(1.1, 0.2)(1.0);
    float v3 = GaussianFunctor(1.1, 0.2)(std::sqrt(2.0));

    for (int i = 0; i != 4; ++i) {
        EXPECT_FLOAT_EQ(v1, som.get_neuron({0, 0}).get_data_pointer()[i]);
        EXPECT_FLOAT_EQ(v2, som.get_neuron({1, 0}).get_data_pointer()[i]);
        EXPECT_FLOAT_EQ(v2, som.get_neuron({0, 1}).get_data_pointer()[i]);
        EXPECT_FLOAT_EQ(v3, som.get_neuron({1, 1}).get_data_pointer()[i]);
    }
}