How to Use:

1. Import BeeHandle
	#include "BeeHandle.cpp"

2. Create a BeeHandle
	BeeHandle bh = BeeHandle()
	
	Constructor: 	BeeHandle(
						int xwidth = 400, 				// Height of bee matrix
						int ywidth = 400, 				// Width of bee matrix
						int stepsize = 10, 				// How many units in the matrix a bee can move a single step
						double randomfactor = PI/4, 	// Random double [-pi,+pi]
						int numthreads = 4, 			// Number of threads/cores in machine
						double landing_percent = 0.0	// % of bees that have to land before updating attractors via safeReplaceAttractors
					)

3. Add points
	Option 1: addP()		// Adds a point with a random location
	Option 2: addP(point)	// Adds a point with coordinates defined by point object
		cv::Point point - openCV Point with defined x and y
	Option 3: addP(x, y)	// Adds a points with coordinates x and y
		int x - x coord
		int y - y coord
	
4. Add attractors
	Option 1 (recommended): bh.safeReplaceAttractors(new_attractors)
		std::vector<cv::Point> new_attractors - a vector of OpenCV Points
	Option 2: bh.replaceAArray(new_attractors)
		std::vector<cv::Point> new_attractors - a vector of OpenCV Points
	Option 3: bh.addA(point)
		cv::Point point - an OpenCV Point

5. Update Points
	bh.updatePoints()

6. Read new bee locations
	std::vector<cv::Point> points = bh.getPoints();