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
};

std::vector<street_segment_data> street_segments;
std::vector<feature_data> features;
std::vector<POI_data> pointOfInterests;
std::vector<ezgl::point2d>  fuel;
std::vector<ezgl::point2d>  school;
std::vector<ezgl::point2d>  restaurant;
std::vector<ezgl::point2d>  parking;
std::vector<ezgl::point2d>  theatre;
std::vector<ezgl::point2d>  bank;
std::vector<ezgl::point2d>  bar;
std::vector<ezgl::point2d>  cafe;
std::vector<ezgl::point2d>  fast_food;
std::vector<ezgl::point2d>   pharmacy; 
std::vector<ezgl::point2d>  hospital;
std::vector<ezgl::point2d>  post_office;
std::vector<ezgl::point2d>  dentist;


std::vector<intersection_data> intersections;

std::unordered_map <OSMID,std::string> street_types;

std::unordered_map<std::string,std::string> map_paths;

struct boundingbox bounds;

//(x, y) = (R·lon·cos(latavg), R·lat)


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

    load_bin();
    loadOSMDatabaseBIN(map_paths.find(map_load_path) -> second);
    
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
        double theta = atan2(delta_y, delta_x)* (180/3.141592653);
        if (theta > 270) theta -= 180;
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
    }
 

    //-------------------------------
   pointOfInterests.resize(getNumPointsOfInterest());
   std::vector <std::string> types;
    
    for (POIIdx poiidx = 0; poiidx < getNumPointsOfInterest(); poiidx++){
        std::string poitype = getPOIType(poiidx);
        pointOfInterests[poiidx].name = getPOIName(poiidx);
        pointOfInterests[poiidx].type = poitype;
        LatLon point = getPOIPosition(poiidx);
       ezgl::point2d coordinate = convertCoordinates(point.longitude(),point.latitude(),bounds.lat_avg);
        pointOfInterests[poiidx].coordinate = coordinate;
        //types.push_back(getPOIType(poiidx));
        
        if(poitype=="fuel"){
            fuel.push_back(coordinate);
        }else if (poitype=="theatre"||poitype=="cinema"){
            theatre.push_back(coordinate);
        }else if (poitype=="school"||poitype=="kindergarten"||poitype=="preschool"||poitype=="college"||poitype=="university"){
            school.push_back(coordinate);
        }else if (poitype=="bank"){
            bank.push_back(coordinate);
        }else if (poitype=="pharmacy"){
            pharmacy.push_back(coordinate);
        }else if (poitype=="pub"||poitype=="bar"){
            bar.push_back(coordinate);
        }else if (poitype=="restaurant"){
            restaurant.push_back(coordinate);
        }else if (poitype=="dentist"||poitype=="orthodonists"||poitype=="orthodonist"){
            dentist.push_back(coordinate);
        }else if (poitype=="hospital"){
            hospital.push_back(coordinate);
        }else if (poitype=="fast_food"){
            fast_food.push_back(coordinate);
        }else if (poitype=="post_office"){
            post_office.push_back(coordinate);
        }
    }

  
   
}

    
   



//choose when to draw highway ramps, what colour and what line width too

