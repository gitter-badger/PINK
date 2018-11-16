/**
 * @file   CudaLib/rotate_and_crop_bilinear_kernel.h
 * @date   Nov 4, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <cuda_runtime.h>
#include <thrust/device_ptr.h>

/**
 * CUDA Kernel Device code for combined rotation and cropping of a list of quadratic images.
 */
template <typename T>
__global__
void rotate_and_crop_bilinear_kernel(thrust::device_ptr<T> rotatedImages, thrust::device_ptr<const T> image, int neuron_size,
    int neuron_dim, int image_dim, thrust::device_ptr<const T> cosAlpha,
    thrust::device_ptr<const T> sinAlpha, int numberOfChannels)
{
    int x2 = blockIdx.x * blockDim.x + threadIdx.x;
    int y2 = blockIdx.y * blockDim.y + threadIdx.y;

    if (x2 >= neuron_dim or y2 >= neuron_dim) return;

    T center = (image_dim - 1) * 0.5;
    T margin = (image_dim - neuron_dim) * 0.5;
    T center_margin = center - margin;

    T cosAlpha_local = cosAlpha[blockIdx.z];
    T sinAlpha_local = sinAlpha[blockIdx.z];

    T x1 = (x2-center_margin)*cosAlpha_local + (y2-center_margin)*sinAlpha_local + center + 0.1;
    T y1 = (y2-center_margin)*cosAlpha_local - (x2-center_margin)*sinAlpha_local + center + 0.1;

    int ix1 = x1;
    int iy1 = y1;
    int ix1b = ix1 + 1;
    int iy1b = iy1 + 1;

    T rx1 = x1 - ix1;
    T ry1 = y1 - iy1;
    T cx1 = 1.0f - rx1;
    T cy1 = 1.0f - ry1;

    T* pCurRot = thrust::raw_pointer_cast(rotatedImages) + blockIdx.z * numberOfChannels * neuron_size;

    T value = cx1 * cy1 * image[ix1  * image_dim + iy1 ]
            + cx1 * ry1 * image[ix1  * image_dim + iy1b]
            + rx1 * cy1 * image[ix1b * image_dim + iy1 ]
            + rx1 * ry1 * image[ix1b * image_dim + iy1b];

    if (x1 >= 0 and x1 < image_dim and y1 >= 0 and y1 < image_dim) {
        atomicExch(pCurRot + x2*neuron_dim + y2, value);
    } else {
        atomicExch(pCurRot + x2*neuron_dim + y2, 0.0f);
    }
}