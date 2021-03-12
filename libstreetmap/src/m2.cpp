#include "StreetsDatabaseAPI.h"
#include "m2.h"
#include "m1.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "OSMDatabaseAPI.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include "load_database.h"

#include <gtk/gtk.h>


#include <point.hpp>
#include <unordered_set>


struct boundingbox{
    double max_x;
    double max_y;
    double min_x;
    double min_y;
    double lat_avg;
    double area;
};

struct street_segment_data{
    std::vector<ezgl::point2d> coordinates;
    std::string name;
    double angle;
    float speed_limit;
    StreetIdx street_id;
    std::string segment_type;
    bool one_way;
    bool highlight;
};


struct feature_data{
    std::vector<ezgl::point2d> coordinates;
    std::string name;
    std::string feature_type;
    int numFeaturePoints;
    double area;
};

struct POI_data{
    ezgl::point2d coordinate {.0, .0};
    std::string name;
    std::string type;
};

struct intersection_data{
    ezgl::point2d coordinate {0,0};
    LatLon position;
    bool highlight;
    std::string name;
};

struct entry_completion{
    GtkEntryCompletion* completion;
    GtkTreeModel* completion_model;
};

std::vector<street_segment_data> street_segments;
std::vector<feature_data> features;
std::vector<POI_data> pointOfInterests;
std::vector<std::vector<StreetSegmentIdx>> street_segments_of_street;


std::vector<std::string> feature_priority;

std::vector<POI_data>  fuel;
std::vector<POI_data>  school;
std::vector<POI_data>  restaurant;
std::vector<POI_data>  parking;
std::vector<POI_data>  theatre;
std::vector<POI_data>  bank;
std::vector<POI_data>  bar;
std::vector<POI_data>  cafe;
std::vector<POI_data>  fastfood;
std::vector<POI_data>  pharmacy; 
std::vector<POI_data>  hospital;
std::vector<POI_data>  post_office;
std::vector<POI_data>  gym;
std::vector<POI_data>  art;
std::vector<POI_data>  library;

std::string default_font;
//std::string arabic = "Noto Sans Arabic";



std::vector<intersection_data> intersections;

std::unordered_map <OSMID,std::string> street_types;

std::unordered_map<std::string,std::string> map_paths;

struct boundingbox bounds;
struct entry_completion entryCompletion;

ezgl::application* global_app;

void close_map();

bool operator< (feature_data& first, feature_data& second){
    if(std::find(feature_priority.begin(),feature_priority.end(),first.feature_type) - feature_priority.begin() < std::find(feature_priority.begin(),feature_priority.end(),second.feature_type) - feature_priority.begin()) return true;
    else return false;
}
//(x, y) = (R·lon·cos(latavg), R·lat)
LatLon latLonFromWorld(double x, double y){
    float lon = x / kEarthRadiusInMeters / cos(bounds.lat_avg) / kDegreeToRadian;
    float lat = y / kEarthRadiusInMeters / kDegreeToRadian;
    LatLon position(lat,lon);
    return position;
}
ezgl::point2d convertCoordinates(double longitude, double latitude, double lat_avg){
    
    double x = kEarthRadiusInMeters * longitude * cos(lat_avg) * kDegreeToRadian;
    double y = kEarthRadiusInMeters * latitude * kDegreeToRadian;
    ezgl::point2d point(x,y);
    
    return point;
}

void load_bin(){
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/beijing_china.streets.bin","/cad2/ece297s/public/maps/beijing_china.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/cairo_egypt.streets.bin","/cad2/ece297s/public/maps/cairo_egypt.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin","/cad2/ece297s/public/maps/cape-town_south-africa.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin","/cad2/ece297s/public/maps/golden-horseshoe_canada.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/hamilton_canada.streets.bin","/cad2/ece297s/public/maps/hamilton_canada.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/hong-kong_china.streets.bin","/cad2/ece297s/public/maps/hong-kong_china.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/iceland.streets.bin","/cad2/ece297s/public/maps/iceland.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin","/cad2/ece297s/public/maps/interlaken_switzerland.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/london_england.streets.bin","/cad2/ece297s/public/maps/london_england.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/moscow_russia.streets.bin","/cad2/ece297s/public/maps/moscow_russia.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/new-delhi_india.streets.bin","/cad2/ece297s/public/maps/new-delhi_india.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/new-york_usa.streets.bin","/cad2/ece297s/public/maps/new-york_usa.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin","/cad2/ece297s/public/maps/rio-de-janeiro_brazil.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/saint-helena.streets.bin","/cad2/ece297s/public/maps/saint-helena.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/singapore.streets.bin","/cad2/ece297s/public/maps/singapore.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/sydney_australia.streets.bin","/cad2/ece297s/public/maps/sydney_australia.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/tehran_iran.streets.bin","/cad2/ece297s/public/maps/tehran_iran.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/tokyo_japan.streets.bin","/cad2/ece297s/public/maps/tokyo_japan.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/toronto_canada.streets.bin","/cad2/ece297s/public/maps/toronto_canada.osm.bin"));    
}

void loadFeaturePriority(){
    feature_priority.push_back("lake");
    feature_priority.push_back("island");
    feature_priority.push_back("beach");
    feature_priority.push_back("park");
    feature_priority.push_back("greenspace");
    feature_priority.push_back("golfcourse");
    feature_priority.push_back("river");
    feature_priority.push_back("stream");
    feature_priority.push_back("building");
}

