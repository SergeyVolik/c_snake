#ifndef IMAGE_H
#define IMAGE_H

int png_sig_cmp(char* header);
void read_png_file(char* file_path);

#endif