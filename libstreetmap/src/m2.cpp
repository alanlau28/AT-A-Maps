#include "StreetsDatabaseAPI.h"
#include "m2.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <math.h>
#include <iostream>
#include <vector>
#include <utility>

constexpr double kEarthRadiusInMeters = 6372797.560856;
constexpr double kDegreeToRadian = 0.017453292519943295769236907684886;

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
};

struct street_data{
    std::vector<ezgl::point2d> coordinates;
    std::string name;
};

std::vector<street_segment_data> street_segments;

std::vector<street_data> streets;

struct boundingbox bounds;

//(x, y) = (R·lon·cos(latavg), R·lat)

ezgl::point2d convertCoordinates(double longitude, double latitude, double lat_avg){
    
    double x = kEarthRadiusInMeters * longitude * cos(lat_avg) * kDegreeToRadian;
    double y = kEarthRadiusInMeters * latitude * kDegreeToRadian;
    ezgl::point2d point(x,y);
    
    return point;
}


void load_map(){
    street_segments.resize(getNumStreetSegments());
    streets.resize(getNumStreets());
    
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
    
    for(int street_segment_id = 0;street_segment_id < getNumStreetSegments();street_segment_id++){
        struct StreetSegmentInfo street_seg_info = getStreetSegmentInfo(street_segment_id);
        int numCurvePoints = street_seg_info.numCurvePoints;
        street_segments[street_segment_id].speed_limit = street_seg_info.speedLimit;
        street_segments[street_segment_id].street_id = street_seg_info.streetID;

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

void draw_all_streets(ezgl::renderer *g){
    
    for(int i = 0;i < street_segments.size(); i++){
        if(street_segments[i].speed_limit >= 22.2){
            g ->set_color(ezgl::ORANGE);
            g->set_line_width(16); 
        }
        else if(street_segments[i].speed_limit >= 16.66){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(4); 
        }
        else{
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(2); 
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
        }
    }
    
}
void draw_some_streets(ezgl::renderer *g){
    for(int i = 0;i < street_segments.size(); i++){
        if(street_segments[i].speed_limit >= 22.2){
            g ->set_color(ezgl::ORANGE);
            g->set_line_width(16); 
        }
        else if(street_segments[i].speed_limit >= 16.66){
            g ->set_color(ezgl::GREY_75);
            g->set_line_width(4); 
        }
        for(int j = 0; j < street_segments[i].coordinates.size()-1; j++){
            if(street_segments[i].speed_limit >= 16.66){
                g->draw_line(street_segments[i].coordinates[j],street_segments[i].coordinates[j+1]);
            }
        }
    }
}

void draw_main_canvas (ezgl::renderer *g){
    ezgl::rectangle world = g->get_visible_world();
    double area = world.area();
    if(bounds.area/area > 459){
        draw_all_streets(g);
    }
    else{
        draw_some_streets(g);
    }
    std::cout << bounds.area/area << std::endl;
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
    
}