void load_map(){
    loadOSMDatabaseBIN(map_paths.find(map_load_path) -> second);
    
    street_segments.resize(getNumStreetSegments());
    street_segments_of_street.resize(getNumStreets());
    
    double max_lon = getIntersectionPosition(0).longitude();
    double min_lon = max_lon;
    double max_lat = getIntersectionPosition(0).latitude();
    double min_lat = max_lat;
    
    for(int i = 0; i < getNumIntersections(); i++){
        max_lon = std::max(max_lon, getIntersectionPosition(i).longitude());
        min_lon = std::min(min_lon, getIntersectionPosition(i).longitude());
        max_lat = std::max(max_lat, getIntersectionPosition(i).latitude());
        min_lat = std::min(min_lat, getIntersectionPosition(i).latitude());
    }
    
    bounds.lat_avg = ((max_lat + min_lat)/2) * kDegreeToRadian;
    
    bounds.max_x = kEarthRadiusInMeters * max_lon * cos(bounds.lat_avg) * kDegreeToRadian;
    bounds.max_y = kEarthRadiusInMeters * max_lat * kDegreeToRadian;
    bounds.min_x = kEarthRadiusInMeters * min_lon * cos(bounds.lat_avg) * kDegreeToRadian;
    bounds.min_y = kEarthRadiusInMeters * min_lat * kDegreeToRadian;
    
    
    for(int i =0;i < getNumberOfWays();i++){
        const OSMWay *curr = getWayByIndex(i);
        for (int j = 0; j < getTagCount(curr); j++) {
            std::pair<std::string, std::string> tagPair = getTagPair(curr, j);
            if(tagPair.first == "highway"){
                std::pair<OSMID,std::string> street_type = std::make_pair(curr->id(),tagPair.second);
                street_types.insert(street_type);
                break;
            }
        }
    }
    
    
    for(int street_segment_id = 0;street_segment_id < getNumStreetSegments();street_segment_id++){
        struct StreetSegmentInfo street_seg_info = getStreetSegmentInfo(street_segment_id);
        int numCurvePoints = street_seg_info.numCurvePoints;
        
        street_segments[street_segment_id].speed_limit = street_seg_info.speedLimit;
        street_segments[street_segment_id].street_id = street_seg_info.streetID;
        street_segments[street_segment_id].one_way = street_seg_info.oneWay;
        street_segments[street_segment_id].name = getStreetName(street_seg_info.streetID);
        street_segments[street_segment_id].highlight = false;
        
        street_segments_of_street[street_seg_info.streetID].push_back(street_segment_id);
        
        
        auto it = street_types.find(street_seg_info.wayOSMID);
        street_segments[street_segment_id].segment_type = it -> second;
        //if the street segment is straight
        if(numCurvePoints == 0){
            
            LatLon pos_from = getIntersectionPosition(street_seg_info.from);
            LatLon pos_to = getIntersectionPosition(street_seg_info.to);
            
            ezgl::point2d coordinate = convertCoordinates(pos_from.longitude(),pos_from.latitude(),bounds.lat_avg);
            street_segments[street_segment_id].coordinates.push_back(coordinate);
            
            
            coordinate = convertCoordinates(pos_to.longitude(),pos_to.latitude(),bounds.lat_avg);
            street_segments[street_segment_id].coordinates.push_back(coordinate);
           
        }
        
        //if the street segment has curve points
        else{
            LatLon point = getIntersectionPosition(street_seg_info.from);
            ezgl::point2d coordinate = convertCoordinates(point.longitude(),point.latitude(),bounds.lat_avg);
            street_segments[street_segment_id].coordinates.push_back(coordinate);
            
            for(int i = 0; i < numCurvePoints; i++){
                point = getStreetSegmentCurvePoint(street_segment_id,i);
                coordinate = convertCoordinates(point.longitude(),point.latitude(),bounds.lat_avg);
                street_segments[street_segment_id].coordinates.push_back(coordinate);
            }

            point = getIntersectionPosition(street_seg_info.to);
            coordinate = convertCoordinates(point.longitude(),point.latitude(),bounds.lat_avg);
            street_segments[street_segment_id].coordinates.push_back(coordinate);
        }
        
        
        //get angle of street
        double delta_x = street_segments[street_segment_id].coordinates[0].x - street_segments[street_segment_id].coordinates[1].x;
        double delta_y = street_segments[street_segment_id].coordinates[0].y - street_segments[street_segment_id].coordinates[1].y;
        double theta = atan2(delta_y, delta_x)* (180/3.141592653);

        

        
        street_segments[street_segment_id].angle = theta;
    }
    
   
    
    
    //-------------------------------------------------
    
    features.resize(getNumFeatures());
    
    for(FeatureIdx featureidx = 0; featureidx<getNumFeatures(); featureidx++){
        features[featureidx].name = getFeatureName(featureidx);
        features[featureidx].feature_type = asString(getFeatureType(featureidx));
        features[featureidx].area = findFeatureArea(featureidx);
        features[featureidx].numFeaturePoints = getNumFeaturePoints(featureidx);
        for(int i = 0; i < features[featureidx].numFeaturePoints; i++){
            LatLon point = getFeaturePoint(featureidx,i);
            ezgl::point2d coordinate = convertCoordinates(point.longitude(),point.latitude(),bounds.lat_avg);
            features[featureidx].coordinates.push_back(coordinate);
            //std::cout<<" wwww\n"<<features[featureidx].feature_type;
        }
    }
    
    std::sort(features.begin(),features.end());
    
    //Gets coordinate of each intersection
    //-------------------------------------------------
    //intersection_coordinates.resize(getNumIntersections());
    intersections.resize(getNumIntersections());
    
    for (int i = 0; i < getNumIntersections(); i++) {
        LatLon point = getIntersectionPosition(i);
        ezgl::point2d coordinate = convertCoordinates(point.longitude(), point.latitude(), bounds.lat_avg);
        intersections[i].highlight = false;
        intersections[i].position = point;
        intersections[i].coordinate = coordinate;
        intersections[i].name = getIntersectionName(i);
    }
 

    //-------------------------------
   pointOfInterests.resize(getNumPointsOfInterest());
   std::vector <std::string> types;
   // int count = 0;
    
    for (POIIdx poiidx = 0; poiidx < getNumPointsOfInterest(); poiidx++){
        std::string poitype = getPOIType(poiidx);
        std::string poiname = getPOIName(poiidx);
        pointOfInterests[poiidx].name = getPOIName(poiidx);
        pointOfInterests[poiidx].type = poitype;
        LatLon point = getPOIPosition(poiidx);
       ezgl::point2d coordinate = convertCoordinates(point.longitude(),point.latitude(),bounds.lat_avg);
        pointOfInterests[poiidx].coordinate = coordinate;
        //types.push_back(getPOIType(poiidx));
        
        if(poitype.compare("fuel")==0||poitype.compare("parking")==0){
            if(poitype=="parking"){
                parking.push_back(POI_data{coordinate,poiname,poitype});
            }
            else {fuel.push_back(POI_data{coordinate,poiname,poitype});}
        }else if (poitype.compare("theatre")==0||poitype.compare("cinema")==0||poitype.compare("post_office")==0){
            if (poitype.compare("post_office")==0) post_office.push_back(POI_data{coordinate,poiname,poitype});
            else theatre.push_back(POI_data{coordinate,poiname,poitype});
        }else if (poitype=="school"||poitype=="kindergarten"||poitype=="preschool"||poitype=="college"||poitype=="university"){
            school.push_back(POI_data{coordinate,poiname,poitype});
        }else if (poitype=="bank"||poitype=="artwork"||poitype=="arts_centre"){
            if(poitype=="artwork"||poitype=="arts_centre"){
                art.push_back(POI_data{coordinate,poiname,poitype});
            }else{
            bank.push_back(POI_data{coordinate,poiname,poitype});
            }
        }else if (poitype=="pharmacy"||poitype=="restaurant"){
            if(poitype=="restaurant") restaurant.push_back(POI_data{coordinate,poiname,poitype});
            else pharmacy.push_back(POI_data{coordinate,poiname,poitype});
        }else if (poitype=="pub"||poitype=="bar"||poitype=="gym"||poitype=="gymnasium"||poitype=="swimming_pool"||poitype=="pool; fitness centre; ice rinks"){
            if (poitype=="gym"||poitype=="gymnasium"||poitype=="swimming_pool"||poitype=="pool; fitness centre; ice rinks"){
                gym.push_back(POI_data{coordinate,poiname,poitype});
                
            }
            else {
                bar.push_back(POI_data{coordinate,poiname,poitype});
            }
        }else if (poitype=="fast_food"||poitype.compare("cafe")==0){
            if(poitype.compare("cafe")==0){
                cafe.push_back(POI_data{coordinate,poiname,poitype});
            }
            else fastfood.push_back(POI_data{coordinate,poiname,poitype});            
        }
        else if (poitype=="hospital"||poitype=="doctors"||poitype.compare("library")==0){       
            if (poitype.compare("library")==0){
                library.push_back(POI_data{coordinate,poiname,poitype});
                //std::cout<<getPOIName(poiidx)<<std::endl;
            }
            else{
                hospital.push_back(POI_data{coordinate,poiname,poitype});
            }
            
        }
    }

   if(map_load_path.compare("/cad2/ece297s/public/maps/tehran_iran.streets.bin")==0) default_font = "Noto Sans Arabic";  
   else default_font = "Noto Sans CJK SC";

}

    

