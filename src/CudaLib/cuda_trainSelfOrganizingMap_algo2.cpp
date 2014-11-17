/**
 * @file   CudaLib/cuda_trainSelfOrganizingMap.cpp
 * @date   Nov 3, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "CudaLib.h"
#include "ImageProcessingLib/Image.h"
#include "ImageProcessingLib/ImageIterator.h"
#include "ImageProcessingLib/ImageProcessing.h"
#include "SelfOrganizingMapLib/SelfOrganizingMap.h"
#include "UtilitiesLib/Filler.h"
#include <iostream>
#include <iomanip>
#include <stdio.h>

using namespace std;
using namespace PINK;

void cuda_trainSelfOrganizingMap_algo2(InputData const& inputData)
{
    if (inputData.verbose) {
    	cout << "\n Starting CUDA version algorithm 2" << endl;
    	cuda_print_properties();
    }

	// Memory allocation
	if (inputData.verbose) cout << "\n  Size of SOM = " << inputData.som_total_size * sizeof(float) << " bytes" << endl;
	float *d_som = cuda_alloc_float(inputData.som_total_size);

	if (inputData.verbose) cout << "  Size of rotated images = " << inputData.numberOfRotationsAndFlip * inputData.neuron_size * sizeof(float)<< " bytes" << endl;
	float *d_rotatedImages = cuda_alloc_float(inputData.numberOfRotationsAndFlip * inputData.neuron_size);

	if (inputData.verbose) cout << "  Size of euclidean distance matrix = " << inputData.som_size * sizeof(float) << " bytes" << endl;
	float *d_euclideanDistanceMatrix = cuda_alloc_float(inputData.som_size);

	if (inputData.verbose) cout << "  Size of best rotation matrix = " << inputData.som_size * sizeof(int) << " bytes\n" << endl;
	int *d_bestRotationMatrix = cuda_alloc_int(inputData.som_size);

	if (inputData.verbose) cout << "  Size of image = " << inputData.image_size * sizeof(float) << " bytes\n" << endl;
	float *d_image = cuda_alloc_float(inputData.image_size_using_flip);

	// Best matching position (x,y)
    int *d_bestMatch = cuda_alloc_int(2);

    // Initialize SOM
	if (inputData.init == ZERO) cuda_fill_zero(d_som, inputData.som_total_size);
	else {
		float *som = (float *)malloc(inputData.som_total_size * sizeof(float));
		fillWithValue(som, inputData.som_total_size);
		cuda_copyHostToDevice_float(d_som, som, inputData.som_total_size);
		free(som);
	}

    // Prepare trigonometric values
	float *d_cosAlpha = NULL, *d_sinAlpha = NULL;
	trigonometricValues(&d_cosAlpha, &d_sinAlpha, inputData.numberOfRotations - 1);

	// Progress status
	float progress = 0.0;
	float progressStep = 1.0 / inputData.numIter / inputData.numberOfImages;
	float nextProgressPrint = 0.0;

	for (int iter = 0; iter != inputData.numIter; ++iter)
	{
		int i = 0;
		for (ImageIterator<float> iterImage(inputData.imagesFilename),iterEnd; iterImage != iterEnd; ++i, ++iterImage)
		{
			if (inputData.verbose) {
				if (progress >= nextProgressPrint) {
					cout << "  Progress: " << fixed << setprecision(0) << progress*100 << " %" << endl;
					nextProgressPrint += inputData.progressFactor;
				}
				progress += progressStep;
			}

			cuda_copyHostToDevice_float(d_image, iterImage->getPointerOfFirstPixel(), inputData.image_size);

			cuda_generateRotatedImages(d_rotatedImages, d_image, inputData.numberOfRotations,
				inputData.image_dim, inputData.neuron_dim, inputData.useFlip, d_cosAlpha, d_sinAlpha);

			cuda_generateEuclideanDistanceMatrix_algo2(d_euclideanDistanceMatrix, d_bestRotationMatrix,
				inputData.som_dim, d_som, inputData.neuron_dim, inputData.numberOfRotationsAndFlip, d_rotatedImages);

			cuda_updateNeurons(d_som, d_rotatedImages, d_bestRotationMatrix, d_euclideanDistanceMatrix, d_bestMatch,
				inputData.som_dim, inputData.neuron_dim, inputData.numberOfRotationsAndFlip);
		}
	}

	// Free memory
	if (d_cosAlpha) cuda_free(d_cosAlpha);
	if (d_sinAlpha) cuda_free(d_sinAlpha);
    cuda_free(d_image);
    cuda_free(d_bestRotationMatrix);
    cuda_free(d_euclideanDistanceMatrix);
    cuda_free(d_rotatedImages);
    cuda_free(d_bestMatch);

	if (inputData.verbose) {
		cout << "  Progress: 100 %\n" << endl;
		cout << "  Write final SOM to " << inputData.resultFilename << " ..." << endl;
	}

	vector<float> som(inputData.som_total_size);
	cuda_copyDeviceToHost_float(&som[0], d_som, inputData.som_total_size);
	writeSOM(&som[0], inputData.som_dim, inputData.neuron_dim, inputData.resultFilename);

	// Free memory
    cuda_free(d_som);
}