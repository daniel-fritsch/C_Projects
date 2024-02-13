#include <stdio.h>
#include <stdlib.h>

// TIFF header structure definition
struct TIFF_Header {
    unsigned char file_marker[2];
    unsigned char app1_marker[2];
    unsigned short app1_length;
    unsigned char exif_string[4];
    unsigned short nul_term;
    unsigned char endianness[2];
    unsigned short versionNum;
    unsigned int offset;
};

// TIFF tag structure definition
struct TIFF_Tag {
    unsigned short tag_ID;
    unsigned short data_type;
    unsigned int items;
    unsigned int val_offset;
};

// Checks if there is actually a file given as an argument to program
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("A file input is required! ");
        return 1;
    }

    // Opens File
    FILE *file = fopen(argv[1], "rb");
    

    // Reads TIFF header from the first segment of the file 
    struct TIFF_Header header;
    size_t header_size = sizeof(struct TIFF_Header);
    size_t elements_read = fread(&header, header_size, 1, file);
    if (elements_read != 1) {
        printf("There was a problem reading the TiFF header.\n");
        fclose(file);
        return 1;
    }
    
    // Necessary Checks upon TIFF Header

    // checks  for jpeg start of file marker
    if (header.file_marker[0] != 0xFF || header.file_marker[1] != 0xD8){
        printf("File marker is not equal to 0xFF 0xD8.\n");
        return 1;
    }

    // checks for app1 marker
    if (header.app1_marker[0] != 0xFF || header.app1_marker[1] != 0xE1) {
        printf("App 1 market not present.\n");
        return 1;
    }
    
    // check exif string 
    if (header.exif_string[0] != 'E' 
        || header.exif_string[1] != 'x'
        || header.exif_string[2] != 'i'
        || header.exif_string[3] != 'f'){
        printf("Exif invalid");
        return 1;
    }

    // endianess check
    if (header.endianness[0] != 'I' || header.endianness[1] != 'I'){
        printf("Endiannes Error");
        return 1;
    }

    //determining number of exif tags
    unsigned short num_exif_tags;
    fread(&num_exif_tags, sizeof(unsigned short),1 ,file);


    //exif tag analysis section

    // reads a TIFF Tag and checks if there was error
    int size_tag;
    for (int i = 0; i < num_exif_tags; i++) {
        struct TIFF_Tag tag;
        size_t tag_size = sizeof(struct TIFF_Tag);
        size_t elements_read = fread(&tag, tag_size, 1, file);

        if (elements_read != 1) {
            printf("Error reading tag");
            fclose(file);
            return 1;
        }

        

        // manufacturer tag
        if (tag.tag_ID == 0x010F && tag.data_type==2){
            long location = ftell(file);
            char manufacturer[tag.items]; 
            fseek(file, tag.val_offset+12, SEEK_SET);
            fread(manufacturer, sizeof(char), tag.items, file);
            
            printf("Manufacturer:    %s\n", manufacturer);
            fseek(file, location, SEEK_SET);
        }

        // Camera model
        if (tag.tag_ID == 0x0110 && tag.data_type==2){
            long location = ftell(file);
            char cam_model[tag.items]; 
            fseek(file, tag.val_offset+12, SEEK_SET);
            fread((cam_model), sizeof(char), tag.items, file);
            
            printf("Camera model:    %s\n", cam_model);
            fseek(file, location, SEEK_SET);
        }

        // Sub-block
        if (tag.tag_ID == 0x8769 && tag.data_type==4){
            unsigned short sub_block_exif_tags;
            long location = ftell(file);
            if (fseek(file, tag.val_offset+12, SEEK_SET) != 0) {
                printf("Error seeking to the subblock position.\n");
                fclose(file);
                return 1;
            }
            fread(&sub_block_exif_tags, sizeof(unsigned short),1 ,file);

            // loop through subblocks same way as before
            for (int i = 0; i < sub_block_exif_tags; i++) {
                struct TIFF_Tag tag;
                size_t tag_size = sizeof(struct TIFF_Tag);
                size_t elements_read = fread(&tag, tag_size, 1, file);

                if (elements_read != 1) {
                    printf("Error reading tag %d.\n", i + 1);
                    fclose(file);
                    return 1;
                }

                if (tag.tag_ID == 0xA002 && tag.data_type==4){  // width
                    printf("Width:           %d pixels\n", tag.val_offset); 
                } else if (tag.tag_ID == 0xA003 && tag.data_type==4){ // height
                    printf("Height:          %d pixels\n", tag.val_offset); 
                } else if (tag.tag_ID == 0x8827 && tag.data_type==3){  // ISO
                    printf("ISO:             ISO %hd\n", tag.val_offset); 
                } else if (tag.tag_ID == 0x829A && tag.data_type==5){ // Exposure Time
                    long location = ftell(file);
                    fseek(file, tag.val_offset+12, SEEK_SET);
                    unsigned int num1, num2;
                    fread(&num1, sizeof(unsigned int), 1, file);
                    fread(&num2, sizeof(unsigned int), 1, file);
                    printf("Exposure Time:   %d/%d second\n", num1, num2); //

                    fseek(file, location, SEEK_SET);
                } else if (tag.tag_ID == 0x829D && tag.data_type==5){ // F-Stop
                    long location = ftell(file);
                    fseek(file, tag.val_offset+12, SEEK_SET);
                    unsigned int num1, num2;
                    fread(&num1, sizeof(unsigned int), 1, file);
                    fread(&num2, sizeof(unsigned int), 1, file);
                    float f_stop = num1/num2;
                    printf("F-stop:          f/%.1f\n", f_stop);

                    fseek(file, location, SEEK_SET);
                } else if (tag.tag_ID == 0x920A && tag.data_type==5){ // Focal Length
                    long location = ftell(file);
                    fseek(file, tag.val_offset+12, SEEK_SET);
                    unsigned int num1, num2;
                    fread(&num1, sizeof(unsigned int), 1, file);
                    fread(&num2, sizeof(unsigned int), 1, file);
                    int focal_len = num1/num2;
                    printf("Focal Length:     %d mm\n", focal_len);

                    fseek(file, location, SEEK_SET);
                } else if (tag.tag_ID == 0x9003 && tag.data_type==2){ // Data Taken
                    long location = ftell(file);
                    char data_taken[tag.items]; 
                    fseek(file, tag.val_offset+12, SEEK_SET);
                    fread((data_taken), sizeof(char), tag.items, file);
                    
                    printf("Data Taken:      %s\n", data_taken);
                    fseek(file, location, SEEK_SET);
                } 


            }

            // Seeks back to before 0x8769 once it is finished reading blocks
            fseek(file, location, SEEK_SET);
        }
    }

    // file close and return
    fclose(file);
    return 0;
}