void drawAllStreets(ezgl::renderer *g, double zoom,bool heavy){
    bool draw = false;
    g -> set_line_cap(ezgl::line_cap::round);
    for(int i = 0;i < street_segments.size(); i++){
        draw = false;
        if(street_segments[i].segment_type == "motorway" ||street_segments[i].segment_type == "motorway_link"){
            draw = true;
            g ->set_color(246,207,101,255);
            if(street_segments[i].segment_type == "motorway_link" && zoom < 7){
                draw = false;
            }
            if(zoom > 25000){
                g->set_line_width(26);
            }
            else if(zoom > 1100){
                g->set_line_width(20);
            }
            else if(zoom > 50){
                g ->set_line_width(8);
            }
            else if (zoom > 7){
                g ->set_line_width(5);
            }
            else{
                g ->set_line_width(3);
            }
        }
        else if(street_segments[i].segment_type == "primary" || street_segments[i].segment_type == "secondary" ||street_segments[i].segment_type == "trunk"){
            draw = true;
            g ->set_color(200, 200, 200,255);
            if(zoom > 25000){
                g->set_line_width(26);
            }
            else if(zoom > 9000){
                g->set_line_width(22);
            }
            else if(zoom > 3200){
                g->set_line_width(18);
            }
            else if(zoom > 1100){
                g->set_line_width(16);
            }
            else if(zoom > 420){
                g->set_line_width(11);
            }
            else if(zoom > 150){
                g->set_line_width(6);
            }
            else if (zoom > 50){
                g->set_line_width(5);
            }
            else if (zoom > 19){
                g->set_line_width(4);
            }
            else if(zoom > 7){
                g->set_line_width(2);
            }
            else{
                g ->set_line_width(1);
            }

        }
        else if(street_segments[i].segment_type == "tertiary" || street_segments[i].segment_type == "unclassified" || street_segments[i].segment_type == "living_street"){
            if(zoom > 7){
                g ->set_color(200, 200, 200,255);
                draw = true;
            }
            if(zoom > 25000){
              g -> set_line_width(16);
              if(heavy) g->set_line_width(13);
            }
            else if(zoom >9000){
              g -> set_line_width(12);
              if(heavy) g->set_line_width(9);
            }
            else if(zoom > 3200){
                g->set_line_width(9);
                if(heavy) g->set_line_width(6);
            }
            else if(zoom > 1100){
                g->set_line_width(6);
            }
            else if(zoom > 420){
                g ->set_line_width(4);
            }
            else if(zoom > 150){
                g ->set_line_width(3);
            }
            else if (zoom > 19){
                g->set_line_width(2);
                if(heavy) draw = false;
            }
            else if(zoom > 7){
                g -> set_line_width(1);
                if(heavy) draw = false;
            }
        }
        else if (street_segments[i].segment_type == "residential"){
            if(zoom > 54){
                g ->set_color(200, 200, 200,255);
                draw = true;
            }
            if(zoom > 25000){
              g -> set_line_width(18);
              if(heavy) g->set_line_width(15);
              
            }
            else if(zoom > 9000){
              g -> set_line_width(11);
              if(heavy) g->set_line_width(8);
              
            }
            else if(zoom > 3200){
                g->set_line_width(9);
                if(heavy) g->set_line_width(6);
                
            }
            else if(zoom > 1100){
                g->set_line_width(6);
                if(heavy){
                    g->set_line_width(3);
                }
            }
            else if(zoom > 420){
                g ->set_line_width(3);
                if(heavy) g->set_line_width(1);
            }
            else if(zoom > 150){
                g ->set_line_width(2);
                if(heavy) g->set_line_width(0);
            }
            else if(zoom > 19){
                g -> set_line_width(1);
                if(heavy) draw = false;
            }
        }
        else{
            if(zoom > 150){
            g ->set_color(200, 200, 200,255);
            draw = true;
            }
            if(zoom > 25000){
                g -> set_line_width(16);
                if(heavy) g->set_line_width(13);
            }
            else if(zoom > 9000){
                g -> set_line_width(10);
                if(heavy) g->set_line_width(10);
            }
            else if(zoom > 3200){
                g->set_line_width(6);
                if(heavy) g->set_line_width(4);
            } 
            else if(zoom > 1100){
                g->set_line_width(3);
                if(heavy) g->set_line_width(2);
            }
            else if(zoom > 420){
                g->set_line_width(2);
                if(heavy) g->set_line_width(1);
            }
            else if(zoom > 150){
                g->set_line_width(1);
            }
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            if(draw){
                g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
            }
        }
    }
    
}
bool checkOverlap (ezgl::renderer *g, std::vector<ezgl::rectangle> &drawn, ezgl::point2d coordinate){
    //ezgl::point2d offset {width/2, height};
    ezgl::rectangle source = ezgl::rectangle(coordinate, 3.0, -5.0);
             //std::cout<<point.x<<" "<<point.y<<std::endl;
             
    ezgl::point2d target = g->world_to_screen(source).bottom_left();
    for(int i = 0; i <drawn.size(); i++){
       
        if (drawn[i].contains(target)){
            return true;
        }
    }
    return false;
}

