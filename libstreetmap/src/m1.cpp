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
#include <string>
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <unordered_set>

std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;
std::vector<double> street_segment_travel_times; 
std::vector<std::vector<double>> street_lengths;
std::vector<std::vector<StreetSegmentIdx>> streets_streetSegments;
//holds all street ids of each street segment with street segments as indices
std::vector<StreetIdx> streets;

//holds [streetId][intersections], first index is streetID of street, second index
//is the array of intersections of streetID
std::vector<std::unordered_set<StreetSegmentIdx>> intersections_of_each_street;


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
    
    //holds all distances of each street segment with street segments as indices
    std::vector<double> segment_distances;
    //map of each street id and distance of each street segment
    std::multimap<int,double> map_streetIds_distances;
     
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
        int street_id = street_info.streetID;
        
        //pushes back street id, distance, and travel time in each respective vector
        streets.push_back(street_id);
        segment_distances.push_back(distance);
        street_segment_travel_times.push_back(distance/speed_limit);
               
    }
    
    for(int i = 0;i < getNumStreetSegments();i++){
        //inserts a pair of street id and street segment distances for each street segment
        map_streetIds_distances.insert(std::pair<int,double>(streets[i],segment_distances[i]));
    } 
    
    
    
    street_lengths.resize(getNumStreets());
    for(int street_id = 0; street_id < getNumStreets(); street_id++){

        //finds all occurrences of street_id and returns in a pair
        auto range = map_streetIds_distances.equal_range(street_id);
        auto it = range.first;
       //iterate until second iterator
        while(it != range.second){
            street_lengths[street_id].push_back((*it).second);
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
    streets_streetSegments.resize(getNumStreets());
    for(int streetSegmentID = 0; streetSegmentID < getNumStreetSegments();streetSegmentID++){
        streets_streetSegments[streets[streetSegmentID]].push_back(streetSegmentID);
    }
    
    intersections_of_each_street.resize(getNumStreets());
    for(int street_id = 0; street_id < getNumStreets();street_id++){
        for(int street_segment = 0; street_segment < streets_streetSegments[street_id].size();street_segment++){
            StreetSegmentInfo info = getStreetSegmentInfo(streets_streetSegments[street_id][street_segment]);
            intersections_of_each_street[street_id].insert(info.to);
            intersections_of_each_street[street_id].insert(info.from);
        }
    }
   /*
    for (int street_id = 0; street_id < getNumStreets(); street_id++) {
        
        intersections_of_each_street.resize(getNumStreets());
        //get intersections of each street
        for (int i = 0; i < streets_streetSegments[street_id].size(); i++) {
            //get info
            StreetSegmentInfo info = getStreetSegmentInfo(streets_streetSegments[street_id][i]);     
            //if street id matches, insert to/from intersections into vector
            if (info.streetID == street_id) {
                intersections_of_each_street[street_id].push_back(info.to);
                intersections_of_each_street[street_id].push_back(info.from);
            }
        }

        //sort array in ascending order, delete duplicates
        std::sort(intersections_of_each_street[street_id].begin(), intersections_of_each_street[street_id].end());
        auto last = std::unique(intersections_of_each_street[street_id].begin(), intersections_of_each_street[street_id].end());
        intersections_of_each_street[street_id].erase(last, intersections_of_each_street[street_id].end());
        
        
    }
    */
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
    LatLon point1,point2;
    std::pair<LatLon,LatLon> points (point1,point2);
    double length = 0.0;
    struct StreetSegmentInfo street_info = getStreetSegmentInfo(street_segment_id);
    int numCurvePoints = street_info.numCurvePoints;
    
    //if the street segment is straight
    if(numCurvePoints == 0){
        point1 = getIntersectionPosition(street_info.from);
        point2 = getIntersectionPosition(street_info.to);
        length += findDistanceBetweenTwoPoints(std::make_pair(point1,point2));
        return length;
    }
    //if the street segment has curve points
    else{
        point1 = getIntersectionPosition(street_info.from);
        point2 = getStreetSegmentCurvePoint(street_segment_id,0);
        points = std::pair<LatLon,LatLon> (point1,point2);
        
        //sum the lengths between each point
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
    // returns travel time from vector
    return street_segment_travel_times[street_segment_id];
}

double findStreetLength(StreetIdx street_id){
    double length = 0.0;
    //traverses through the vector of street_lengths[street_id] and adds them
    for(int i = 0; i < street_lengths[street_id].size();i++){
        length += street_lengths[street_id][i];
    }
    return length;
}

double findFeatureArea(FeatureIdx feature_id){
    double x1, y1, x2, y2, lat1, lat2, lat_avg;
    double area = 0.0;
    int feature_points = getNumFeaturePoints(feature_id);
    
    //Latitude and longitude of the first and last point
    LatLon point1 = getFeaturePoint(feature_id, 0);
    LatLon point2 = getFeaturePoint(feature_id, feature_points-1);
    
    //if the polygon is closed, compute the area of the closed polygon
    if(point1 == point2){
        for(int i = 0; i < feature_points-1; i++){
            
            point1 = getFeaturePoint(feature_id, i);
            point2 = getFeaturePoint(feature_id, i+1);
            
            lat1 = point1.latitude() * kDegreeToRadian;
            lat2 = point2.latitude() * kDegreeToRadian;
            lat_avg = (lat1 + lat2) / 2.0;
    
            x1 = point1.longitude() * cos(lat_avg) * kDegreeToRadian * kEarthRadiusInMeters;
            y1 = point1.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
            x2 = point2.longitude() * cos(lat_avg) * kDegreeToRadian * kEarthRadiusInMeters;
            y2 = point2.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
            
            area += ((x2+x1)/2.0) * (y2-y1);
        }  
    }
    
    if(area < 0) return area * -1.0;
    else return area;
}




//end Alan , start Alex

std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){
    std::vector<StreetIdx> streetIdx;//create vector for final return
    if (street_prefix.size()==0) return streetIdx;//check for length 0 input
    
    std::multimap<std::string, StreetIdx> streets_NamesIdx;//multimap of all the street names and corresponding idx
    street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), ::isspace), street_prefix.end());//formatting: lowercase and remove spaces
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);
    
    //fill multimap
    for (StreetIdx i = 0; i < getNumStreets();i++){
        std::string streetName = getStreetName(i);
        streetName.erase(std::remove_if(streetName.begin(), streetName.end(), isspace), streetName.end());//remove all spaces
        std::transform(streetName.begin(), streetName.end(), streetName.begin(), ::tolower);//transform into lower case only
        streets_NamesIdx.insert(std::make_pair(streetName,i));
    }
    
    auto firstOccurance = streets_NamesIdx.lower_bound(street_prefix);//locate first match, save as iterator
    if(firstOccurance==streets_NamesIdx.end()) return streetIdx;//if no match, return empty vector immediately
    
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



    
                                                     
