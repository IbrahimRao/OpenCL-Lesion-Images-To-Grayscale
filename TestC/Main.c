// Ibrahim Hamid Rao
// I21-2958
// PDC Assignment 3
// Bs(CS) H


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <CL/cl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define MAX_SOURCE_SIZE (0x100000)

// Loading images from the folder "Lesion_Images" using stb_image.h
unsigned char* loadImage(const char* input_path, int* img_width, int* img_height, int* img_channels) {
    unsigned char* img_data = stbi_load(input_path, img_width, img_height, img_channels, STBI_rgb);
    if (!img_data) {
        printf("Error loading image %s.\n", input_path);
    }
    else {
        printf("Image loaded \033[0;32msuccessfully\033[0m: %s\n", input_path);
        printf("-> Width: %d, Height: %d, Channels: %d\n", *img_width, *img_height, *img_channels);
        printf("\n");
        
    }
    return img_data;
}

// Function to create OpenCL buffers for input and output images
cl_int createBuffers(cl_context ctx, unsigned char* img_data, int img_width, int img_height, int img_channels, cl_mem* input_image, cl_mem* output_image) {
    cl_int ret;
    *input_image = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned char) * img_width * img_height * img_channels, img_data, &ret);
    if (ret != CL_SUCCESS) {
        printf("Error creating input buffer.\n");
        return ret;
    }
    *output_image = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, sizeof(unsigned char) * img_width * img_height * img_channels, NULL, &ret);
    if (!(ret == CL_SUCCESS)) 
    { 
        printf("Error creating output buffer.\n");
        clReleaseMemObject(*input_image);
        return ret; 
    }
    return CL_SUCCESS;
}

// Setting kernel arguments
cl_int setKernelArguments(cl_kernel krnl, cl_mem input_image, cl_mem output_image, int img_width, int img_height, int img_channels) {
    cl_int ret;
    ret = clSetKernelArg(krnl, 0, sizeof(cl_mem), (void*)&input_image);
    ret |= clSetKernelArg(krnl, 1, sizeof(cl_mem), (void*)&output_image);
    ret |= clSetKernelArg(krnl, 2, sizeof(int), (void*)&img_width);
    ret |= clSetKernelArg(krnl, 3, sizeof(int), (void*)&img_height);
    ret |= clSetKernelArg(krnl, 4, sizeof(int), (void*)&img_channels);
    if (!(ret == CL_SUCCESS)) 
    { 
        printf("Error setting kernel arguments.\n"); 
    }
    return ret;
}

// Executing the kernel
cl_int executeKernel(cl_command_queue cmd_queue, cl_kernel krnl, int img_width, int img_height) {
    size_t global_item_size[2] = { img_width, img_height };
    cl_int ret = clEnqueueNDRangeKernel(cmd_queue, krnl, 2, NULL, global_item_size, NULL, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        printf("Error executing kernel.\n");
    }
    return ret;
}

// Reading the output buffer
cl_int readOutputBuffer(cl_command_queue cmd_queue, cl_mem output_image, int img_width, int img_height, int img_channels, unsigned char** output_data) {
    cl_int ret;
    *output_data = (unsigned char*)malloc(sizeof(unsigned char) * img_width * img_height * img_channels);
    ret = clEnqueueReadBuffer(cmd_queue, output_image, CL_TRUE, 0, sizeof(unsigned char) * img_width * img_height * img_channels, *output_data, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        printf("Error reading output buffer.\n");
        free(*output_data);
    }
    return ret;
}

// Saving the output image
cl_int saveOutputImage(const char* output_path, int img_width, int img_height, int img_channels, unsigned char* output_data) {
    stbi_write_jpg(output_path, img_width, img_height, img_channels, output_data, 100);
    printf("-> Output image %s saved \033[0;32msuccessfully\033[0m.\n", output_path);
    printf("\n");
    return CL_SUCCESS;
}