void convert_point(ezgl::renderer *g, std::vector<ezgl::rectangle> &drawn, ezgl::point2d point){
    ezgl::point2d offset {-156, 80};
    ezgl::rectangle source = ezgl::rectangle(point, 3.0, -5.0);
             //std::cout<<point.x<<" "<<point.y<<std::endl;
             
             ezgl::point2d target = g->world_to_screen(source).bottom_left();
             //std::cout<<target.x<<" "<<target.y<<std::endl;
             
             g->set_coordinate_system(ezgl::SCREEN);
             //g->fill_rectangle(target+offset, 310,-117);
             
             drawn.push_back(ezgl::rectangle (target+offset, 310,-197));
             std::cout<<drawn.size()<<std::endl;
             g->set_coordinate_system(ezgl::WORLD);
}

void drawOneWays(ezgl::renderer *g, double zoom){
    std::vector<ezgl::rectangle> drawn_arrow;
    g ->set_color(ezgl::BLACK);
    bool draw;
    for(int i = 0;i < street_segments.size(); i++){
        draw = false;
        if(street_segments[i].segment_type == "motorway_link"){
                draw = true;
        }
        else if(street_segments[i].segment_type == "primary" || street_segments[i].segment_type == "secondary" ||street_segments[i].segment_type == "trunk"){
            draw = true;
        }
        else if(street_segments[i].segment_type == "tertiary" || street_segments[i].segment_type == "unclassified" || street_segments[i].segment_type == "living_street"){     
            draw = true;
        }
        else if (street_segments[i].segment_type == "residential"){
            draw = true;
        }
        ezgl::point2d start = street_segments[i].coordinates[0];
        ezgl::point2d finish = street_segments[i].coordinates[street_segments[i].coordinates.size()-1];
        if(draw && street_segments[i].one_way&& g->get_visible_world().contains(start)){
            double angle = street_segments[i].angle;
            if(angle < 0) angle += 180;
            g ->set_text_rotation(angle);
            
            
            if(!checkOverlap(g,drawn_arrow,start)){
                convert_point(g, drawn_arrow, start);
            if(start.x - finish.x <= 0 && start.y - finish.y >= 0){
                if(zoom > 25107){
                    g -> draw_text(start,"←",100,100);
                }
                else{
                    g -> draw_text(start,"←",50,50);
                }
            }
            else if(start.x - finish.x <= 0 && start.y - finish.y <= 0){
                if(zoom > 25107){
                    g -> draw_text(start,"→",100,100);
                }
                else{
                    g -> draw_text(start,"→",50,50);
                }
            }
            else if(start.x - finish.x >= 0 && start.y - finish.y >= 0){
                if(zoom > 25107){
                    g -> draw_text(start,"←",100,100);
                }
                else{
                    g -> draw_text(start,"←",50,50);
                }
            }
            else{
                if(zoom > 25107){
                    g -> draw_text(start,"→",100,100);
                }
                else{
                    g -> draw_text(start,"→",50,50);
                }
            }
        }
        }
        
    }
    drawn_arrow.clear();
}


