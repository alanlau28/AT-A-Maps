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
#include <set>
#include <algorithm>
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <unordered_map>
#include "load_database.h"

struct SegmentData{
    double travel_time;
    StreetIdx streetID;
};

struct StreetData{
    //holds [streetId][intersections], first index is streetID of street, second index
    //is the array of intersections of streetID
    std::vector<StreetSegmentIdx> intersections;
    
    //hold all street segment ids of each street
    //street ids are indices
    std::vector<StreetSegmentIdx> street_segments;
    
    std::vector<double> street_lengths;
};


std::vector<StreetData> street_data;

//holds all street ids and travel times of each street segment
//street segments ids are indices
std::vector<SegmentData> street_segment_data;

//holds all street segment ids of each intersection
//intersection ids are indices
std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;

//holds distances of all street segments on each street
//street ids are indices
//std::vector<std::vector<double>> street_lengths;

//holds street names and corresponding street ids as pair
//<StreetName, StreetIdx>
std::multimap<std::string, StreetIdx> streets_NamesIdx;


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
    //Indicates whether the map has loaded successfully
    bool load_successful = false; 
    
    map_load_path = map_streets_database_filename;
    std::cout << "loadMap: " << map_streets_database_filename << std::endl;

    load_successful = loadStreetsDatabaseBIN(map_streets_database_filename);
    //
    // Load your map related data structures here.
    //
    if(!load_successful){
        return false;
    }
    
    
    street_data.resize(getNumStreets());
    
    //holds all distances of each street segment with street segments as indices
    //<double distance>
    std::vector<double> segment_distances;
    
    //map of each street id and distance of each street segment
    //<int streetIdx, double distance>
    std::unordered_multimap<StreetIdx,double> map_streetIds_distances;
     
    //traverse through all intersections
    for (int intersection = 0; intersection < getNumIntersections(); intersection++) {
        std::vector<StreetSegmentIdx> street_segment_index; //create empty vector

        //traverse through street segment of each intersection
        for (int street_segment_num = 0; street_segment_num < getNumIntersectionStreetSegment(intersection); street_segment_num++) {
            street_segment_index.push_back(getIntersectionStreetSegment(intersection, street_segment_num)); //push back street segment id into vector
        }

        intersection_street_segments.push_back(street_segment_index); //push back vector into intersection_street_segment
    }
    
    //traverse all street segments info
    for(int segment = 0; segment < getNumStreetSegments(); segment++){
        //street info
        struct StreetSegmentInfo street_info = getStreetSegmentInfo(segment);
        struct SegmentData street_segment_info;
        double speed_limit = street_info.speedLimit;
        double distance = findStreetSegmentLength(segment);
        int street_id = street_info.streetID;
        
        street_segment_info.travel_time = distance/speed_limit;
        street_segment_info.streetID = street_id;
        
        //pushes back street id, distance, and travel time in each respective vector
        segment_distances.push_back(distance);
        street_segment_data.push_back(street_segment_info);
    }
    
    for(int i = 0;i < getNumStreetSegments();i++){
        //inserts a pair of street id and street segment distances for each street segment
        map_streetIds_distances.insert(std::pair<int,double>(street_segment_data[i].streetID,segment_distances[i]));
    }     
    
    
    
    for(int street_id = 0; street_id < getNumStreets(); street_id++){        
        //finds all occurrences of street_id and returns in a pair
        auto range = map_streetIds_distances.equal_range(street_id);
        auto it = range.first;
       
        //iterate until second iterator
        while(it != range.second){
            street_data[street_id].street_lengths.push_back((*it).second);
            it++;
        }    
      
    }
    
    
    /* streets: every index is the street segment id, every element holds the street id
     * streets[streetSegmentID] -> the street id
     *  resize initializes streets_streetSegments so that you can access specific elements
     * each index of streets_streetSegments is the streetID
     * Using streets[streetSegmentID], you can access the street of the respective street segment
     * push back on that index because the element holds a vector of streetsegmentID
     */
    for(int streetSegmentID = 0; streetSegmentID < getNumStreetSegments();streetSegmentID++){
        street_data[street_segment_data[streetSegmentID].streetID].street_segments.push_back(streetSegmentID);
    }    
    
   
    for (int street_id = 0; street_id < getNumStreets(); street_id++) {        
  
        //get intersections of each street
        for (int i = 0; i < street_data[street_id].street_segments.size(); i++) {
            //get info
            StreetSegmentInfo street_seg_info = getStreetSegmentInfo(street_data[street_id].street_segments[i]);     
            //if street id matches, insert to/from intersections into vector
            if (street_seg_info.streetID == street_id) {
                street_data[street_id].intersections.push_back(street_seg_info.to);
                street_data[street_id].intersections.push_back(street_seg_info.from);
            }
        }

        //sort array in ascending order, delete duplicates
        auto begin = street_data[street_id].intersections.begin();
        auto end = street_data[street_id].intersections.end();
        std::sort(begin, end);
        auto last = std::unique(begin,end);
        street_data[street_id].intersections.erase(last, end);       
    }
    
    //fill multimap of <string streetName, streetIdx>
    for (StreetIdx street_id = 0; street_id < getNumStreets();street_id++){
        std::string streetName = getStreetName(street_id);
        
        //formatting: lowercase and remove spaces
        streetName.erase(std::remove_if(streetName.begin(), streetName.end(), ::isspace), streetName.end());
        std::transform(streetName.begin(), streetName.end(), streetName.begin(), ::tolower);
        
        streets_NamesIdx.insert(std::make_pair(streetName,street_id));        
    }

   
    //map loaded successfully
    return true;
}