// Wrapper function to perform the entire grayscale conversion process
cl_int convertImageToGrayscale(const char* input_path, const char* output_path, cl_kernel krnl, cl_command_queue cmd_queue, cl_context ctx) {
    int img_width, img_height, img_channels;
    unsigned char* img_data = loadImage(input_path, &img_width, &img_height, &img_channels);
    if (!img_data) return -1;

    cl_mem input_image, output_image;
    cl_int ret = createBuffers(ctx, img_data, img_width, img_height, img_channels, &input_image, &output_image);
    if (ret != CL_SUCCESS) {
        stbi_image_free(img_data);
        return ret;
    }

    ret = setKernelArguments(krnl, input_image, output_image, img_width, img_height, img_channels);
    if (ret != CL_SUCCESS) {
        stbi_image_free(img_data);
        clReleaseMemObject(input_image);
        clReleaseMemObject(output_image);
        return ret;
    }

    ret = executeKernel(cmd_queue, krnl, img_width, img_height);
    if (ret != CL_SUCCESS) {
        stbi_image_free(img_data);
        clReleaseMemObject(input_image);
        clReleaseMemObject(output_image);
        return ret;
    }

    unsigned char* output_data;
    ret = readOutputBuffer(cmd_queue, output_image, img_width, img_height, img_channels, &output_data);
    if (ret != CL_SUCCESS) {
        stbi_image_free(img_data);
        clReleaseMemObject(input_image);
        clReleaseMemObject(output_image);
        return ret;
    }

    saveOutputImage(output_path, img_width, img_height, img_channels, output_data);

    stbi_image_free(img_data);
    free(output_data);
    clReleaseMemObject(input_image);
    clReleaseMemObject(output_image);

    return CL_SUCCESS;
}

int main() {

    printf("Converting RBG ISIC Dataset Images to Grayscale:\n");
    printf("Loading the Images from the folder:- Lesion_Images\n");
    printf("\n");
    printf("=========================================================================================");
    printf("\n");
    printf("\n");

    // Get platform and device information
    cl_platform_id plat_id = NULL;
    cl_device_id dev_id = NULL;
    cl_uint num_plat;
    cl_uint num_dev;
    cl_int ret = clGetPlatformIDs(1, &plat_id, &num_plat);
    if (!(ret == CL_SUCCESS)) 
    {
        printf("Error getting platform IDs.\n");
        return 1;
    }
    ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_GPU, 1, &dev_id, &num_dev);
    if (!(ret == CL_SUCCESS)) 
    {
        printf("Error getting device IDs.\n");
        return 1;
    }

    // Create OpenCL context
    cl_context ctx = clCreateContext(NULL, 1, &dev_id, NULL, NULL, &ret);
    if (!(ret == CL_SUCCESS)) 
    {
        printf("Error creating OpenCL context.\n");
        return 1;
    }

    // Create command queue
    cl_command_queue cmd_queue = clCreateCommandQueue(ctx, dev_id, 0, &ret);
    if (!(ret == CL_SUCCESS)) 
    {
        printf("Error creating command queue.\n");
        return 1;
    }

    // Load kernel source code
    FILE* kernel_file;
    errno_t err = fopen_s(&kernel_file, "kernel.cl", "r");
    if (err != 1-1 || kernel_file == NULL) 
    {
        printf("Failed to open kernel file.\n");
        return 1;
    }

    char* kernel_src = (char*)malloc(MAX_SOURCE_SIZE);
    size_t kernel_size = fread(kernel_src, 1, MAX_SOURCE_SIZE, kernel_file);
    fclose(kernel_file);

    // Create program from kernel source
    cl_program prog = clCreateProgramWithSource(ctx, 1, (const char**)&kernel_src, (const size_t*)&kernel_size, &ret);
    if (!(ret == CL_SUCCESS)) 
    { 
        printf("Error creating program from kernel source.\n"); 
        return 1; 
    }

    // Build program
    ret = clBuildProgram(prog, 1, &dev_id, NULL, NULL, NULL);
    if (!(ret == CL_SUCCESS)) 
    {
        printf("Error building program.\n");
        char build_log[16384];
        clGetProgramBuildInfo(prog, dev_id, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
        printf("Build log:\n%s\n", build_log);
        return 1;
    }

    // Create kernel
    cl_kernel krnl = clCreateKernel(prog, "convertImage", &ret);
    if (ret != CL_SUCCESS) 
    { 
        printf("Error creating kernel.\n"); 
        return 1; 
    }

    // Open the directory
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir("Lesion_Images")) == NULL) 
    {
        // Iterate through all files in the directory
        perror("Error opening directory");
        return 1;
    }
    else {
        while ((ent = readdir(dir)) != NULL) {
            // Check if the file is a regular file (not a directory)
            if (ent->d_type == DT_REG) {
                char input_path[256];
                char output_path[256];
                snprintf(input_path, sizeof(input_path), "Lesion_Images/%s", ent->d_name);
                snprintf(output_path, sizeof(output_path), "Grayscale_Images/%s", ent->d_name);

                // Convert the image to grayscale
                convertImageToGrayscale(input_path, output_path, krnl, cmd_queue, ctx);
            }
        }
        closedir(dir);
    }

    // Clean up
    free(kernel_src);
    clReleaseKernel(krnl);
    clReleaseProgram(prog);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(ctx);

    return 0;
}