POIIdx findClosestPOI(LatLon my_position, std::string POIname){
    POIIdx target;//define variables needed
    double distance;
    std::string name;
    std::multimap<double, POIIdx> matchingPOI;//define multimap for convenience (already sorted)
    for (POIIdx POI = 0; POI < getNumPointsOfInterest();POI++){
        name = getPOIName(POI);
        //check if name matches
        if(name.compare(POIname)==0){
            distance = findDistanceBetweenTwoPoints(std::make_pair(my_position,getPOIPosition(POI)));
            matchingPOI.insert(std::make_pair(distance,POI));
        }
    }
    target = matchingPOI.begin()->second;
    return target;
}


IntersectionIdx findClosestIntersection(LatLon my_position){
    std::multimap<double,IntersectionIdx> intersections;//define multimap for convenience
    IntersectionIdx target;
    double distance;
    //insert into multimap
    for (IntersectionIdx intersection = 0; intersection < getNumIntersections();intersection++){
        LatLon query = getIntersectionPosition(intersection);
        distance = findDistanceBetweenTwoPoints(std::make_pair(query,my_position));
        intersections.insert(std::make_pair(distance,intersection));
    }
    
    target = intersections.begin()->second;//return smallest value
    return target;
}

//return smallest rectangle that contains all the intersections and
//curve points of the given street
LatLonBounds findStreetBoundingBox(StreetIdx street_id){
    
    LatLonBounds box;
    std::set<float> pointsOfStreet_Lon;
    std::set<float> pointsOfStreet_Lat;
    std::vector<IntersectionIdx> intersections = intersections_of_each_street[street_id];
    std::vector<StreetSegmentIdx> segmentsOfThisStreet = streets_streetSegments[street_id];
    //LatLon bottom_left = segmentsOfThisStreet[0];
   // LatLon top_right = segmentsOfThisStreet[0];
    
    for (int i = 0; i < intersections.size(); i++) {
        LatLon position = getIntersectionPosition(intersections[i]);
 
        pointsOfStreet_Lon.insert(position.longitude());
        pointsOfStreet_Lat.insert(position.latitude());
    }
     
    
    for(StreetSegmentIdx i = 0; i < segmentsOfThisStreet.size();i++){
        struct StreetSegmentInfo street_info = getStreetSegmentInfo(segmentsOfThisStreet[i]);
        int numCurvePoints = street_info.numCurvePoints;
        for (int j = 0; j < numCurvePoints; j++){
            LatLon curvePoint = getStreetSegmentCurvePoint(i,j);
            pointsOfStreet_Lon.insert(curvePoint.longitude());
            pointsOfStreet_Lat.insert(curvePoint.latitude());         
        }
        
    }
    
    LatLon min = LatLon(*(pointsOfStreet_Lat.begin()), *(pointsOfStreet_Lon.begin()));
    LatLon max = LatLon(*(pointsOfStreet_Lat.rbegin()), *(pointsOfStreet_Lon.rbegin()));
    box.min = min;
    box.max = max;
   
    return box;
}




















































