// Returns the distance between two (latitude,longitude) coordinates in meters
double findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> points) {
    
    //converts latitudes of points into radians and finds the average
    double lat1 = points.second.latitude() * kDegreeToRadian;
    double lat2 = points.first.latitude() * kDegreeToRadian;
    double lat_avg = (lat1 + lat2) / 2.0;
    
    //finds the x and y coordinates of each point
    double x1 = points.first.longitude() * cos(lat_avg) * kDegreeToRadian;
    double y1 = points.first.latitude() * kDegreeToRadian;
    double x2 = points.second.longitude() * cos(lat_avg) * kDegreeToRadian;
    double y2 = points.second.latitude() * kDegreeToRadian;
    
    //finds distance between the two points and returns
    return kEarthRadiusInMeters *  sqrt(pow(y2-y1,2)+ pow(x2-x1,2));
}



// Returns the length of the given street segment in meters
double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    struct StreetSegmentInfo street_seg_info = getStreetSegmentInfo(street_segment_id);
    int numCurvePoints = street_seg_info.numCurvePoints;
    
    //if the street segment is straight
    if(numCurvePoints == 0){
        //sum length between to and from
        LatLon pos_from = getIntersectionPosition(street_seg_info.from);
        LatLon pos_to = getIntersectionPosition(street_seg_info.to);
        
        //return length between the two points
        return findDistanceBetweenTwoPoints(std::make_pair(pos_from,pos_to));
        
    }
    //if the street segment has curve points
    else{
        //first length is between from and first curve point
        LatLon point1 = getIntersectionPosition(street_seg_info.from);
        LatLon point2 = getStreetSegmentCurvePoint(street_segment_id,0);
        double length = findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        
        //sum the lengths between each curve point
        for(int i = 0; i < numCurvePoints-1; i++){
            point1 = getStreetSegmentCurvePoint(street_segment_id,i);
            point2 = getStreetSegmentCurvePoint(street_segment_id,i+1);
            length += findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        }
        
        //sum the final length between last curve point and to
        point1 = getStreetSegmentCurvePoint(street_segment_id,numCurvePoints-1);
        point2 = getIntersectionPosition(street_seg_info.to);       
        length += findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        
        return length;
    }

}

// Returns the travel time to drive from one end of a street segment 
// to the other, in seconds, when driving at the speed limit
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id) {    
    return street_segment_data[street_segment_id].travel_time;
}

// Returns the length of the given street segment in meters
double findStreetLength(StreetIdx street_id){
    double length = 0.0;
    
    //traverses through the street segment lengths of a street and adds them
    for(int street_seg_num = 0; street_seg_num < street_data[street_id].street_lengths.size(); street_seg_num++){
        length += street_data[street_id].street_lengths[street_seg_num];
    }
    
    return length;
}

