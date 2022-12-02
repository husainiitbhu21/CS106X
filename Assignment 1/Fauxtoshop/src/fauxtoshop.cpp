#include <iostream>
#include <stdlib.h>
#include "console.h"
#include "gwindow.h"
#include "grid.h"
#include "simpio.h"
#include "strlib.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "math.h" //for sqrt and exp in the optional Gaussian kernel
#include "gmath.h" // for sinDegrees(), cosDegrees(), PI
#include "tokenscanner.h"
using namespace std;

static const int    WHITE = 0xFFFFFF;
static const int    BLACK = 0x000000;
static const int    GREEN = 0x00FF00;

struct pixel{
    int row;
    int col;
};

struct coordinate{
    int x;
    int y;
};

struct RGB_Colour{
    int red;
    int green;
    int blue;
};


void doFauxtoshop(GWindow &gw, GBufferedImage &img);

int getimage_name(GBufferedImage &img);
int FilterPrompt();
int jump(int x, GBufferedImage &img);
void Save_Image(GBufferedImage &img);

void Scatter_Image(GBufferedImage &img);
pixel random_pixel(int Row, int Col, int degree_scatter, GBufferedImage &img);

void Edge_Detection(GBufferedImage &img);
RGB_Colour ConvertPixelColorToRGB(int pix);
int MaxColour_Difference(const RGB_Colour &x1, const RGB_Colour &x2);
int Max_NeighbourDifference(int row, int col, Grid<int> &grid_image);

void Green_Screen(GBufferedImage &img);
pixel pixel_prompt(string & prompt);
pixel StringtoPoint(string s);

void Compare_Image(GBufferedImage &img);

void Rotation(GBufferedImage &img);
coordinate rotate_coordinate(coordinate a, int angle);
pixel convertCoordinatetoPixel(coordinate a, GBufferedImage &img);
coordinate convertPixeltoCoordinate(pixel p, GBufferedImage &img);

void Gaussian_Blur(GBufferedImage &img);
int getWeightedColor(int pixel, double weight);

bool openImageFromFilename(GBufferedImage& img, string filename);
bool saveImageToFilename(const GBufferedImage &img, string filename);
void getMouseClickLocation(int &row, int &col);
Vector<double> gaussKernelForRadius(int radius);
int valuePrompt(string & prompt, int lower, int upper);

/* STARTER CODE FUNCTION - DO NOT EDIT
 *
 * This main simply declares a GWindow and a GBufferedImage for use
 * throughout the program. By asking you not to edit this function,
 * we are enforcing that the GWindow have a lifespan that spans the
 * entire duration of execution (trying to have more than one GWindow,
 * and/or GWindow(s) that go in and out of scope, can cause program
 * crashes).
 */
int main() {
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setVisible(true);
    GBufferedImage img;
    doFauxtoshop(gw, img);
    return 0;
}

/* This is yours to edit. Depending on how you approach your problem
 * decomposition, you will want to rewrite some of these lines, move
 * them inside loops, or move them inside helper functions, etc.
 *
 * TODO: rewrite this comment.
 */
void doFauxtoshop(GWindow &gw, GBufferedImage &img) {
    cout << "Welcome to Fauxtoshop!" << endl;

    while(getimage_name(img)) {
        gw.setCanvasSize(img.getWidth(), img.getHeight());
        gw.add(&img,0,0);


        while(jump(FilterPrompt(), img)) {
            gw.clear();
            cout << endl;
        }

    }
    cout << "\nExiting Fauxtoshop. Goodbye... " << endl;
}

/*Prompts the user to enter an image name or "" to quit
Returns 1 if image was successfully opened
Returns 0 if user entered " " and would like to quit program*/
int getimage_name(GBufferedImage &img) {
    while(true) {
        string name_image = getLine("Enter name of image file to open (or blank to quit):");

        if(name_image == "") {
            return 0;
        }
        else if(openImageFromFilename(img, name_image)) {
            cout << "Opening image file. It may take a minute..." << endl;
            return 1;
        }
        else {
            cout << "Wrong file name... Please try again or enter blank to quit" << endl;
        }
    }
}


/*Prompts user to choose a filter of their choice
 * Returns the integer coressponding to the filter */
