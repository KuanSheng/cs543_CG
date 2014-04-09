import ij.*;
import ij.plugin.filter.PlugInFilter;
import ij.process.*;
import java.awt.*;
import ij.gui.*;


public class Circular_Hough implements PlugInFilter {

    public int radiusMin;  
    public int radiusMax;  
    public int radiusInc; 
    public int maxCircles; 
    public int threshold = -1;
    byte imageValues[]; 
    double houghValues[][][]; 
    public int width; // Hough Space width (depends on image width)
    public int height;  // Hough Space heigh (depends on image height)
    public int depth;  // Hough Space depth (depends on radius interval)
    public int offset; // Image Width
    public int offx;   // ROI x offset
    public int offy;   // ROI y offset
    Point centerPoint[];
    double radius[] = new double[100];
    private int vectorMaxSize = 500;
    boolean useThreshold = false;
    int lut[][][];


    public int setup(String arg, ImagePlus imp) {
        if (arg.equals("about")) {
            showAbout();
            return DONE;
        }
        return DOES_ALL;
    }

    public void run(ImageProcessor ip) {

        imageValues = (byte[])ip.getPixels();
        Rectangle r = ip.getRoi();


        offx = r.x;
        offy = r.y;
        width = r.width;
        height = r.height;
        offset = ip.getWidth();


        if( readParameters() ) { 


            houghTransform();

           
           /* ImageProcessor newip = new ByteProcessor(width, height);
            byte[] newpixels = (byte[])newip.getPixels();
            createHoughPixels(newpixels);*/

            // Create image View for Marked Circles.
            ImageProcessor circlesip = new ByteProcessor(width, height);
            byte[] circlespixels = (byte[])circlesip.getPixels();

            // Mark the center of the found circles in a new image
            if(useThreshold)
                getCenterPointsByThreshold(threshold);
            else
                getCenterPoints(maxCircles);
            drawCircles(circlespixels,ip);

            //new ImagePlus("Hough Space [r="+radiusMin+"]", newip).show(); // Shows only the hough space for the minimun radius
            new ImagePlus(maxCircles+" Circles Found", circlesip).show();
        }
    }

    void showAbout() {
        IJ.showMessage("About Circles_...",
                       "This plugin finds n circles\n" +
                       "using a basic HoughTransform operator\n." +
                       "For better results apply an Edge Detector\n" +
                       "filter and a binarizer before using this plugin\n"+
                       "\nAuthor: Hemerson Pistori (pistori@ec.ucdb.br)"
                      );
    }

    boolean readParameters() {

        GenericDialog gd = new GenericDialog("Hough Parameters", IJ.getInstance());
        gd.addNumericField("Minimum radius (in pixels) :", 10, 0);
        gd.addNumericField("Maximum radius (in pixels)", 250, 0);
        gd.addNumericField("Increment radius (in pixels) :", 2, 0);
        gd.addNumericField("Number of Circles (NC): (enter 0 if using threshold)", 5, 0);
        gd.addNumericField("Threshold: (not used if NC > 0)", 60, 0);

      //  gd.showDialog();

        if (gd.wasCanceled()) {
            return(false);
        }

        radiusMin = (int) gd.getNextNumber();
        radiusMax = (int) gd.getNextNumber();
        radiusInc = (int) gd.getNextNumber();
        depth = ((radiusMax-radiusMin)/radiusInc)+1;
        maxCircles = (int) gd.getNextNumber();
        threshold = (int) gd.getNextNumber();
        if (maxCircles > 0) {
            useThreshold = false;
            threshold = -1;
        } else {
            useThreshold = true;
            if(threshold < 0) {
                IJ.showMessage("Threshold must be greater than 0");
                return(false);
            }
        }
        return(true);

    }

   
    private int buildLookUpTable() {

        int i = 0;
        int incDen = Math.round (8F * radiusMin);  // increment denominator

        lut = new int[2][incDen][depth];

        for(int radius = radiusMin;radius <= radiusMax;radius = radius+radiusInc) {
            i = 0;
            for(int incNun = 0; incNun < incDen; incNun++) {
                double angle = (2*Math.PI * (double)incNun) / (double)incDen;
                int indexR = (radius-radiusMin)/radiusInc;
                int rcos = (int)Math.round ((double)radius * Math.cos (angle));
                int rsin = (int)Math.round ((double)radius * Math.sin (angle));
                if((i == 0) | (rcos != lut[0][i][indexR]) & (rsin != lut[1][i][indexR])) {
                    lut[0][i][indexR] = rcos;
                    lut[1][i][indexR] = rsin;
                    i++;
                }
            }
        }

        return i;
    }