// Returns the area of the given closed feature in square meters
// Assume a non self-intersecting polygon (i.e. no holes)
// Return 0 if this feature is not a closed polygon.
double findFeatureArea(FeatureIdx feature_id){
    double area = 0.0;
    int feature_points = getNumFeaturePoints(feature_id);
    
    //Latitude and longitude of the first and last point of feature
    LatLon point1 = getFeaturePoint(feature_id, 0);
    LatLon point2 = getFeaturePoint(feature_id, feature_points-1);
    
    //if the polygon is closed, compute the area of the closed polygon
    if(point1 == point2){
        //traverse through each feature point
        for(int i = 0; i < feature_points-1; i++){
            
            point1 = getFeaturePoint(feature_id, i);
            point2 = getFeaturePoint(feature_id, i+1);
            
            //convert latitudes to radians and find the average
            double lat1 = point1.latitude() * kDegreeToRadian;
            double lat2 = point2.latitude() * kDegreeToRadian;
            double lat_avg = (lat1 + lat2) / 2.0;
            
            //find the x,y coordinates of point 1 and point 2
            double x1 = point1.longitude() * cos(lat_avg) * kDegreeToRadian * kEarthRadiusInMeters;
            double y1 = point1.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
            double x2 = point2.longitude() * cos(lat_avg) * kDegreeToRadian * kEarthRadiusInMeters;
            double y2 = point2.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
            
            //calculate the area between the two points and add to the total area sum
            area += ((x2+x1)/2.0) * (y2-y1);
        }  
    }
    //if feature points are traversed counterclockwise, area is negative 
    if(area < 0) {
        return area * -1.0;
    }
    //if feature points are traversed clockwise, area is positive
    return area;
   
}




// Returns all street ids corresponding to street names that start with the given prefix 
//  For example, both "bloor " and "BloOrst" are prefixes to 
// "Bloor Street East".
std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){
    std::vector<StreetIdx> streetIdx;//create vector for final return
    if (street_prefix.size()==0) return streetIdx;//check for length 0 input
    
    //formatting: lowercase and remove spaces
    street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), ::isspace), street_prefix.end());
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);
    
    //locate first match(if there is any), save as iterator
    auto firstOccurance = streets_NamesIdx.lower_bound(street_prefix);
    //if no match, return empty vector immediately
    if(firstOccurance==streets_NamesIdx.end()) return streetIdx;
    
    //increment iterator until input is not a prefix of Key anymore 
    for (auto it = firstOccurance; (it!=streets_NamesIdx.end()&& (it->first.compare(0, street_prefix.size(), street_prefix)==0)); ++it){      
        streetIdx.push_back(it->second);//add streetIdx to vector for return
    }
    
    return streetIdx;
}

// Returns the street names at the given intersection (includes duplicate street names in the returned vector)
std::vector<std::string> findStreetNamesOfIntersection(IntersectionIdx intersection_id) {
    std::vector<std::string> intersectionNames;
    
    //find number of street segments connected to an intersection
    int numIntersections = getNumIntersectionStreetSegment(intersection_id);

    //loop through each segment at intersection, find street name, add to vector
    for (int street_seg_num = 0; street_seg_num < numIntersections; street_seg_num++) {
        StreetSegmentIdx segment = getIntersectionStreetSegment(intersection_id, street_seg_num);
        std::string name = getStreetName(getStreetSegmentInfo(segment).streetID);
        intersectionNames.push_back(name);
    }

    return intersectionNames;
}

    
// Returns the nearest point of interest of the given name to the given position                                            
POIIdx findClosestPOI(LatLon my_position, std::string POIname){  
   
    std::multimap<double, POIIdx> matchingPOI;//define multimap for convenience (already sorted)
    
    for (POIIdx POI = 0; POI < getNumPointsOfInterest();POI++){
        std::string name = getPOIName(POI);
        //check if name matches
        if(name.compare(POIname)==0){
            double distance = findDistanceBetweenTwoPoints(std::make_pair(my_position,getPOIPosition(POI)));
            matchingPOI.insert(std::make_pair(distance,POI));
        }
    }
    //locate beginning of multimap, aka smallest distance
    return matchingPOI.begin()->second;
}

