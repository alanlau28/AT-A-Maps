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
#include <map>
#include <algorithm>
#include "m1.h"
#include "StreetsDatabaseAPI.h"

std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;
std::vector<std::vector<double>> street_segment_travel_times; 

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

    load_successful = loadStreetsDatabaseBIN(map_streets_database_filename);
    //
    // Load your map related data structures here.
    //

    //traverse through all intersections
    for (int intersection = 0; intersection < getNumIntersections(); intersection++) {
        std::vector<StreetSegmentIdx> street_segment_index; //create empty vector

        //traverse through street segment of each intersection
        for (int street_segment_num = 0; street_segment_num < getNumIntersectionStreetSegment(intersection); street_segment_num++) {
            street_segment_index.push_back(getIntersectionStreetSegment(intersection, street_segment_num)); //push back street segment id into vector
        }

        intersection_street_segments.push_back(street_segment_index); //push back vector into intersection_street_segment
    }
    
    for(int segment = 0; segment < getNumStreetSegments(); segment++){
        struct StreetSegmentInfo street_info = getStreetSegmentInfo(segment);
        double speed_limit = street_info.speedLimit;
        double distance = findStreetSegmentLength(segment);
        std::vector<double> segment_travel_time;
        segment_travel_time.push_back(distance/speed_limit);
        street_segment_travel_times.push_back(segment_travel_time);
        
    }
    

    //unordered/ other data structures












    load_successful = true; //Make sure this is updated to reflect whether
    //loading the map succeeded or failed

    return load_successful;
}
//LatLon         getIntersectionPosition(IntersectionIdx intersectionIdx)
// Returns the distance between two (lattitude,longitude) coordinates in meters
// Speed Requirement --> moderate


double findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> points) {
    double x1, y1, x2, y2, lat1, lat2, lat_avg, distance;

    lat1 = points.second.latitude() * kDegreeToRadian;
    lat2 = points.first.latitude() * kDegreeToRadian;
    lat_avg = (lat1 + lat2) / 2.0;

    x1 = points.first.longitude() * cos(lat_avg) * kDegreeToRadian;
    y1 = points.first.latitude() * kDegreeToRadian;
    x2 = points.second.longitude() * cos(lat_avg) * kDegreeToRadian;
    y2 = points.second.latitude() * kDegreeToRadian;
    
    distance = kEarthRadiusInMeters *  sqrt(pow(y2-y1,2)+ pow(x2-x1,2));
    return distance;
}



// Returns the length of the given street segment in meters
// Speed Requirement --> moderate

double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    //StreetSegmentInfo getStreetSegmentInfo(StreetSegmentIdx streetSegmentIdx);
    //LatLon         getIntersectionPosition(IntersectionIdx intersectionIdx);
    //LatLon getStreetSegmentCurvePoint(StreetSegmentIdx streetSegmentIdx, int pointNum);
    LatLon point1,point2;
    std::pair<LatLon,LatLon> points (point1,point2);
    double length = 0.0;
    struct StreetSegmentInfo street_info = getStreetSegmentInfo(street_segment_id);
    int numCurvePoints = street_info.numCurvePoints;
    
    if(numCurvePoints == 0){
        point1 = getIntersectionPosition(street_info.from);
        point2 = getIntersectionPosition(street_info.to);
        length += findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        return length;
    }
    else{
        point1 = getIntersectionPosition(street_info.from);
        point2 = getStreetSegmentCurvePoint(street_segment_id,0);
        points = std::pair<LatLon,LatLon> (point1,point2);
        length += findDistanceBetweenTwoPoints(points);
        
        for(int i = 0; i < numCurvePoints-1; i++){
            point1 = getStreetSegmentCurvePoint(street_segment_id,i);
            point2 = getStreetSegmentCurvePoint(street_segment_id,i+1);
            length += findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        }
        point1 = getStreetSegmentCurvePoint(street_segment_id,numCurvePoints-1);
        point2 = getIntersectionPosition(street_info.to);
        length += findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        return length;
    }

}

