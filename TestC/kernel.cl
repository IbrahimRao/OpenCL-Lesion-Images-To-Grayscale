__kernel void convertImage(__global const unsigned char *input_image,
                                  __global unsigned char *output_image,
                                  const int width,
                                  const int height,
                                  const int channels) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x < width && y < height) {
        int index = y * width + x;
        int pixel_index = index * channels;

        // Calculate luminance value
        float luminance = 0.2126f * input_image[pixel_index] + 0.7152f * input_image[pixel_index + 1] + 0.0722f * input_image[pixel_index + 2];

        // Write luminance value to output image (for all channels)
        for (int c = 0; c < channels; ++c) {
            output_image[pixel_index + c] = (unsigned char)luminance;
        }
    }
}