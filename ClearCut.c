#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void updateImage(const char* file_name) {
    int width, height, channels;
    unsigned char *image_data = stbi_load(file_name, &width, &height, &channels, STBI_rgb_alpha);
    
    if (!image_data) {
        printf("Error loading image.\n");
        return;
    }

    // Iterate through each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate the index of the current pixel
            int pixel_index = (y * width + x) * 4; // Each pixel has 4 channels (RGBA)
            
            // Read the color of the current pixel
            unsigned char red = image_data[pixel_index];
            unsigned char green = image_data[pixel_index + 1];
            unsigned char blue = image_data[pixel_index + 2];
            unsigned char alpha = image_data[pixel_index + 3];

            // Output the color of the current pixel
            printf("Pixel at (%d, %d): R=%d, G=%d, B=%d, A=%d\n", x, y, red, green, blue, alpha);
        }
    }

    // Free the image data
    stbi_image_free(image_data);
}

void readDir(const char* dir, int system) {
    char command[256];

    if (system == 0) {
        strcpy(command, "dir /b ");
    } else if (system == 1) {
        strcpy(command, "ls ");
    } else {
        printf("\x1b[31mUnknown system type\x1b[0m\n");
        return;
    }

    FILE *ls_output;
    char file_name[256];

    int location = 0;

    ls_output = popen(strcat(command, dir), "r");
    if (ls_output == NULL) {
        printf("\x1b[31mError opening pipe for \"%s %s\"\x1b[0m\n", command, dir);
        //return 2;
    }

    //printf("Files in directory:\n");
    while (fgets(file_name, sizeof(file_name), ls_output) != NULL) {
        // Remove the newline character if present
        if (file_name[strlen(file_name) - 1] == '\n') {
            file_name[strlen(file_name) - 1] = '\0';
        }

        printf("%s\n", file_name);
        
    }

    pclose(ls_output);
}

int main(int argv, const char* args) {
    int system = 0; // 0 - MS-DOS, 1 - Unix

    readDir(".\\", system);

    return 0;
}