//end Alex, start Tawseef

/* functions to do: 
 * findStreetSegmentsOfIntersection, --done
 * findAdjacentIntersections,        --done 
 * findIntersectionsOfStreet         --wip
 * findIntersectionsOfTwoStreets,    --wip
 */

/*returns the street segments that connect to given intersection
 * finds number of segments connected to an intersections, loops through them
 */
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id) {

    std::vector<StreetSegmentIdx> streetSegments;

    //loop through each segment connected to an intersection
    for (int i = 0; i < intersection_street_segments[intersection_id].size(); i++) {
        //get id of each segment
        
        streetSegments.push_back(intersection_street_segments[intersection_id][i]);
    }

    return streetSegments;
}

/*returns all intersections reachable by traveling down one street segment
 * from given intersection
 */
std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id) {

    std::vector<IntersectionIdx> intersections;
   
    for (int i = 0; i < intersection_street_segments[intersection_id].size(); i++) {
      
        //get segment info
        StreetSegmentInfo info = getStreetSegmentInfo(intersection_street_segments[intersection_id][i]);
             
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
//currently nonfunctional
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    
    std::vector<IntersectionIdx> intersections = intersections_of_each_street[street_id];
    
//    for (int i = 0; i < streets_streetSegments[street_id].size(); i++) {
//        //get info
//        StreetSegmentInfo info = getStreetSegmentInfo(streets_streetSegments[street_id][i]);     
//        //if street id matches, insert to/from intersections into vector
//        if (info.streetID == street_id) {
//            intersections.push_back(info.to);
//            intersections.push_back(info.from);
//        }
//    }
//    
//    //sort array in ascending order, delete duplicates
//    std::sort(intersections.begin(), intersections.end());
//    auto last = std::unique(intersections.begin(), intersections.end());
//    intersections.erase(last, intersections.end());
    return intersections;
}

//returns intersections where two streets intersect
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(std::pair<StreetIdx, StreetIdx> street_ids){
   
    
    std::vector<IntersectionIdx> result;
    
    std::vector<IntersectionIdx> street1 = intersections_of_each_street[street_ids.first];
    std::vector<IntersectionIdx> street2 = intersections_of_each_street[street_ids.second];
    
    for (int i = 0; i < intersections_of_each_street[street_ids.first].size(); i++) {
        for (int j = 0; j < intersections_of_each_street[street_ids.second].size(); j++) {
            if (street1[i] == street2[j]) {
                result.push_back(street1[i]);
            }
        }
    }
    
    
    //compare the intersections of each street, if they match, insert
//    for (int i = 0; i < streets_streetSegments[street_ids.first].size(); i++) {
//        
//        StreetSegmentInfo street1 = getStreetSegmentInfo(streets_streetSegments[street_ids.first][i]);
//        
//        for (int j = 0; j < streets_streetSegments[street_ids.second].size(); j++) {
//            
//            StreetSegmentInfo street2 = getStreetSegmentInfo(streets_streetSegments[street_ids.second][j]);
//            
//            if (street1.from == street2.from || street1.from == street2.to) {
//                result.push_back(street1.from);
//            } else if (street1.to == street2.to || street1.to == street2.from) {
//                result.push_back(street1.to);
//            } 
//        }
//    }
//    
    //sort array in ascending order, delete duplicates
//    std::sort(result.begin(), result.end());
//    auto last = std::unique(result.begin(), result.end());
//    result.erase(last, result.end());
    
    return result;
}

    
    
























//end Tawseef

void closeMap() {
    //unloads map / frees memory used by API
    street_lengths.clear();
    intersection_street_segments.clear();
    street_segment_travel_times.clear();
    intersections_of_each_street.clear();
    closeStreetDatabase();

}