void draw_features(ezgl::renderer *g, double zoom){
    for (int i = 0; i<features.size();i++){
        if (features[i].feature_type == "park"&&features[i].numFeaturePoints>1){
            g ->set_color(185, 226, 168,255);
            if(features[i].area>200000){
                //std::cout<<features[i].area;
            g->fill_poly(features[i].coordinates);
            } else if (features[i].area>10000&&zoom>5){
                g->fill_poly(features[i].coordinates);
            }else if (features[i].area<=10000&&zoom>100){
                g->fill_poly(features[i].coordinates);
            }
        } else if (features[i].feature_type == "golfcourse"&&features[i].numFeaturePoints>1){
            g ->set_color(178, 217, 163,255);
            
            g->fill_poly(features[i].coordinates);
        }else if (features[i].feature_type == "greenspace"&&features[i].numFeaturePoints>1){
            g ->set_color(171, 209, 157,255);
            if(features[i].area>200000){
            g->fill_poly(features[i].coordinates);
            }
            else if (features[i].area>10000&&zoom>5){
                g->fill_poly(features[i].coordinates);
            }else if (features[i].area<=10000&&zoom>100){
                g->fill_poly(features[i].coordinates);
            }
        }
        else if(features[i].feature_type == "lake" &&features[i].numFeaturePoints>1){
            g ->set_color(170, 218, 255,255);
            if(features[i].area>200000){
            g->fill_poly(features[i].coordinates);
            }
            else if (features[i].area>10000&&zoom>5){
                g->fill_poly(features[i].coordinates);
            }else if (features[i].area<=10000&&zoom>100){
                g->fill_poly(features[i].coordinates);
            }
        }
        else if(features[i].feature_type == "island"&&features[i].numFeaturePoints>1){
            g ->set_color(232, 232, 232,255);
          
            g->fill_poly(features[i].coordinates);
        }else if(features[i].feature_type == "beach"&&features[i].numFeaturePoints>1){
            g ->set_color(255, 242, 175,255);
            
            g->fill_poly(features[i].coordinates);
        }else if(features[i].feature_type == "building"&&features[i].numFeaturePoints>1&&zoom>1500){
            g ->set_color(180, 180, 180,255);
          
            g->fill_poly(features[i].coordinates);
        }else if((features[i].feature_type == "stream"||features[i].feature_type == "river")&&features[i].numFeaturePoints>1){
            g ->set_color(170, 218, 255,255);
            g->set_line_width(0);
            if(features[i].feature_type == "river") g->set_line_width(3);
            for (int j = 0;(j<features[i].coordinates.size()-1)&&zoom>100;j++){
                g->draw_line(features[i].coordinates[j], features[i].coordinates[j+1]);
            }
       
        }
    }
}

