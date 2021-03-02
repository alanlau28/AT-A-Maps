#include "StreetsDatabaseAPI.h"
#include "m2.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <math.h>
#include <iostream>
#include <vector>
#include <utility>

/*constexpr double kEarthRadiusInMeters = 6372797.560856;
constexpr double kDegreeToRadian = 0.017453292519943295769236907684886;

struct intersection_data{
    LatLon position;
    std::string name;
};

std::vector<intersection_data> intersections;

*/

void draw_main_canvas (ezgl::renderer *g){
    
    
    
    g->draw_rectangle ({0,0},{1000,1000});
    
    
    /*for (size_t i = 0; i<intersections.size();++i){
      
        
        float lon = intersections[i].position.longitude() * kDegreeToRadian;
        float lat = intersections[i].position.latitude() * kDegreeToRadian;
        float x = kEarthRadiusInMeters * cos(lat) * cos(lon);
        float y = kDegreeToRadian * cos(lat) * sin(lon);
        
        float width = 0.001;
        float height =  width;
        
        g->fill_rectangle({x,y},{x+width, y+height});
        
        
    }*/
}
/*
void findMapBounds(){
double max_lon = getIntersectionPosition(0).longitude();
double min_lon = max_lon;
double max_lat = getIntersectionPosition(0).latitude();
double min_lat = max_lat;
intersections.resize(getNumIntersections());

for (int id = 0; id < getNumIntersections(); ++id){
    max_lat = std::max(max_lat, intersections[id].position.latitude());
    min_lat = std::min(min_lat, intersections[id].position.latitude());
    max_lon = std::max(max_lon, intersections[id].position.longitude());
    min_lon = std::min(min_lon, intersections[id].position.longitude());
}
}*/

void drawMap(){
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    
    ezgl::application application(settings);
    
    //ezgl::rectangle initial_background({min_lon,min_lat},{max_lon,max_lat});
    ezgl::rectangle initial_background({0,0},{1000,1000});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_background);
    
    
    application.run(nullptr,nullptr,nullptr,nullptr);
}