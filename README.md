# OpenCL-Lesion-Images-To-Grayscale
---

This repository contains C code for converting RGB images to grayscale using OpenCL. Below are the instructions to set up and use the code effectively.

## Prerequisites

1. **Visual Studio Configuration:**
   - Add the following libraries to your project:
     - `#include <CL/cl.h>`
     - `#include <dirent.h>`
     - `#include "stb_image.h"`
     - `#include "stb_image_write.h"`
   - Include necessary include folders and lib folders to:
     - VC++ Directory (Additional directories)
     - C/C++ directory (Additional Directories)
   - Add `OpenCL.lib;` to Linker -> Input in Configuration Manager in the project properties.

2. **Image Folders:**
   - Place your original RGB images in a folder named "Lesion_Images".
   - The output grayscale images will be saved in a folder named "Grayscale_Images".

3. **Image Format:**
   - Images of any size can be converted, but they must be in JPG format.

## Setup

1. **Download OpenCL SDK:**
   - Download the OpenCL SDK from [NVIDIA's OpenCL SDK](https://developer.nvidia.com/opencl).

2. **Download Required Libraries:**
   - Download `stb_image.h` and `stb_image_write.h` from [stb GitHub repository](https://github.com/nothings/stb/tree/master).
   - Download `dirent.h` from [tronkko's dirent GitHub repository](https://github.com/tronkko/dirent/tree/master).

## Usage

1. **Compile the Code:**
   - Compile the provided C code in Visual Studio after setting up the project properties as mentioned above.

2. **Run the Program:**
   - After successful compilation, run the executable.
   - The program will convert all images from the "Lesion_Images" folder to grayscale and save them in the "Grayscale_Images" folder.

3. **View Output:**
   - Check the "Grayscale_Images" folder to view the converted grayscale images.

4. **Customization (Optional):**
   - Modify the code according to your requirements, such as handling different image formats or implementing additional image processing functionalities.


## Input Image

---

![ISIC_1092675](https://github.com/IbrahimRao/OpenCL-Lesion-Images-To-Grayscale/assets/66884608/cd0d17b8-8af9-4e35-a532-bc0c903a99c0)


## Output Image

---

![ISIC_1092675](https://github.com/IbrahimRao/OpenCL-Lesion-Images-To-Grayscale/assets/66884608/a1b553c5-ac18-4e1c-bf63-e8889d3487fb)
