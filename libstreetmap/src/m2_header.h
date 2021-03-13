/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2_header.h
 * Author: tanshiq1
 *
 * Created on March 12, 2021, 6:32 PM
 */

#ifndef M2_HEADER_H
#define M2_HEADER_H

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <gtk/gtk.h>


void close_map();


LatLon latLonFromWorld(double x, double y);


ezgl::point2d convertCoordinates(double longitude, double latitude, double lat_avg);


void load_bin();


void loadFeaturePriority();


void load_map();


void drawAllStreets(ezgl::renderer *g, double zoom,bool heavy);


bool checkOverlap (ezgl::renderer *g, std::vector<ezgl::rectangle> &drawn, ezgl::point2d coordinate);


void convert_point(ezgl::renderer *g, std::vector<ezgl::rectangle> &drawn, ezgl::point2d point);


void drawOneWays(ezgl::renderer *g, double zoom);


void draw_features(ezgl::renderer *g, double zoom);


void text(ezgl::renderer *g, std::string word, ezgl::color color, ezgl::point2d point);


void draw_POI (ezgl::renderer *g, ezgl::point2d small, ezgl::point2d large);


void draw_street_names (ezgl::renderer *g);


void drawHighlights(ezgl::renderer *g);


void clearHighlights();


void draw_main_canvas (ezgl::renderer *g);


void search_entry(GtkEntry* entry );


void search_entry_activate(GtkEntry* entry);


void find_button(GtkWidget * /*widget*/ , ezgl::application *app);



void initial_setup(ezgl::application *application, bool);


void act_on_mouse_click(ezgl::application* app, GdkEventButton* event,double x, double y);


void map_list(GtkListBox* box);









#endif /* M2_HEADER_H */

