EXIF Reader

This is an Exif reader in C designed to take a JPG image as an input as a terminal paramter and print out the specifications
of the photo. This includes camera manufacturer, model, width in pixels, etc.

The command to run the program is of the form:
./exifview img1.jpg

Make sure to input an image as a parameter for the program to produce the desired output. 


The program works by reading through the Exif Header and upon confirming the necessary information(App1 tag, Endianness, etc.)
it reads through the individual Exif tags, looking for 3 specifically. These are the manufacturer tag, model tag, and the subblock tag. Upon reaching the sublock tag the program goes to the directed subblock within the file and looks at the Exif tags stored there. Upon retrieving the specs information from this section it prints all that it has discovered out. 