    private void houghTransform () {

        int lutSize = buildLookUpTable();

        houghValues = new double[width][height][depth];

        int k = width - 1;
        int l = height - 1;

        for(int y = 1; y < l; y++) {
            for(int x = 1; x < k; x++) {
                for(int radius = radiusMin;radius <= radiusMax;radius = radius+radiusInc) {
                    if( imageValues[(x+offx)+(y+offy)*offset] != 0 )  {// Edge pixel found
                        int indexR=(radius-radiusMin)/radiusInc;
                        for(int i = 0; i < lutSize; i++) {

                            int a = x + lut[1][i][indexR]; 
                            int b = y + lut[0][i][indexR]; 
                            if((b >= 0) & (b < height) & (a >= 0) & (a < width)) {
                                houghValues[a][b][indexR] += 1;
                            }
                        }

                    }
                }
            }

        }

    }


    // Convert Values in Hough Space to an 8-Bit Image Space.
    private void createHoughPixels (byte houghPixels[]) {
        double d = -1D;
        for(int j = 0; j < height; j++) {
            for(int k = 0; k < width; k++)
                if(houghValues[k][j][0] > d) {
                    d = houghValues[k][j][0];
                }

        }

        for(int l = 0; l < height; l++) {
            for(int i = 0; i < width; i++) {
                houghPixels[i + l * width] = (byte) Math.round ((houghValues[i][l][0] * 255D) / d);
            }

        }
    }

	// Draw the circles found in the original image.
	public void drawCircles(byte[] circlespixels, ImageProcessor ip) {

		// Copy original input pixels into output
		// circle location display image and
		// combine with saturation at 100
		int roiaddr=0;
		for( int y = offy; y < offy+height; y++) {
			for(int x = offx; x < offx+width; x++) {
				// Copy;
				circlespixels[roiaddr] = imageValues[x+offset*y];
				// Saturate
				if(circlespixels[roiaddr] != 0 )
					circlespixels[roiaddr] = 100;
				else
					circlespixels[roiaddr] = 0;
				roiaddr++;
			}
		}
		
		if(centerPoint == null) {
			if(useThreshold)
				getCenterPointsByThreshold(threshold);
			else
			getCenterPoints(maxCircles);
		}
		byte cor = -1;
		// Redefine these so refer to ROI coordinates exclusively
		int offset = width;
		int offx=0;
		int offy=0;

		for(int l = 0; l < maxCircles; l++) {
			int i = centerPoint[l].x;
			int j = centerPoint[l].y;
			double r = radius[l];
			// Draw a gray cross marking the center of each circle.
			for( int k = -10 ; k <= 10 ; ++k ) {
				int p = (j+k+offy)*offset + (i+offx);
				if(!outOfBounds(j+k+offy,i+offx))
					circlespixels[(j+k+offy)*offset + (i+offx)] = cor;
				if(!outOfBounds(j+offy,i+k+offx))
					circlespixels[(j+offy)*offset   + (i+k+offx)] = cor;
			}
			for( int k = -2 ; k <= 2 ; ++k ) {
				if(!outOfBounds(j-2+offy,i+k+offx))
					circlespixels[(j-2+offy)*offset + (i+k+offx)] = cor;
				if(!outOfBounds(j+2+offy,i+k+offx))
					circlespixels[(j+2+offy)*offset + (i+k+offx)] = cor;
				if(!outOfBounds(j+k+offy,i-2+offx))
					circlespixels[(j+k+offy)*offset + (i-2+offx)] = cor;
				if(!outOfBounds(j+k+offy,i+2+offx))
					circlespixels[(j+k+offy)*offset + (i+2+offx)] = cor;
			}
			drawCircle(ip ,  i, j, r);
		}
	}