int FilterPrompt() {
    cout << "Which image filter would you like to apply?" << endl;
        cout << "\t1 - Scatter\n";
        cout << "\t2 - Edge detection\n";
        cout << "\t3 - Green screen with another image\n";
        cout << "\t4 - Compare image with another image\n";
        cout << "\t5 - Rotation\n";
        cout << "\t6 - Gaussian Blur\n";
        cout << "\t0 - Quit\n";
        return  getInteger("\nYour choice: ");
}


/*Jump table to execute filter function based on user selection
Returns 1 and prints out error if input is outside range
Returns 0 if user's choice is to quit*/
int jump(int x, GBufferedImage &img) {
    if((x < 0)||(x > 6)) {
        cout << "\nINPUT ERROR: Enter a number between 0 and 6." <<endl;
        return 1;
    }
    else if(x == 0) {
        return 0;
    }
    else if(x == 1) {
        Scatter_Image(img);
        Save_Image(img);
        return 0;
    }
    else if(x == 2) {
        Edge_Detection(img);
        Save_Image(img);
        return 0;
    }
    else if(x == 3) {
        Green_Screen(img);
        Save_Image(img);
        return 0;
    }
    else if(x == 4) {
        Compare_Image(img);
        Save_Image(img);
        return 0;
    }
    else if(x == 5) {
        Rotation(img);
        Save_Image(img);
        return 0;
    }
    else if(x == 6) {
        Gaussian_Blur(img);
        Save_Image(img);
        return 0;
    }
    else {
        cout << "Not a valid option." << endl;
        return 0;
    }

}


/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to open the image file 'filename'.
 *
 * This function returns true when the image file was successfully
 * opened and the 'img' object now contains that image, otherwise it
 * returns false.
 */