double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id) {
    // Note: (time = distance/speed_limit) double / float
    return street_segment_travel_times[street_segment_id][0];

}






















































































































//end Alan , start Alex

std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){
    std::vector<StreetIdx> streetIdx;//create vector for final return
    if (street_prefix.size()==0) return streetIdx;//check for length 0 input
    std::multimap<std::string, StreetIdx> streets;//multimap of all the street names and corresponding idx
    street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), ::isspace), street_prefix.end());;
    
    //fill multimap
    for (int i = 0; i < getNumStreets();i++){
        std::string streetName = getStreetName(i);
        streetName.erase(std::remove_if(streetName.begin(), streetName.end(), isspace), streetName.end());//remove all spaces
        std::transform(streetName.begin(), streetName.end(), streetName.begin(), ::tolower);//transform into lower case only
        streets.insert(std::make_pair(streetName,i));
    }
    
    auto firstOccurance = streets.lower_bound(street_prefix);//locate first match, save as iterator
    if(firstOccurance==streets.end()) return streetIdx;//if no match, return empty vector immediately
    
    //increment iterator until input is not a prefix of Key anymore 
    for (auto it = firstOccurance; it->first.compare(0, street_prefix.size(), street_prefix)==0; ++it){
        streetIdx.push_back(it->second);
    }    
    
    return streetIdx;
}

std::vector<std::string> findStreetNamesOfIntersection(IntersectionIdx intersection_id) {
    std::vector<std::string> intersectionNames;

    int numIntersections = getNumIntersectionStreetSegment(intersection_id);

    //loop through each segment at intersection, find name, add to vector
    for (int i = 0; i < numIntersections; i++) {
        StreetSegmentIdx segment = getIntersectionStreetSegment(intersection_id, i);

        std::string name = getStreetName(getStreetSegmentInfo(segment).streetID);

        intersectionNames.push_back(name);
    }

    return intersectionNames;
}



























































































































































//end Alex, start Tawseef

/* functions to do: 
 * findStreetSegmentsOfIntersection, --done
 * findAdjacentIntersections,        --done 
 * findIntersectionsOfStreet
 * findIntersectionsOfTwoStreets,
 * findStreetboundingBox
 */

/*returns the street segments that connect to given intersection
 * finds number of segments connected to an intersections, loops through them
 */
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id) {

    //get number of street segments connected to intersection
    int numIntersections = getNumIntersectionStreetSegment(intersection_id);

    std::vector<StreetSegmentIdx> streetSegments;

    //loop through each segment connected to an intersection
    for (int i = 0; i < numIntersections; i++) {
        //get id of each segment
        StreetSegmentIdx segment = getIntersectionStreetSegment(intersection_id, i);
        streetSegments.push_back(segment);
    }

    return streetSegments;
}

/*returns all intersections reachable by traveling down one street segment
 * from given intersection
 */
std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id) {

    std::vector<IntersectionIdx> intersections;


    //get number of segments connected at intersection
    int numIntersections = getNumIntersectionStreetSegment(intersection_id);

    for (int i = 0; i < numIntersections; i++) {
        //get id of each segment
        StreetSegmentIdx segment = getIntersectionStreetSegment(intersection_id, i);
        //get segment info
        StreetSegmentInfo info = getStreetSegmentInfo(segment);
        
       
        //if oneway, only pass in intersection at position segment.to
        //else insert both intersections at from and to direction
        if (info.oneWay) {
            intersections.push_back(info.to);
            
        } else if (!info.oneWay ){
            intersections.push_back(info.to);
            intersections.push_back(info.from);

        }
        
       
    }
    //sort array in ascending order, delete duplicates
    std::sort(intersections.begin(), intersections.end());
    auto last = std::unique(intersections.begin(), intersections.end());
    intersections.erase(last, intersections.end());
    //delete intersection_id
    intersections.erase(std::remove(intersections.begin(), intersections.end(), intersection_id), intersections.end());
    
    
    return intersections;
}

//Returns all intersections along given street

















































































































































//end Tawseef

void closeMap() {
    //unloads map / frees memory used by API
    closeStreetDatabase();

}
