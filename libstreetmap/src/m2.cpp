#include "StreetsDatabaseAPI.h"
#include "m2.h"
#include "m1.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "OSMDatabaseAPI.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>

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
    float speed_limit;
    StreetIdx street_id;
    std::string segment_type;
    bool one_way;
};

std::vector<street_segment_data> street_segments;

std::unordered_map <OSMID,std::string> street_types;

std::unordered_map<std::string,std::string> map_paths;

struct boundingbox bounds;

//(x, y) = (R·lon·cos(latavg), R·lat)

bool operator< (const street_segment_data &a, const street_segment_data &b){
    return a.speed_limit < b.speed_limit;
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
    map_paths.insert(std::make_pair("//cad2/ece297s/public/maps/new-york_usa.streets.bin","/cad2/ece297s/public/maps/new-york_usa.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin","/cad2/ece297s/public/maps/rio-de-janeiro_brazil.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/saint-helena.streets.bin","/cad2/ece297s/public/maps/saint-helena.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/singapore.streets.bin","/cad2/ece297s/public/maps/singapore.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/sydney_australia.streets.bin","/cad2/ece297s/public/maps/sydney_australia.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/tehran_iran.streets.bin","/cad2/ece297s/public/maps/tehran_iran.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/tokyo_japan.streets.bin","/cad2/ece297s/public/maps/tokyo_japan.osm.bin"));
    map_paths.insert(std::make_pair("/cad2/ece297s/public/maps/toronto_canada.streets.bin","/cad2/ece297s/public/maps/toronto_canada.osm.bin"));    
}

void load_map(){

    
    loadOSMDatabaseBIN("/cad2/ece297s/public/maps/toronto_canada.osm.bin");
    
    street_segments.resize(getNumStreetSegments());
    
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
    }
 
}


//choose when to draw highway ramps, what colour and what line width too
void drawAllStreets(ezgl::renderer *g){
    ezgl::color colour;
    
    for(int i = 0;i < street_segments.size(); i++){
        if(street_segments[i].segment_type == "motorway"){
            colour = ezgl::ORANGE;
            g ->set_color(colour);
            g->set_line_width(8); 

        }
        else if(street_segments[i].segment_type == "primary" || street_segments[i].segment_type == "secondary" ||street_segments[i].segment_type == "trunk"){
            colour = ezgl::GREY_75;
            g ->set_color(colour);
            g->set_line_width(3);

        }
        else if(street_segments[i].segment_type == "tertiary" || street_segments[i].segment_type == "unclassified" || street_segments[i].segment_type == "living_street"){
            colour = ezgl::GREY_75;
            g ->set_color(colour);
            g->set_line_width(2);

        }
        else{
            colour = ezgl::GREY_75;
            g ->set_color(colour);
            g->set_line_width(1); 
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
            if(street_segments[i].one_way){
                g ->set_color(ezgl::BLACK);
                //drawArrow(g,street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
                g ->set_color(colour);
            }
        }
    }
    
}
void drawSomeStreets(ezgl::renderer *g){
    bool draw = false;
    for(int i = 0;i < street_segments.size(); i++){
        draw = false;
        if(street_segments[i].segment_type == "motorway"){
            g ->set_color(ezgl::ORANGE);
            g->set_line_width(8); 
            draw = true;
        }
        else if(street_segments[i].segment_type == "primary" || street_segments[i].segment_type == "secondary" ||street_segments[i].segment_type == "trunk"){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(3);
            draw = true;
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            if(draw){
                g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
            }
        }
    }
}

void drawMoreStreets(ezgl::renderer *g){
    bool draw = false;
    for(int i = 0;i < street_segments.size(); i++){
        draw = false;
        if(street_segments[i].segment_type == "motorway"){
            g ->set_color(ezgl::ORANGE);
            g->set_line_width(8); 
            draw = true;
        }
        else if(street_segments[i].segment_type == "primary" || street_segments[i].segment_type == "secondary" ||street_segments[i].segment_type == "trunk"){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(3);
            draw = true;
        }
        else if(street_segments[i].segment_type == "tertiary" || street_segments[i].segment_type == "unclassified" || street_segments[i].segment_type == "living_street"){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(1);
            draw = true;
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            if(draw){
                g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
            }
        }
    }
}

void drawMostStreets(ezgl::renderer *g){
    bool draw = false;
    for(int i = 0;i < street_segments.size(); i++){
        draw = false;
        if(street_segments[i].segment_type == "motorway"){
            g ->set_color(ezgl::ORANGE);
            g->set_line_width(8); 
            draw = true;
        }
        else if(street_segments[i].segment_type == "primary" || street_segments[i].segment_type == "secondary" ||street_segments[i].segment_type == "trunk"){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(3);
            draw = true;
        }
        else if(street_segments[i].segment_type == "tertiary" || street_segments[i].segment_type == "unclassified" || street_segments[i].segment_type == "living_street" || street_segments[i].segment_type == "residential"){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(1);
            draw = true;
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            if(draw){
                g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
            }
        }
    }
}

void draw_main_canvas (ezgl::renderer *g){
    ezgl::rectangle world = g->get_visible_world();
    double area = world.area();
    double zoom = bounds.area/area;
   // std::cout << bounds.area/area << std::endl;
    if(zoom > 165){
        drawAllStreets(g);
    }
    else if(zoom > 21){
        drawMostStreets(g);
    }
    else if(zoom > 2){
        drawMoreStreets(g);
    }
    else{
        drawSomeStreets(g);
    }
}
void initial_setup(ezgl::application *application, bool){
    ezgl::rectangle world = application-> get_renderer()->get_visible_world();
    bounds.area = world.area();
}

void drawMap(){
    load_map();
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    
    ezgl::application application(settings);
    
   
    ezgl::rectangle initial_background({bounds.min_x,bounds.min_y},{bounds.max_x,bounds.max_y});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_background);
    

    application.run(initial_setup,nullptr,nullptr,nullptr);
    
    closeOSMDatabase();
    
}
