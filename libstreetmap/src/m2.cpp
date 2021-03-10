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
};


struct feature_data{
    std::vector<ezgl::point2d> coordinates;
    std::string name;
    std::string feature_type;
    int numFeaturePoints;
    double area;
};

std::vector<street_segment_data> street_segments;
std::vector<feature_data> features;

std::vector<ezgl::point2d> intersection_coordinates;

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
        street_segments[street_segment_id].name = getStreetName(street_seg_info.streetID);
        
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
        street_segments[street_segment_id].angle = atan2(delta_y, delta_x)* (180/3.141592653);
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
    
    
    //Gets coordinate of each intersection
    //-------------------------------------------------
    //intersection_coordinates.resize(getNumIntersections());
    
    for (int i = 0; i < getNumIntersections(); i++) {
        LatLon point = getIntersectionPosition(i);
        ezgl::point2d coordinate = convertCoordinates(point.longitude(), point.latitude(), bounds.lat_avg);
        intersection_coordinates.push_back(coordinate);
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


void draw_features(ezgl::renderer *g, double zoom){
    for (int i = 0; i<features.size();i++){
        if (features[i].feature_type == "park"&&features[i].numFeaturePoints>1){
            g ->set_color(195, 236, 178,255);
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
            g ->set_color(181, 219, 167,255);
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
        else if(features[i].feature_type == "beach"&&features[i].numFeaturePoints>1){
            g ->set_color(255, 242, 175,255);
            
            g->fill_poly(features[i].coordinates);
        }else if(features[i].feature_type == "island"&&features[i].numFeaturePoints>1){
            g ->set_color(232, 232, 232,255);
          
            g->fill_poly(features[i].coordinates);
        }else if(features[i].feature_type == "building"&&features[i].numFeaturePoints>1&&zoom>1500){
            g ->set_color(213, 216, 219,255);
          
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

void draw_street_names (ezgl::renderer *g) {
    for (int i = 0; i < getNumStreetSegments(); i++) {
        
        double x = (street_segments[i].coordinates[1].x + street_segments[i].coordinates[0].x)/2;
        double y = (street_segments[i].coordinates[1].y + street_segments[i].coordinates[0].y)/2;
        
        g->set_color(0,0,0);
        g->set_font_size(15);
        if (street_segments[i].name != "<unknown>") {
            g->set_text_rotation(street_segments[i].angle);
            g->draw_text({x, y}, street_segments[i].name, 100.0, 100.0);
        }
        
    }
}

void draw_main_canvas (ezgl::renderer *g){
    ezgl::rectangle world = g->get_visible_world();
    double area = world.area();
    double zoom = bounds.area/area;
   // std::cout << bounds.area/area << std::endl;
    
    draw_features(g,zoom);
   
    
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
    
    
    //draw street names
     draw_street_names(g);
//  
    
}


//UI function declarations for convenience
void search_entry(GtkEntry *entry);
void find_button(ezgl::renderer *g);


void initial_setup(ezgl::application *application, bool){
    ezgl::rectangle world = application-> get_renderer()->get_visible_world();
    bounds.area = world.area();
    
    //UI stuff
    //connect search bar entry as signal
    GtkEntry *entry = (GtkEntry*) application->get_object("SearchEntry");
    g_signal_connect(entry, "activate", G_CALLBACK(search_entry), entry);
    
    //connect "Find" button as signal
    GtkWidget* findButton = (GtkWidget*) application->get_object("SearchFindButton");
    g_signal_connect(findButton, "clicked", G_CALLBACK(find_button), application->get_renderer());
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
    
    //connect search bar
    
    
    closeOSMDatabase();
    
}


//UI stuff from here


//searchEntry
void search_entry(GtkEntry *entry) {
    
    // Get the text written in the widget
    const char* text = gtk_entry_get_text(entry);
    
    std::cout << text << std::endl;
    
    //after that clear the entry
    
    gtk_entry_set_text (entry, "");
    
}

//Find Button - right now works through command line
void find_button(ezgl::renderer *g) {
    
    std::cout<< "Insert your streets" << std::endl;
    
    std::string street1, street2;
    
    //get street names
    std::getline(std::cin, street1);
    std::getline(std::cin, street2);
    
    //find street ids
    std::vector<StreetIdx> streetOne = findStreetIdsFromPartialStreetName(street1);
    std::vector<StreetIdx> streetTwo = findStreetIdsFromPartialStreetName(street2);
    
    //for holding intersections
    std::vector<IntersectionIdx> intersections;
    
    if (streetOne.size() > 0 && streetTwo.size() > 0) {
        //first matches are used as arguments
        std::pair<StreetIdx, StreetIdx> streets (streetOne[0], streetTwo[0]);
        
        intersections = findIntersectionsOfTwoStreets(streets);
        
       
    } else {
        std::cout << "No suitable streets found" << std::endl;
        return;
    }
        
    if (intersections.size() > 0) {
//    for (int i = 0; i < intersection_coordinates.size(); i++) {
//            float width = 25;
//            float height = 25;
//            g->set_color(200, 50, 10);
//            ezgl::point2d coordinate(intersection_coordinates[i].x - width/2, intersection_coordinates[i].y - height/2);
//            g->fill_rectangle(coordinate, height, width);
//           
//        }
    } else {
        std::cout << "No intersections found" << std::endl;
    }
    
    

    
    //for checking that results are good
    for (int i = 0; i < intersections.size(); i++) {
        std::cout << intersection_coordinates[intersections[i]].x << " " 
                  << intersection_coordinates[intersections[i]].y << " " << std::endl;
        
        std::cout << intersections[i] << " " ;
    }
    std::cout << std::endl;
    
    //at the end delete vectors
    intersections.clear();
 
}

