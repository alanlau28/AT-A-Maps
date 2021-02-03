/* 
 * Copyright 2021 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <math.h>
#include "m1.h"
#include "StreetsDatabaseAPI.h"

std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;

// loadMap will be called with the name of the file that stores the "layer-2"
// map data accessed through StreetsDatabaseAPI: the street and intersection 
// data that is higher-level than the raw OSM data). 
// This file name will always end in ".streets.bin" and you 
// can call loadStreetsDatabaseBIN with this filename to initialize the
// layer 2 (StreetsDatabase) API.
// If you need data from the lower level, layer 1, API that provides raw OSM
// data (nodes, ways, etc.) you will also need to initialize the layer 1 
// OSMDatabaseAPI by calling loadOSMDatabaseBIN. That function needs the 
// name of the ".osm.bin" file that matches your map -- just change 
// ".streets" to ".osm" in the map_streets_database_filename to get the proper
// name.
bool loadMap(std::string map_streets_database_filename) {
    bool load_successful = false; //Indicates whether the map has loaded 
                                  //successfully

    std::cout << "loadMap: " << map_streets_database_filename << std::endl;

    //
    // Load your map related data structures here.
    //

    //traverse through all intersections
    for(int intersection = 0; intersection < getNumIntersections(); intersection++){
        std::vector<StreetSegmentIdx> street_segment_index; //create empty vector
        
        //traverse through street segment of each intersection
        for(int street_segment_num = 0; street_segment_num < getNumIntersectionStreetSegment(intersection);street_segment_num++){
            street_segment_index.push_back(getIntersectionStreetSegment(intersection,street_segment_num)); //push back street segment id into vector
        }
        
        intersection_street_segments.push_back(street_segment_index);//push back vector into intersection_street_segment
    }
    
    //unordered/ other data structures
    
    
    
    
    
    
    
    
    
    
    

    load_successful = true; //Make sure this is updated to reflect whether
                            //loading the map succeeded or failed

    return load_successful;
}
//LatLon         getIntersectionPosition(IntersectionIdx intersectionIdx)
// Returns the distance between two (lattitude,longitude) coordinates in meters
// Speed Requirement --> moderate
double findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> points){
    double x1,y1,x2,y2,lat1,lat2,lat_avg,distance;
    lat1 = points.second.latitude() * kDegreeToRadian;
    lat2 = points.first.latitude() * kDegreeToRadian;
    lat_avg = (lat1 + lat2)/2.0;
    
    x1 = points.first.longitude() * cos(lat_avg) * kDegreeToRadian;
    y1 = points.first.latitude() * kDegreeToRadian;
    x2 = points.second.longitude() * cos(lat_avg) * kDegreeToRadian;
    y2 = points.second.latitude() * kDegreeToRadian;
    distance = kEarthRadiusInMeters *  sqrt(pow(y2-y1,2)+ pow(x2-x1,2));
    return distance;
}



// Returns the length of the given street segment in meters
// Speed Requirement --> moderate
double findStreetSegmentLength(StreetSegmentIdx street_segment_id){
    //StreetSegmentInfo getStreetSegmentInfo(StreetSegmentIdx streetSegmentIdx);
    //LatLon         getIntersectionPosition(IntersectionIdx intersectionIdx);
    LatLon point1,point2;
    double length;
    struct StreetSegmentInfo street_info = getStreetSegmentInfo(street_segment_id);
    point1 = getIntersectionPosition(street_info.from);
    point2 = getIntersectionPosition(street_info.to);
    std::pair<LatLon,LatLon> points (point1,point2);
    length = findDistanceBetweenTwoPoints(points);
    return length;
}



double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id){
    // Note: (time = distance/speed_limit) double / float
    double distance,time;
    float speed_limit;
    struct StreetSegmentInfo street_info = getStreetSegmentInfo(street_segment_id);
    distance = findStreetSegmentLength(street_segment_id);
    speed_limit = street_info.speedLimit;
    time = distance/speed_limit;
    return time;
}


























































































































































//end Alan







































































































































































































//end Alex







































































































































































































//end Tawseef
void closeMap() {
    //Clean-up your map related data structures here
    
}
