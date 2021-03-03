#include "StreetsDatabaseAPI.h"
#include "m2.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <math.h>
#include <iostream>
#include <vector>
#include <utility>
#include <point.hpp>

constexpr double kEarthRadiusInMeters = 6372797.560856;
constexpr double kDegreeToRadian = 0.017453292519943295769236907684886;

struct boundingbox{
    double max_x;
    double max_y;
    double min_x;
    double min_y;
    double lat_avg;
};

struct street_segment_data{
    std::vector<ezgl::point2d> coordinates;
    std::string name;
    float speed_limit;
};

std::vector<street_segment_data> street_segments;

struct boundingbox bounds;

//(x, y) = (R·lon·cos(latavg), R·lat)

ezgl::point2d convertCoordinates(double longitude, double latitude, double lat_avg){
    
    double x = kEarthRadiusInMeters * longitude * cos(lat_avg) * kDegreeToRadian;
    double y = kEarthRadiusInMeters * latitude * kDegreeToRadian;
    ezgl::point2d point(x,y);
    
    return point;
}


void load_map(){
    
    double max_lon = getIntersectionPosition(0).longitude();
    double min_lon = max_lon;
    double max_lat = getIntersectionPosition(0).latitude();
    double min_lat = max_lat;
    street_segments.resize(getNumStreetSegments());
    
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

        //if the street segment is straight
        if(numCurvePoints == 0){
            
            LatLon pos_from = getIntersectionPosition(street_seg_info.from);
            LatLon pos_to = getIntersectionPosition(street_seg_info.to);
            
            ezgl::point2d coordinate = convertCoordinates(pos_from.longitude(),pos_from.latitude(),bounds.lat_avg);
            street_segments[street_segment_id].coordinates.push_back(coordinate);
            
            coordinate = convertCoordinates(pos_to.longitude(),pos_to.latitude(),bounds.lat_avg);
            street_segments[street_segment_id].coordinates.push_back(coordinate);
            
            street_segments[street_segment_id].speed_limit = street_seg_info.speedLimit;
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
            
            street_segments[street_segment_id].speed_limit = street_seg_info.speedLimit;
        }
         
        
    }
 
}

void draw_main_canvas (ezgl::renderer *g){
    for(int i = 0;i < street_segments.size(); i++){
        if(street_segments[i].speed_limit >= 80){
            g ->set_color(ezgl::ORANGE);
            g->set_line_width(8); 
        }
        else if(street_segments[i].speed_limit >= 50){
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
void drawMap(){
    load_map();
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    
    ezgl::application application(settings);
    
   
    ezgl::rectangle initial_background({bounds.min_x,bounds.min_y},{bounds.max_x,bounds.max_y});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_background);
    
    
    application.run(nullptr,nullptr,nullptr,nullptr);
    
}