void drawAllStreets(ezgl::renderer *g, double zoom){


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
            if(zoom > 25107){
                g->set_line_width(26);
            }
            else if(zoom > 1171){
                g->set_line_width(20);
            }
            else if(zoom > 54){
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
            g ->set_color(213,216,219,255);
            if(zoom > 25107){
                g->set_line_width(26);
            }
            else if(zoom > 9038){
                g->set_line_width(22);
            }
            else if(zoom > 3253){
                g->set_line_width(18);
            }
            else if(zoom > 1171){
                g->set_line_width(16);
            }
            else if(zoom > 421){
                g->set_line_width(11);
            }
            else if(zoom > 151){
                g->set_line_width(6);
            }
            else if (zoom > 54){
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
                g ->set_color(213,216,219,255);
                draw = true;
            }
            if(zoom > 25107){
              g -> set_line_width(16);  
            }
            else if(zoom >9038){
              g -> set_line_width(12);  
            }
            else if(zoom > 3253){
                g->set_line_width(9);
            }
            else if(zoom > 1171){
                g->set_line_width(6);
            }
            else if(zoom > 421){
                g ->set_line_width(4);
            }
            else if(zoom > 151){
                g ->set_line_width(3);
            }
            else if (zoom > 19){
                g->set_line_width(2);
            }
            else if(zoom > 7){
                g -> set_line_width(1);
            }
        }
        else if (street_segments[i].segment_type == "residential"){
            if(zoom > 54){
                g ->set_color(213,216,219,255);
                draw = true;
            }
            if(zoom > 25107){
              g -> set_line_width(18);  
            }
            else if(zoom > 9038){
              g -> set_line_width(11);  
            }
            else if(zoom > 3253){
                g->set_line_width(9);
            }
            else if(zoom > 1171){
                g->set_line_width(6);
            }
            else if(zoom > 421){
                g ->set_line_width(3);
            }
            else if(zoom > 151){
                g ->set_line_width(2);
            }
            else if(zoom > 19){
                g -> set_line_width(1);
            }
        }
        else{
            if(zoom > 151){
            g ->set_color(213,216,219,255);
            draw = true;
            }
            if(zoom > 27351){
                g -> set_line_width(16);  
            }
            else if(zoom > 9038){
                g -> set_line_width(10);
            }
            else if(zoom > 3253){
                g->set_line_width(6); 
            } 
            else if(zoom > 1171){
                g->set_line_width(3);
            }
            else if(zoom > 421){
                g->set_line_width(2);
            }
            else if(zoom > 151){
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
        else if(features[i].feature_type == "island"&&features[i].numFeaturePoints>1){
            g ->set_color(232, 232, 232,255);
          
            g->fill_poly(features[i].coordinates);
        }else if(features[i].feature_type == "beach"&&features[i].numFeaturePoints>1){
            g ->set_color(255, 242, 175,255);
            
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

void draw_POI (ezgl::renderer *g, double zoom){
     ezgl::surface *png_surface; 
    for (int i = 0; i< getNumPointsOfInterest(); i++){
        if (pointOfInterests[i].type == "fuel"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/fillingstation.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
             
         }
        else if ((pointOfInterests[i].type == "school"||pointOfInterests[i].type == "university"||pointOfInterests[i].type == "college"||pointOfInterests[i].type == "kindergarten"||pointOfInterests[i].type == "preschool")){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/university.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }
        else if (pointOfInterests[i].type == "restaurant"){
            if((i%5 == 0)){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/restaurant.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
            }
        }else if (pointOfInterests[i].type == "parking"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/parking.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }else if (pointOfInterests[i].type == "theatre"||pointOfInterests[i].type == "cinema"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/theater.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }else if (pointOfInterests[i].type == "bank"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/bank.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }else if (pointOfInterests[i].type == "pharmacy"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/drugstore.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }else if (pointOfInterests[i].type == "bar"||pointOfInterests[i].type == "pub"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/bar.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }else if (pointOfInterests[i].type == "cafe;bakery"){
            png_surface = ezgl::renderer::load_png("libstreetmap/resources/coffee.png");
            ezgl::point2d point = pointOfInterests[i].coordinate;
            g->draw_surface(png_surface, point);
             ezgl::renderer::free_surface(png_surface);
        }
    }

}

void draw_main_canvas (ezgl::renderer *g){
    ezgl::rectangle world = g->get_visible_world();
    double area = world.area();
    double zoom = bounds.area/area;

    std::cout << bounds.area/area << std::endl;
    
    draw_features(g,zoom);
    drawAllStreets(g,zoom);       
    if(zoom>800){
        draw_POI(g,zoom);
    }
    
    //draw street names
     draw_street_names(g);
     


}


//UI function declarations for convenience
void search_entry(GtkEntry *entry);
void find_button(GtkWidget * /*widget*/ , ezgl::application *app);


void initial_setup(ezgl::application *application, bool){
    ezgl::rectangle world = application-> get_renderer()->get_visible_world();
    bounds.area = world.area();
    
    //UI stuff
    //connect search bar entry as signal
    GtkEntry *entry = (GtkEntry*) application->get_object("SearchEntry");
    g_signal_connect(entry, "activate", G_CALLBACK(search_entry), entry);
    
    //connect "Find" button as signal
    GtkWidget* findButton = (GtkWidget*) application->get_object("SearchFindButton");
    g_signal_connect(findButton, "clicked", G_CALLBACK(find_button), application);
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
void find_button(GtkWidget * /*widget*/, ezgl::application *app) {
    
    std::cout<< "Insert your streets" << std::endl;
    
    std::string street1, street2;
    
    //get street names
    std::getline(std::cin, street1);
    std::getline(std::cin, street2);
    
    //find street ids
    std::vector<StreetIdx> streetOne = findStreetIdsFromPartialStreetName(street1);
    std::vector<StreetIdx> streetTwo = findStreetIdsFromPartialStreetName(street2);
    
    //for holding intersections
    std::vector<IntersectionIdx> intersectionResults;
    
    if (streetOne.size() > 0 && streetTwo.size() > 0) {
        //first matches are used as arguments
        std::pair<StreetIdx, StreetIdx> streets (streetOne[0], streetTwo[0]);
        
        intersectionResults = findIntersectionsOfTwoStreets(streets);
        
       
    } else {
        std::cout << "No suitable streets found" << std::endl;
        return;
    }
    
    std::cout << "these are the intersections found" << std::endl;
    for (int i = 0; i < intersectionResults.size(); i++) {
        std::cout << intersectionResults[i] << " ";
    }
    
    //draw stuff from here
    app->refresh_drawing();

        
    if (intersectionResults.size() > 0) {
    for (int i = 0; i < intersectionResults.size(); i++) {
            
        std::cout << intersections[intersectionResults[i]].coordinate.x << " "
                  << intersections[intersectionResults[i]].coordinate.y << std::endl;
        
        intersections[intersectionResults[i]].highlight = true;
        }
    } else {
        std::cout << "No intersections found" << std::endl;
    }
    
    
    std::cout << std::endl;
    
    //at the end delete vectors
    intersectionResults.clear();
 
}