void text(ezgl::renderer *g, std::string word, ezgl::color color, ezgl::point2d point){

    std::string font = "Noto Sans CJK SC";
    //font_slant a = 0;
    g->format_font(default_font, ezgl::font_slant::normal, ezgl::font_weight::normal);
     g->set_font_size(22);
             g->set_text_rotation(0);
             g->set_color(color);

             g->draw_text(point, word, 50, 20);


}
//all png used in this function come from https://mapicons.mapsmarker.com/
void draw_POI (ezgl::renderer *g, double zoom, ezgl::point2d small, ezgl::point2d large){    
     ezgl::surface *png_surface; 
     png_surface = ezgl::renderer::load_png("libstreetmap/resources/blank.png");
     ezgl::point2d point {.0,.0};
     ezgl::point2d offset {-56, 40};
     ezgl::point2d text_offset {30,0};
     std::vector<ezgl::rectangle>  drawn;
     int count = 0;
     for(int i = 0; i<restaurant.size();i++){
         
         if((restaurant[i].coordinate<large)&&(small<restaurant[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, restaurant[i].coordinate))){  
             count++;             
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/restaurant.png"); //zoom             
             point=restaurant[i].coordinate;             
             convert_point(g, drawn, point);
             /*ezgl::rectangle source = ezgl::rectangle(point, 3.0, -5.0);
             //std::cout<<point.x<<" "<<point.y<<std::endl;
             
             ezgl::point2d target = g->world_to_screen(source).bottom_left();
             //std::cout<<target.x<<" "<<target.y<<std::endl;
             
             g->set_coordinate_system(ezgl::SCREEN);
             
             
             drawn.push_back(ezgl::rectangle (target+offset, 110,-117));
             g->set_coordinate_system(ezgl::WORLD);
            */
             
             g->draw_surface(png_surface, point);
             text(g, restaurant[i].name, ezgl::color {211, 108, 0,255}, point);
             //std::cout<<restaurant[i].name<<std::endl;
             //ezgl::renderer::free_surface(png_surface);             
         }         
         
     }
     
     
     count = 0;
     for (int i = 0; i < fuel.size(); i++){         
         if(fuel[i].coordinate<large&&(small<fuel[i].coordinate)&&(!checkOverlap(g,drawn, fuel[i].coordinate))){
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/fillingstation.png");
             point=fuel[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, fuel[i].name, ezgl::color {121, 134, 203,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     for (int i = 0; i < bar.size(); i++){         
         if(bar[i].coordinate<large&&(small<bar[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, bar[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/bar.png");//zoom
             point=bar[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, bar[i].name, ezgl::color {211, 108, 0,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
     for (int i = 0; i < school.size(); i++){         
         if(school[i].coordinate<large&&(small<school[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, school[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/university.png");
             point=school[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, school[i].name, ezgl::color {84,145,245,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
     for (int i = 0; i < pharmacy.size(); i++){         
         if(pharmacy[i].coordinate<large&&(small<pharmacy[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, pharmacy[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/drugstore.png");
             point=pharmacy[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, pharmacy[i].name, ezgl::color {238, 103, 92,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     for (int i = 0; i < theatre.size(); i++){         
         if(theatre[i].coordinate<large&&(small<theatre[i].coordinate)&&(!checkOverlap(g,drawn, theatre[i].coordinate))){
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/theater.png");
             point=theatre[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, theatre[i].name, ezgl::color {52, 68, 149,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
     for (int i = 0; i < bank.size(); i++){         
         if(bank[i].coordinate<large&&(small<bank[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, bank[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/bank.png");
             point=bank[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, bank[i].name, ezgl::color {92, 108, 191,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count =0;
     for (int i = 0; i < cafe.size(); i++){         
         if(cafe[i].coordinate<large&&(small<cafe[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, cafe[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/coffee.png");//zoom
             point=cafe[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, cafe[i].name, ezgl::color {211, 108, 0,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
     for (int i = 0; i < parking.size(); i++){         
         if(parking[i].coordinate<large&&(small<parking[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, parking[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/parking.png");
             point=parking[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, parking[i].name, ezgl::color {47, 59, 118,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     for (int i = 0; i < post_office.size(); i++){         
         if(post_office[i].coordinate<large&&(small<post_office[i].coordinate)&&(!checkOverlap(g,drawn, post_office[i].coordinate))){
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/postal.png");
             point=post_office[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, post_office[i].name, ezgl::color {47, 59, 118,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
    for (int i = 0; i < hospital.size(); i++){         
         if(hospital[i].coordinate<large&&(small<hospital[i].coordinate)&&count<0&&(!checkOverlap(g,drawn, hospital[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/hospital.png");
             point=hospital[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
            text(g, hospital[i].name, ezgl::color {238, 103, 92,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
     for (int i = 0; i < fastfood.size(); i++){         
         if(fastfood[i].coordinate<large&&(small<fastfood[i].coordinate)&&count<5&&(!checkOverlap(g,drawn, fastfood[i].coordinate))){
             count++;
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/fastfood.png");//zoom
             point=fastfood[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, fastfood[i].name, ezgl::color {211, 108, 0,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     count = 0;
     for (int i = 0; i < art.size(); i++){         
         if(art[i].coordinate<large&&(small<art[i].coordinate)&&(!checkOverlap(g,drawn, art[i].coordinate))){
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/art.png");//zoom
             point=art[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, art[i].name, ezgl::color {18, 181, 203,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     for (int i = 0; i < gym.size(); i++){         
         if(gym[i].coordinate<large&&(small<gym[i].coordinate)&&(!checkOverlap(g,drawn, gym[i].coordinate))){
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/gym.png");//zoom
             point=gym[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, gym[i].name, ezgl::color {47, 59, 118,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     for (int i = 0; i < library.size(); i++){         
         if(library[i].coordinate<large&&(small<library[i].coordinate)&&(!checkOverlap(g,drawn, library[i].coordinate))){
             png_surface = ezgl::renderer::load_png("libstreetmap/resources/library.png");//zoom
             point=library[i].coordinate;
             convert_point(g, drawn, point);
             g->draw_surface(png_surface, point);
             text(g, library[i].name, ezgl::color {47, 59, 118,255}, point);
             //ezgl::renderer::free_surface(png_surface);
         }       
     }
     
     ezgl::renderer::free_surface(png_surface);   

     drawn.clear();
    

}

void draw_street_names (ezgl::renderer *g) {
    
    ezgl::rectangle world = g->get_visible_world();

    g->format_font(default_font, ezgl::font_slant::normal, ezgl::font_weight::normal);

    double width = world.width();
    //std::cout << width << std::endl;
    std::string previous;

     
    for (int i = 0; i < getNumStreetSegments(); i++) {
        
        g->set_color(0,0,0);
        g->set_font_size(12);
        
        //if straight street segment
        if (street_segments[i].coordinates.size() == 2 ) {

            double x = (street_segments[i].coordinates[1].x + street_segments[i].coordinates[0].x)/2;
            double y = (street_segments[i].coordinates[1].y + street_segments[i].coordinates[0].y)/2;
            
            double xBound = std::abs(street_segments[i].coordinates[1].x - street_segments[i].coordinates[0].x);
            double yBound = std::abs(street_segments[i].coordinates[1].y - street_segments[i].coordinates[0].y);
            
            double theta = street_segments[i].angle;
            if (theta > 90) theta -= 180;
            if (theta < -90) theta += 180;
            
            if (x < world.right() && x > world.left() && y < world.top() && y > world.bottom()) {
            

                
                if (street_segments[i].name != "<unknown>" && street_segments[i].name != previous) {

                    //if (street_segments[i])
                    if ((street_segments[i].segment_type == "motorway" ||
                        street_segments[i].segment_type == "primary" )&& width < 10000 ) {
                        g->set_text_rotation(theta);
                        g->draw_text({x, y}, street_segments[i].name, xBound, yBound);
                    }
                    else if ((street_segments[i].segment_type == "trunk" ||
                            street_segments[i].segment_type == "secondary")&& width < 7000) {
                        g->set_text_rotation(theta);
                        g->set_font_size(14);
                        g->draw_text({x, y}, street_segments[i].name, xBound, yBound);
                    }
                    else if((street_segments[i].segment_type == "tertiary" || 
                            street_segments[i].segment_type == "unclassified" || 
                            street_segments[i].segment_type == "living_street" ||
                            street_segments[i].segment_type == "residential") && width < 3000){
                        g->set_text_rotation(theta);
                        g->set_font_size(16);
                        g->draw_text({x, y}, street_segments[i].name, xBound, yBound);
                    } 
                    else if (width < 1000){
                        
                        g->set_text_rotation(theta);
                        g->set_font_size(20);
                        g->draw_text({x, y}, street_segments[i].name, xBound, yBound);
                    }
                    //std::cout << street_segments[i].name << " " << street_segments[i].angle << std::endl;
                    
                    previous = street_segments[i].name;
                }

            }
        } 
        //handle curved streets
        else if (street_segments[i].coordinates.size() > 2) {
            
        } 
        
        
        //global_app->refresh_drawing();
        
    }

}


void drawHighlights(ezgl::renderer *g){
    ezgl::surface *png_surface;
    
    png_surface = ezgl::renderer::load_png("libstreetmap/resources/pin.png");
    
    ezgl::rectangle world = g->get_visible_world();
    
    for(int i = 0;i < intersections.size();i++){
        
        double x = intersections[i].coordinate.x;
        double y = intersections[i].coordinate.y;
        
        //check that intersections are within world bounds
        if (x < world.right() && x > world.left() && y < world.top() && y > world.bottom()) {
            if(intersections[i].highlight){
                g -> draw_surface(png_surface,intersections[i].coordinate);
                
            }
        }
        
    }
    
    //highlight street 
    for (int i = 0; i < street_segments.size(); i++) {
        double x = (street_segments[i].coordinates[1].x + street_segments[i].coordinates[0].x)/2;
        double y = (street_segments[i].coordinates[1].y + street_segments[i].coordinates[0].y)/2;
        
         if (x < world.right() && x > world.left() && y < world.top() && y > world.bottom()) {
             if (street_segments[i].highlight) {
                 g->set_line_width(20);
                 g->set_color(ezgl::RED);
                 for (int j = 0; j < street_segments[i].coordinates.size()-1; j++) {
                     g->draw_line(street_segments[i].coordinates[j], street_segments[i].coordinates[j+1]);
                 }
             }
         }
        
        
    }
    
    ezgl::renderer::free_surface(png_surface);
}

void clearHighlights(){
    for(int i = 0;i < intersections.size();i++){
        intersections[i].highlight = false;
    }
    
    for (int i = 0; i < street_segments.size(); i++) {
        street_segments[i].highlight = false;
    }
}



void draw_main_canvas (ezgl::renderer *g){
    bool heavy_map = false;
    ezgl::rectangle world = g->get_visible_world();
    double area = world.area();
    double zoom = bounds.area/area;
    ezgl::point2d small = world.bottom_left();
    ezgl::point2d large = world.top_right();
    
    std::cout<<zoom<<std::endl;

    g -> set_color(243,243,239,255); 
    g -> fill_rectangle(world);
    
    if(getNumStreetSegments() > 1000000){
        zoom /= 3;
        heavy_map = true;
    }
    draw_features(g,zoom);
    drawAllStreets(g,zoom,heavy_map);       
    
    if(zoom > 3249){
        drawOneWays(g,zoom);
    }
    
    //draw street names
    draw_street_names(g);


    drawHighlights(g);


     if(zoom>1500){
        draw_POI(g,zoom,small, large);
    }


}


//UI function declarations for convenience
void search_entry(GtkEntry* entry );
void search_entry_activate(GtkEntry* entry);
void find_button(GtkWidget * /*widget*/ , ezgl::application *app);
void map_list(GtkListBox* box);


void initial_setup(ezgl::application *application, bool){
    ezgl::rectangle world = application-> get_renderer()->get_visible_world();
    bounds.area = world.area();
    
    //UI stuff
    //connect search bar entry as signal
    GtkEntry* entry = (GtkEntry*) application->get_object("SearchEntry");
    
    g_signal_connect(entry, 
                     "search-changed", 
                     G_CALLBACK(search_entry), 
                     entry);
    
    g_signal_connect(entry, 
                     "activate", 
                     G_CALLBACK(search_entry_activate),  
                     entry);

    
    
    //connect "Find" button as signal
    GtkWidget* findButton = (GtkWidget*) application->get_object("SearchFindButton");
    g_signal_connect(findButton, "clicked", G_CALLBACK(find_button), application);
    
    //listbox for picking new maps
    GtkListBox* mapList = (GtkListBox*) application->get_object("MapList");
    g_signal_connect(mapList, "row-activated", G_CALLBACK(map_list), mapList);
    
    
    //for autocorrect
    entryCompletion.completion = (GtkEntryCompletion*) global_app->get_object("SearchEntryCompletion");
    gtk_entry_set_completion(entry, entryCompletion.completion);
}



void act_on_mouse_click(ezgl::application* app, GdkEventButton* event,double x, double y){
    if(event -> button == 1){
        clearHighlights();
        LatLon position = latLonFromWorld(x,y);
        int id = findClosestIntersection(position);
        intersections[id].highlight = true;
        app -> refresh_drawing();
        app -> update_message("Pin placed at " + intersections[id].name);
    }
    else if(event -> button == 3){
        clearHighlights();
        app -> refresh_drawing();
        app -> update_message("Pin removed");
    }
}

void drawMap(){
    load_bin();
    loadFeaturePriority();
    load_map();
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    
    ezgl::application application(settings);
    global_app = &application;
   
    ezgl::rectangle initial_background({bounds.min_x,bounds.min_y},{bounds.max_x,bounds.max_y});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_background);
    application.run(initial_setup, act_on_mouse_click, nullptr,nullptr);
    
    close_map();
    //connect search bar
    
}


//UI stuff from here
void map_list(GtkListBox* box) {
    
    GtkListBoxRow* selected = gtk_list_box_get_selected_row(box);
    
    std::string selectedText = gtk_widget_get_name((GtkWidget *)selected);
    
    std::cout << selectedText << std::endl;
    close_map();
    closeMap();
    map_load_path = selectedText;
    bool load_success = loadMap(map_load_path);
    if(!load_success) {
        std::cerr << "Failed to load map '" << map_load_path << "'\n";
    }
    load_map();
    ezgl::rectangle initial_background({bounds.min_x,bounds.min_y},{bounds.max_x,bounds.max_y});
    global_app -> change_canvas_world_coordinates("MainCanvas",initial_background);
    ezgl::zoom_fit(global_app -> get_canvas("MainCanvas"),initial_background);
    bounds.area = global_app -> get_renderer() -> get_visible_world().area();
    global_app -> refresh_drawing(); 
}

//searchEntry
void search_entry(GtkEntry* entry) {
      
    GtkListStore* store = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter iter;
    
    
    // Get the text written in the widget
    std::string text = gtk_entry_get_text(entry);
    
    std::vector<StreetIdx> street;
    
    if (text.size() > 2) {
        street = findStreetIdsFromPartialStreetName(text);
        
        if (street.size() > 0) {
            for (int i = 0; i < street.size(); i++) {
                std::string name = getStreetName(street[i]);
                
                    gtk_list_store_append (store, &iter);
                    gtk_list_store_set (store, &iter, 0, name.c_str(), -1);
                 
                 
            }
            
        }
    } 
    
    entryCompletion.completion_model = GTK_TREE_MODEL(store);
    gtk_entry_completion_set_model (entryCompletion.completion, entryCompletion.completion_model);
    gtk_entry_completion_set_text_column(entryCompletion.completion, 0);
    
    //gtk_list_store_clear(store);
    
    //after this include streets into entry completion
//    for (int i = 0; i < street.size(); i++) {
//        std::cout << getStreetName(street[i]) << " ";
//    }
//    std::cout << std::endl;
    
    
    //after that clear the entry and vector
    street.clear();
}

void search_entry_activate(GtkEntry* entry){
    
    //clear any previous highlights
    clearHighlights();
    
    // Get the text written in the widget
    std::string text = gtk_entry_get_text(entry);
    
    std::vector<StreetIdx> street;
    
    if (text.size() > 2) {
        street = findStreetIdsFromPartialStreetName(text);
    } 
     
    std::vector<StreetSegmentIdx> segments;
    
    if (street.size() > 0) {
        segments = street_segments_of_street[street[0]]; 
    } else {
        std::cout << "No matching street found" << std::endl;
    }
    
     
    for (int i = 0; i < segments.size(); i++) {
        street_segments[segments[i]].highlight = true;
        }
    
    global_app->refresh_drawing();
    street.clear();
    segments.clear();
    gtk_entry_set_text(entry, " ");
}

//Find Button - right now works through command line
void find_button(GtkWidget * /*widget*/, ezgl::application *app) {
    
    //clear any previous highlights
    clearHighlights();
    
            
    //for error handling
    GtkPopover* popOver = (GtkPopover*) app->get_object("FindPopOver");
    GtkLabel*   popOverLabel = (GtkLabel*) app->get_object("FindPopOverLabel");
    
    GtkEntry* entry = (GtkEntry*) app->get_object("SearchEntry");
    std::string input = gtk_entry_get_text(entry);
    
    std::string delim1 = "and";
    std::string delim2 = "&";
    
    std::string street1, street2;
    
    if (input.find(delim1) !=  std::string::npos) {
        street1 = input.substr(0, input.find(delim1));
        street2 = input.substr(input.find(delim1) + delim1.length(), input.size());
       
    } else if (input.find(delim2) != std::string::npos) {
        street1 = input.substr(0, input.find(delim2));
        street2 = input.substr(input.find(delim2) + delim2.length(), input.size());
        
        std::cout << street1 << " " << street2 << std::endl;
    } else {  
        gtk_popover_popup(popOver);    
        gtk_label_set_text(popOverLabel, "Put an 'and' or '&' \n between streets");
        return;
   }

    //find street ids
    std::vector<StreetIdx> streetOne = findStreetIdsFromPartialStreetName(street1);
    std::vector<StreetIdx> streetTwo = findStreetIdsFromPartialStreetName(street2);
    
    //for holding intersections
    std::vector<IntersectionIdx> intersectionResults;
    
    if (streetOne.size() > 0 && streetTwo.size() > 0) {
        //first matches are used as arguments
        std::pair<StreetIdx, StreetIdx> streets (streetOne[0], streetTwo[0]);
        
        intersectionResults = findIntersectionsOfTwoStreets(streets);
        
        if (intersectionResults.size() > 0) {
            for (int i = 0; i < intersectionResults.size(); i++) {
                intersections[intersectionResults[i]].highlight = true;
            }
        } else {
            gtk_popover_popup(popOver);    
            gtk_label_set_text(popOverLabel, "No intersections found");
            return;
        }
       
    } else {
        gtk_popover_popup(popOver);    
        gtk_label_set_text(popOverLabel, "No suitable streets found");
        return;
    }
    
    //clear entry
    gtk_entry_set_text(entry, " ");
    global_app -> refresh_drawing();
        
    //at the end delete vectors
    intersectionResults.clear();
 
}

void close_map(){
    closeOSMDatabase();
    street_segments.clear();
    features.clear();
    pointOfInterests.clear();
    street_segments_of_street.clear();
    fuel.clear();
    school.clear();
    restaurant.clear();
    parking.clear();
    theatre.clear();
    bank.clear();
    bar.clear();
    cafe.clear();
    fastfood.clear();
    pharmacy.clear(); 
    hospital.clear();
    post_office.clear();
    gym.clear();
    art.clear();
    library.clear();

    intersections.clear();

    street_types.clear();
    
}