    private boolean outOfBounds(int y,int x) {
        if(x >= width)
            return(true);
        if(x <= 0)
            return(true);
        if(y >= height)
            return(true);
        if(y <= 0)
            return(true);
        return(false);
    }

    public Point nthMaxCenter (int i) {
        return centerPoint[i];
    }


    /** Search for a fixed number of circles.

    @param maxCircles The number of circles that should be found.  
    */
    private void getCenterPoints (int maxCircles) {


        centerPoint = new Point[maxCircles];
        int xMax = 0;
        int yMax = 0;
        int rMax = 0;




        for(int c = 0; c < maxCircles; c++) {
            double counterMax = -1;
            for(int radius = radiusMin;radius <= radiusMax;radius = radius+radiusInc) {


                int indexR = (radius-radiusMin)/radiusInc;
                for(int y = 0; y < height; y++) {
                    for(int x = 0; x < width; x++) {
                        if(houghValues[x][y][indexR] > counterMax) {
                            counterMax = houghValues[x][y][indexR];
                            xMax = x;
                            yMax = y;
                            rMax = radius;
                        }
                    }

                }
            }

            centerPoint[c] = new Point (xMax, yMax);
            radius[c] = rMax;
            clearNeighbours(xMax,yMax,rMax);
        }
    }


   
    private void getCenterPointsByThreshold (int threshold) {

        centerPoint = new Point[vectorMaxSize];
        int xMax = 0;
        int yMax = 0;
        int countCircles = 0;

        for(int radius = radiusMin;radius <= radiusMax;radius = radius+radiusInc) {
            int indexR = (radius-radiusMin)/radiusInc;
            for(int y = 0; y < height; y++) {
                for(int x = 0; x < width; x++) {



                    if(houghValues[x][y][indexR] > threshold) {


                        if(countCircles < vectorMaxSize) {


                            centerPoint[countCircles] = new Point (x, y);

                            clearNeighbours(xMax,yMax,radius);

                            ++countCircles;
                        } else
                            break;
                    }
                }
            }
        }

        maxCircles = countCircles;
    }

    /** Clear, from the Hough Space, all the counter that are near (radius/2) a previously found circle C.
        
    
    */
    private void drawCircle(ImageProcessor ip, double x, double y, double r){
    	ip.setLineWidth(2);
    	ip.setColor(250);
    	
    	int n = 400;
    	double dtheta = (Math.PI * 2)/n;
    	
    	int ix = (int)Math.round(x+r);
    	int iy = (int)Math.round(y);
    	
    	ip.moveTo(ix,iy);
    	
    	for(int i = 1;i<=n;i++){
    		double theta = i*dtheta;
    		double x0 = x+r * Math.cos(theta);
    		double y0 = y+r * Math.sin(theta);
    		
    		ix = (int)Math.round(x0);
    		iy = (int)Math.round(y0);
    		
    		ip.lineTo(ix,iy);
    		
    	}
    	
    }
    private void clearNeighbours(int x,int y, int radius) {


        // The following code just clean the points around the center of the circle found.


        double halfRadius = radius / 2.0F;
	double halfSquared = halfRadius*halfRadius;


        int y1 = (int)Math.floor ((double)y - halfRadius);
        int y2 = (int)Math.ceil ((double)y + halfRadius) + 1;
        int x1 = (int)Math.floor ((double)x - halfRadius);
        int x2 = (int)Math.ceil ((double)x + halfRadius) + 1;



        if(y1 < 0)
            y1 = 0;
        if(y2 > height)
            y2 = height;
        if(x1 < 0)
            x1 = 0;
        if(x2 > width)
            x2 = width;



        for(int r = radiusMin;r <= radiusMax;r = r+radiusInc) {
            int indexR = (r-radiusMin)/radiusInc;
            for(int i = y1; i < y2; i++) {
                for(int j = x1; j < x2; j++) {	      	     
                    if(Math.pow (j - x, 2D) + Math.pow (i - y, 2D) < halfSquared) {
                        houghValues[j][i][indexR] = 0.0D;
                    }
                }
            }
        }

    }

}