bool openImageFromFilename(GBufferedImage& img, string filename) {
    try { img.load(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to save the image file to 'filename'.
 *
 * This function returns true when the image was successfully saved
 * to the file specified, otherwise it returns false.
 */
bool saveImageToFilename(const GBufferedImage &img, string filename) {
    try { img.save(filename); }
    catch (...) { return false; }
    return true;
}


/*Prompts user to enter a filename to save editted image
 Exits if user enters "" and saves image to file if a correct image name is entered*/
void Save_Image(GBufferedImage &img) {
    string filename;
    do {
        filename = getLine("Enter a filename to save image (or blank to skip saving): ");
        if(filename == "") {
            return;
        }
    } while(!saveImageToFilename(img, filename));
}


/* Prompts the user to enter a number between a lowerBound and upperBound (inclusive) specified
 Function returns the user's choice if a correct input is given, otherwise user is reprompted */
int valuePrompt(string & prompt, int lower, int upper) {
    int choice;
    do{
        choice = getInteger(prompt, "\nINPUT ERROR: " + prompt);
    } while((choice < lower) || (choice > upper));
    return choice;
}


/*-------------Scatter----------------------*/

/*This functions takes a GBufferedImage and GWindow and “scatters” its pixels,
 * making something that looks like a sand drawing that was shaken.
The function prompts the user to provide a “degree of scatter”
for how far we should scatter pixels.
The value should be an integer between 1 and 100, inclusive (otherwise reprompt).*/
void Scatter_Image(GBufferedImage &img) {
    string s1 = "Enter degree of scatter [1-100]: ";
    int degree_scatter = valuePrompt(s1, 1 , 100);

    Grid<int> grid_image = img.toGrid();
    for(int r = 0; r < img.getHeight(); r++){
          for(int c = 0; c < img.getWidth(); c++){
              pixel rp = random_pixel(r, c, degree_scatter, img);
              grid_image[r][c] = grid_image[rp.row][rp.col];
           }
     }

     img.fromGrid(grid_image);/*Instead, make all the changes you want in a Grid object, and then change the image all at once by calling the "img.fromGrid"
     (where img is the name of your GBufferedImage instance), which takes a Grid as its argument.*/
}


/*Returns a randomly selected pixel that exists within the image bounds.
 The pixel is returned by randomly selecting a row within radius
 of the center pixel's row, and randomly selecting a column within radius of
 the center pixel's column.*/
pixel random_pixel(int Row, int Col, int radius, GBufferedImage &img) {
      int x, y;

      do {
      x = randomInteger(Row - radius, Row + radius);
      y = randomInteger(Col - radius, Col + radius);
      } while((x < 0)||(y < 0)||(x > (img.getHeight()-1))||(y > (img.getWidth()-1)));

      pixel k = {x, y};
      return k;
}


/*---------------Edge Detection----------------------*/

/*For this filter, your program will create a new black and white image of the same size as the original, where a given pixel is
black if it was an edge in the original image, and white if it was not an edge in the original image.
First, ask the user for a threshold that controls how different two adjacent pixels must be from each other to be considered
an “edge.” This should be a positive (nonzero) integer value (otherwise re-prompt).*/
void Edge_Detection(GBufferedImage &img) {
    string s2 = "Enter threshold for edge detection: ";
    int threshold = valuePrompt(s2, 1, 10000);


    Grid<int> copy_Grid = img.toGrid();
    Grid<int> grid_image(img.getHeight(), img.getWidth());


    for(int r = 0; r < img.getHeight(); r++){
          for(int c = 0; c < img.getWidth(); c++){
               if(Max_NeighbourDifference(r, c, copy_Grid) > threshold) {
                   grid_image[r][c] = BLACK;
               }
               else {
                   grid_image[r][c] = WHITE;
               }
          }
    }

    img.fromGrid(grid_image);
}


/*Converts an int pixel colour (type int) to an RGBColour type*/
RGB_Colour ConvertPixelColorToRGB(int pixel) {
        RGB_Colour colour;
        GBufferedImage::getRedGreenBlue(pixel, colour.red, colour.green, colour.blue);
        return colour;
}


/*Calculates and returns the maximum difference between two RGB colours*/
int MaxColour_Difference(const RGB_Colour &x1, const RGB_Colour &x2) {
       int dRed, dGreen, dBlue;
       dRed = abs(x1.red - x2.red);
       dGreen = abs(x1.green - x2.green);
       dBlue = abs(x1.blue - x2.blue);

       return max(dGreen, max(dRed, dBlue));
}


/*Calculates maximum difference around neighbors*/
int Max_NeighbourDifference(int row, int col, Grid<int> &grid_image) {
         RGB_Colour center = ConvertPixelColorToRGB(grid_image[row][col]);
         RGB_Colour neighbour;
         int max_diff = 0;

         for(int i = row -1;i <= row+1;i++) {
             for(int j = col-1;j <= col+1;j++) {
                 if(grid_image.inBounds(i, j)){
                     neighbour = ConvertPixelColorToRGB(grid_image[i][j]);
                     if(MaxColour_Difference(center, neighbour) > max_diff) {
                         max_diff = MaxColour_Difference(center, neighbour);
                     }
                 }
             }
         }
         return max_diff;
}


/*-------------------Green Screen----------------------*/

/*This function pastes a “sticker” image on top of a “background” image,
 * but ignores any part of the sticker that is close to pure green in color*/
void Green_Screen(GBufferedImage &img) {
      Grid<int> grid_image = img.toGrid();
      GBufferedImage sticker;
      string filename;
      do {
         filename = getLine("Enter a name of sticker image file to open: ");
      } while(!openImageFromFilename(sticker, filename));
      Grid<int> sticker_Grid = sticker.toGrid();

      const RGB_Colour Green = ConvertPixelColorToRGB(GREEN);

      string s3 = "Now choose a tolerance threshold [1-100]: ";
      int threshold = valuePrompt(s3, 1, 100);

      string s = "Enter location to place image as \"(row,col)\" (or blank to use mouse): ";
      pixel p = pixel_prompt(s);

      for(int r = 0; r < sticker.getHeight(); r++){
              for(int c = 0; c < sticker.getWidth(); c++){
                  if(!grid_image.inBounds(p.row + r,p.col+c)) {
                      break;
                  }
                  RGB_Colour colour = ConvertPixelColorToRGB(sticker_Grid[r][c]);
                  if(MaxColour_Difference(colour, Green) > threshold){
                      grid_image[p.row+r][p.col+c] = sticker_Grid[r][c];
                  }
               }
      }

      img.fromGrid(grid_image);
}


/*Prompts the user to enter a location to place 'sticker'.
 * This can be done in two ways: 1) by typing in the "(row,col)" exactly in that format
 * ex (100,150) or 2) by entering "" and then clicking on the background image*/
pixel pixel_prompt(string & prompt) {
    pixel p;
    while(true) {
        string s = getLine(prompt);
        if(s == ""){
            cout << "Now click the background image to place new image" << endl;
            getMouseClickLocation(p.row, p.col);
            cout << "You chose: (" << p.row << "," << p.col << ")" << endl;
            break;
        }
        else {
            p = StringtoPoint(s);
            break;
        }
    }

    return p;
}


/*Converts a string input to a pixel type
 * Ex "(x,y)" -> p.row = x & p.col = y*/
pixel StringtoPoint(string s) {
       pixel p;
       Vector<int> numbers;
       TokenScanner scanner(s);
       while(scanner.hasMoreTokens()){
           string token = scanner.nextToken();
           TokenType type = scanner.getTokenType(token);
           if(type == NUMBER){
                   numbers.push_back(stringToInteger(token));
           }
       }
       if(numbers.size() == 2){
           string pixelStr = "(" + integerToString(numbers[0]) + "," + integerToString(numbers[1]) + ")";
           if (s == pixelStr) {
                  p = {numbers[0], numbers[1]};
                  return p;
           }
       }
       error("stringToPoint: Illegal input format");

       return p;
}


/*-------------------Compare Images----------------------*/

/*For this menu option, ask the user to name another image file
 * (open it as a GBufferedImage in the same way described in Green Screen section),
 * and you will count how many pixels differ between the two.
 * This would be easy to do by iterating over the pixels yourself, but it’s actually
 *even easier than that—there is a countDiffPixels method in the GBufferedImage class
 *that does all the work for you.
 *Use it, then report the result to the user.
 *Print a nonzero count as “These images differ in _ pixel locations!”
 *or print “These images are the same!” as applicable.
 *You will continue to display the original image.
 *(It will be a bit superfluous when the main menu asks the user
 *if they want to save the resulting image in this case, but that’s okay.) */
void Compare_Image(GBufferedImage &img) {
        GBufferedImage image_2;
        string s;

        do {
             s = getLine("Enter the filename for comparision: ");
        } while(!openImageFromFilename(image_2, s));

        int Diff_in_pixels = img.countDiffPixels(image_2);

        if(Diff_in_pixels == 0) {
           cout << "These images are the same!" << endl;
        }
        else {
           cout << "These images differ in " << Diff_in_pixels << " pixel locations!" << endl;
        }
}


/*---------------Rotation----------------------*/

/*Rotates input image by a given angle
 * between -360 and 360 degrees*/
void Rotation(GBufferedImage &img) {
     Grid<int> grid_image = img.toGrid();
     Grid<int> rotated_image(img.getHeight(), img.getWidth(), WHITE);

     string s4 = "Please enter an angle in degrees: ";
     int angle = valuePrompt(s4, -360, 360);

     for(int r = 0; r < img.getHeight(); r++) {
             for(int c = 0; c < img.getWidth(); c++) {
                     pixel p1;
                     p1.row = r;
                     p1.col = c;
                     coordinate new2 = rotate_coordinate(convertPixeltoCoordinate(p1, img), angle);
                     pixel new1 = convertCoordinatetoPixel(new2, img);
                     if(grid_image.inBounds(new1.row, new1.col)) {
                         rotated_image[r][c] = grid_image[new1.row][new1.col];
                     }
             }
     }

     img.fromGrid(rotated_image);
}


/*Rotates coordinate by input angle*/
coordinate rotate_coordinate(coordinate a, int angle) {
     coordinate alpha;
     alpha.x =  a.x * cosDegrees(angle) - a.y * sinDegrees(angle);
     alpha.y =  a.x * sinDegrees(angle) + a.y * cosDegrees(angle);
     return alpha;
}


/*Converts coordinate type to pixel type
 * Exception is thrown if incorrect input is passed*/
pixel convertCoordinatetoPixel(coordinate a, GBufferedImage &img) {
     pixel beta;
     beta.row = ceil(a.x) + img.getHeight()/2;
     beta.col = ceil(a.y) + img.getWidth()/2;
     return beta;
}


/*Converts pixel type to coordinate type
 * Exception is thrown if incorrect input is passed*/
coordinate convertPixeltoCoordinate(pixel p, GBufferedImage &img) {
     coordinate alpha;
     alpha.x = p.row - img.getHeight()/2;
     alpha.y = p.col - img.getWidth()/2;
     return alpha;
}


/*-------------------Guassian Blur----------------------*/

/* At every pixel, we’ll perform some math operation
 * involving the values in the kernel matrix
 * and the values of a pixel and its surroundings
 * to determine the value for a pixel in the output image.
 * Refer to "https://aryamansharda.medium.com/image-filters-gaussian-blur-eb36db6781b1" */
void Gaussian_Blur(GBufferedImage &img) {
     Grid<int> grid_image = img.toGrid();
     Grid<int> Blurred_Grid(img.getHeight(), img.getWidth(), 0); // Final Output
     Grid<int> temp(img.getHeight(), img.getWidth(), 0); // Temperory variable

     string s5 = "Enter radius for Gaussian Blur that is greater than 0: ";
     int radius = valuePrompt(s5, 1, 10000);

     const Vector <double> kernel = gaussKernelForRadius(radius);
     int sum = 0;


     /*Rows*/
     for(int r = 0; r < img.getHeight(); r++) {
         for(int c = 0; c < img.getWidth(); c++) {
             for(int i = -radius; i <= radius; i++) {
                  if(grid_image.inBounds(r,c+i)) {
                      sum += getWeightedColor(grid_image[r][c+i], kernel[i+radius]);
                  }
                  else {
                      sum += getWeightedColor(grid_image[r][c-i], kernel[i+radius]);
                  }
             }
             temp[r][c] = sum;
             sum = 0;
          }
      }

      /*Columns*/
      for(int c = 0; c < img.getWidth(); c++) {
          for(int r = 0; r < img.getHeight(); r++) {
              for(int j = -radius; j <= radius; j++) {
                  if(grid_image.inBounds(r+j,c)) {
                      sum += getWeightedColor(temp[r+j][c], kernel[j+radius]);
                  }
                  else {
                      sum += getWeightedColor(temp[r-j][c], kernel[j+radius]);
                  }
              }
              Blurred_Grid[r][c] = sum;
              sum = 0;
          }
      }

      img.fromGrid(Blurred_Grid);
}


/*Converts pixel color to rgb and multiplies by a weight*/
int getWeightedColor(int pixel, double w) {
    RGB_Colour colour = ConvertPixelColorToRGB(pixel);
    colour.red = (int)(colour.red * w);
    colour.green = (int)(colour.green * w);
    colour.blue = (int)(colour.blue * w);
    return GBufferedImage::createRgbPixel(colour.red, colour.green, colour.blue);
}


/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Waits for a mouse click in the GWindow and reports click location.
 *
 * When this function returns, row and col are set to the row and
 * column where a mouse click was detected.
 */
void getMouseClickLocation(int &row, int &col) {
    GMouseEvent me;
    do {
        me = getNextEvent(MOUSE_EVENT);
    } while (me.getEventType() != MOUSE_CLICKED);
    row = me.getY();
    col = me.getX();
}

/* HELPER FUNCTION
 *
 * This is a helper function for the Gaussian blur option.
 *
 * The function takes a radius and computes a 1-dimensional Gaussian blur kernel
 * with that radius. The 1-dimensional kernel can be applied to a
 * 2-dimensional image in two separate passes: first pass goes over
 * each row and does the horizontal convolutions, second pass goes
 * over each column and does the vertical convolutions. This is more
 * efficient than creating a 2-dimensional kernel and applying it in
 * one convolution pass.
 *
 * This code is based on the C# code posted by Stack Overflow user
 * "Cecil has a name" at this link:
 * http://stackoverflow.com/questions/1696113/how-do-i-gaussian-blur-an-image-without-using-any-in-built-gaussian-functions
 *
 */
Vector<double> gaussKernelForRadius(int radius) {
    if (radius < 1) {
        Vector<double> empty;
        return empty;
    }
    Vector<double> kernel(radius * 2 + 1);
    double magic1 = 1.0 / (2.0 * radius * radius);
    double magic2 = 1.0 / (sqrt(2.0 * PI) * radius);
    int r = -radius;
    double div = 0.0;
    for (int i = 0; i < kernel.size(); i++) {
        double x = r * r;
        kernel[i] = magic2 * exp(-x * magic1);
        r++;
        div += kernel[i];
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel[i] /= div;
    }
    return kernel;
}
