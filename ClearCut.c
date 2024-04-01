#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "stb/stb_image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* supported[] = {"jpg", "jpeg", "png", "bmp", "gif", "psd", "tga", "hdr", "exr", "pic", "pnm", "pbm", "pgm", "ppm"};
int supportedCount = 14;

int prevPerc = -1;

void progressBar(int percent) {

    if (percent == prevPerc) {
        return;
    }

    prevPerc = percent;

    if (percent > 0) {
        printf("\r");
    }

    int len = 50;

    int curr = percent / (100 / len);

    printf("<");
    for (int i = 0; i < len; i++) {
        if (curr > 0) {
            printf("=");
            curr--;
        } else {
            printf("-");
        }
    }

    printf("> (%d%%)", percent);
}

void clearLastLine() {
    // Move cursor up one line
    printf("\x1b[1A"); // ASCII escape sequence for moving cursor up
    // Clear the entire line
    printf("\x1b[2K"); // ASCII escape sequence for clearing line
}

void updateImage(const char* file_name) {
    int width, height, channels;
    unsigned char *image_data = stbi_load(file_name, &width, &height, &channels, STBI_rgb_alpha);
    
    if (!image_data) {
        printf("\x1b[31mError loading image\x1b[0m\n");
        return;
    }

    unsigned char *new_image_data = (unsigned char *)malloc(width * height * 4);
    if (!image_data) {
        printf("\x1b[31mFailed to allocate memory for image data\x1b[0m\n");
        return;
    }

    // Iterate through each pixel
    int delta = 50;
    unsigned char base_red = image_data[0];
    unsigned char base_green = image_data[0 + 1];
    unsigned char base_blue = image_data[0 + 2];

    int pos = 0;

    progressBar(0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            // Calculate the index of the current pixel
            int pixel_index = (y * width + x) * 4; // Each pixel has 4 channels (RGBA)
            
            // Read the color of the current pixel
            unsigned char red = image_data[pixel_index];
            unsigned char green = image_data[pixel_index + 1];
            unsigned char blue = image_data[pixel_index + 2];
            unsigned char alpha = image_data[pixel_index + 3];

            if (red < base_red + delta && red > base_red - delta && green < base_green + delta && green > base_green - delta && blue < base_blue + delta && blue > base_blue - delta) {
                new_image_data[pixel_index + 0] = 0;     // Red component
                new_image_data[pixel_index + 1] = 0;     // Green component
                new_image_data[pixel_index + 2] = 0;   // Blue component
                new_image_data[pixel_index + 3] = 0;   // Alpha component
            } else {
                new_image_data[pixel_index + 0] = red;     // Red component
                new_image_data[pixel_index + 1] = green;     // Green component
                new_image_data[pixel_index + 2] = blue;   // Blue component
                new_image_data[pixel_index + 3] = alpha;   // Alpha component
            }

            pos++;

            double percent = ((double)pos / ((double)width * (double)height)) * 100.0;
            
            progressBar((int)percent);

            // Output the color of the current pixel
            //printf("Pixel at (%d, %d): R=%d, G=%d, B=%d, A=%d\n", x, y, red, green, blue, alpha);
        }
    }

    // Write the image data to a PNG file
    char location[256];
    strcpy(location, "transparent/");

    char name[256];

    for (int i = 0; i < strlen(file_name); i++) {
        if (file_name[i] == '.') {
            break;
        }

        name[i] = file_name[i];
    }

    printf("\nWriting to new file...\n");

    if (!stbi_write_png(strcat(location, strcat(name, ".png")), width, height, 4, new_image_data, width * 4)) {
        printf("\x1b[31mError writing PNG file (%s)\x1b[0m\n", name);
        free(new_image_data);
        return;
    }

    // Free allocated memory
    free(new_image_data);

    // Free the image data
    stbi_image_free(image_data);

    clearLastLine();
    clearLastLine();
    clearLastLine();

    printf("%s - done\n", file_name);
}

int isImage(const char* file_name) {
    int length = strlen(file_name);

    for (int i = 0; i < length; i++) {
        if (file_name[i] == '.') {
            int exeLen = length - i - 1;
            char* exe = (char*)malloc((exeLen + 1) * sizeof(char));
            for (int j = i + 1; j < length; j++) {
                exe[j - (i + 1)] = file_name[j];
            }
            exe[exeLen] = '\0';
            for (int j = 0; j < supportedCount; j++) {
                if (strcmp(supported[j], exe) == 0) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

int readDir(const char* dir, int s) {
    char command[256];

    if (s == 0) {
        strcpy(command, "dir /b ");
    } else if (s == 1) {
        strcpy(command, "ls ");
    } else {
        printf("\x1b[31mUnknown system type\x1b[0m\n");
        return -1;
    }

    FILE *ls_output;
    char file_name[256];

    int location = 0;

    ls_output = popen(strcat(command, dir), "r");
    if (ls_output == NULL) {
        printf("\x1b[31mError opening pipe for \"%s %s\"\x1b[0m\n", command, dir);
        return -1;
    }

    //printf("Files in directory:\n");
    int imageFound = 1;

    while (fgets(file_name, sizeof(file_name), ls_output) != NULL) {
        // Remove the newline character if present
        if (file_name[strlen(file_name) - 1] == '\n') {
            file_name[strlen(file_name) - 1] = '\0';
        }

        if (isImage(file_name) != 0) {
            //printf(" - not an image\n");
            //printf("\n");
            continue;
        }

        if (imageFound == 1) {
            system("mkdir transparent");
        }

        imageFound = 0;

        printf("%s", file_name);

        printf("\n");

        updateImage(file_name);
        
    }

    pclose(ls_output);

    return imageFound;
}

int main(int argc, const char* argv[]) {
    int system = 0; // 0 - MS-DOS, 1 - Unix

    if (argc > 1) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-s") == 0) {
                for (int j = 0; j < supportedCount; j++) {
                    printf("%s\n", supported[j]);
                }
                return 0;
            }
        }
    }

    int ret = readDir(".\\", system);

    if (ret == 0) {
        printf("Success! The new images are stored in the subdirectory 'transparent'\n");
    } else if (ret == 1) {
        printf("Uh Oh! Looks like there isn't any supported images in this location!\nUse 'ClearCut -s' to view all supported image types\n");
    }

    return 0;
}