// Returns the nearest intersection to the given position
IntersectionIdx findClosestIntersection(LatLon my_position){
    std::multimap<double,IntersectionIdx> intersections;//define multimap for convenience    
    
    //insert into multimap
    for (IntersectionIdx intersection = 0; intersection < getNumIntersections();intersection++){
        //find distance between my_position and intersections
        LatLon query = getIntersectionPosition(intersection);
        double distance = findDistanceBetweenTwoPoints(std::make_pair(query,my_position));
        intersections.insert(std::make_pair(distance,intersection));
    }
    
    //return intersection id of closest intersection
    return intersections.begin()->second;
}

//return smallest rectangle that contains all the intersections and curve points of the given street
LatLonBounds findStreetBoundingBox(StreetIdx street_id){
    
    LatLonBounds box;
    std::set<float> pointsOfStreet_Lon;//set of longitude positions
    std::set<float> pointsOfStreet_Lat;//set of latitude positions
    
    //vector of all intersections of a street
    std::vector<IntersectionIdx> intersections = street_data[street_id].intersections;
    
    //vector of all street segments of a street
    std::vector<StreetSegmentIdx> segmentsOfThisStreet = street_data[street_id].street_segments;
  
    //insert all intersection positions into sets (lat and lon)
    for (int i = 0; i < intersections.size(); i++) {
        LatLon position = getIntersectionPosition(intersections[i]);
 
        pointsOfStreet_Lon.insert(position.longitude());
        pointsOfStreet_Lat.insert(position.latitude());
    }
    
    //insert all curve points into sets (lat and lon)
    for(int segmentNum = 0; segmentNum < segmentsOfThisStreet.size();segmentNum++){
        //number of curve points in a segment
        struct StreetSegmentInfo street_info = getStreetSegmentInfo(segmentsOfThisStreet[segmentNum]);
        int numCurvePoints = street_info.numCurvePoints;
        
            //positions of curve points
            for (int curvePoint_num = 0; curvePoint_num < numCurvePoints; curvePoint_num++){
                LatLon curvePoint = getStreetSegmentCurvePoint(segmentsOfThisStreet[segmentNum],curvePoint_num);
                pointsOfStreet_Lon.insert(curvePoint.longitude());
                pointsOfStreet_Lat.insert(curvePoint.latitude());         
            }
        
    }
    
    //set is already sorted, create bounding box with smallest & largest LatLon values
    box.min = LatLon(*(pointsOfStreet_Lat.begin()), *(pointsOfStreet_Lon.begin()));
    box.max = LatLon(*(pointsOfStreet_Lat.rbegin()), *(pointsOfStreet_Lon.rbegin()));
   
    return box;
}





/*returns the street segments that connect to given intersection
 * finds number of segments connected to an intersections, loops through them
 */
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id) {

    return intersection_street_segments[intersection_id];
}

/*returns all intersections reachable by traveling down one street segment
 * from given intersection
 */
std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id) {

    std::vector<IntersectionIdx> intersections;
   
    for (int street_seg_num = 0; street_seg_num < intersection_street_segments[intersection_id].size(); street_seg_num++) {
      
        //get segment info
        StreetSegmentInfo info = getStreetSegmentInfo(intersection_street_segments[intersection_id][street_seg_num]);
             
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
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    
    return street_data[street_id].intersections;
}

//returns intersections where two streets intersect
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(std::pair<StreetIdx, StreetIdx> street_ids){
   
    
    std::vector<IntersectionIdx> result;
    
    //make two vectors with the intersections of each street
    std::vector<IntersectionIdx> street1 = street_data[street_ids.first].intersections;
    std::vector<IntersectionIdx> street2 = street_data[street_ids.second].intersections;
    
    //corner case when streetIds are same
    //return all the intersections along street
    if (street_ids.first == street_ids.second) {        
        return street1;        
    }
    
    //else check which intersections along each street match, insert into vector
    for (int first_intersections = 0; first_intersections < street1.size(); first_intersections++) {
        for (int second_intersections = 0; second_intersections < street2.size(); second_intersections++) {
            //if intersections match
            if (street1[first_intersections] == street2[second_intersections]) {
                result.push_back(street1[first_intersections]);
            }
        }
    }   
    
    return result;
}




void closeMap() {
    //unloads map / frees memory used by API
    intersection_street_segments.clear();

    street_data.clear();
    
    
    streets_NamesIdx.clear();
    closeStreetDatabase();
    street_segment_data.clear();
}
