#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "RtWeekend.h"

#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"

#include "Ray.h"
#include "Color.h"
#include "Vec3.h"

#include <stdio.h>
#include <iostream>

cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);

__global__ void addKernel(int *c, const int *a, const int *b)
{
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}

int main()
{
	HittableList world;

	auto groundMaterial = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(Point3(0.0, -1000.0, 0.0), 1000.0, groundMaterial));

	for (int gridX = -11; gridX < 11; gridX++)
	{
		for (int gridZ = -11; gridZ < 11; gridZ++)
		{
			const double materialSelector = RandomDouble();

			const Point3 center(
				gridX + 0.9 * RandomDouble(),
				0.2,
				gridZ + 0.9 * RandomDouble()
			);

			if ((center - Point3(4.0, 0.2, 0.0)).Length() > 0.9)
			{
				std::shared_ptr<Material> sphereMaterial;

				if (materialSelector < 0.8)
				{
					const Color albedo = Color::Random() * Color::Random();
					sphereMaterial = std::make_shared<Lambertian>(albedo);
				}
				else if (materialSelector < 0.95)
				{
					const Color albedo = Color::Random(0.5, 1.0);
					const double fuzz = RandomDouble(0.0, 0.5);
					sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
				}
				else
				{
					sphereMaterial = std::make_shared<Dielectric>(1.5);
				}

				world.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
			}
		}
	}

	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Point3(0.0, 1.0, 0.0), 1.0, material1));

	auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(Point3(-4.0, 1.0, 0.0), 1.0, material2));

	auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(Point3(4.0, 1.0, 0.0), 1.0, material3));

	// Camera
	Camera camera;

	camera.aspectRatio = 16.0 / 9.0;
	camera.imageWidth = 1200;
	camera.samplesPerPixel = 500;
	camera.maxDepth = 50;

	camera.vFov = 20;
	camera.lookFrom = Point3(13.0, 2.0, 3.0);
	camera.lookAt = Point3(0.0, 0.0, 0.0);
	camera.vUp = Vec3(0, 1, 0);

	camera.defocus_angle = 0.6;
	camera.focus_dist = 10.0;

	// Render

	camera.Render(world);

    return 0;
}

// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size)
{
    int *dev_a = 0;
    int *dev_b = 0;
    int *dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    addKernel<<<1, size>>>(dev_c, dev_a, dev_b);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }
    
    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);
    
    return cudaStatus;
}
