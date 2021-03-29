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

extern std::vector<std::unordered_map<StreetSegmentIdx,IntersectionIdx>> adjacent;


void close_map();


LatLon latLonFromWorld(double x, double y);


ezgl::point2d convertCoordinates(double longitude, double latitude);


void load_bin();


void loadFeaturePriority();


void load_map();


void drawAllStreets(ezgl::renderer *g, double zoom,bool heavy);

/**
 * Provides functions to check for overlapping of points. 
 * 
 * Given a vector of bounding boxes, check if the new point is within any boxes.
 * 
 * @param *g: ezgl renderer
 * @param drawn: vector of bounding boxes
 * @param coordinate: coordinate of point in question, in point2d
 */
bool checkOverlap (ezgl::renderer *g, std::vector<ezgl::rectangle> &drawn, ezgl::point2d coordinate);

/**
 * Provides functions convert point to screen coordinate and add bounding box to vector.
 * 
 * Apply enough offset to define bounding box
 * 
 * @param *g: ezgl renderer
 * @param drawn: vector of bounding boxes
 * @param coordinate: coordinate of point in question, in point2d
 */
void convert_point(ezgl::renderer *g, std::vector<ezgl::rectangle> &drawn, ezgl::point2d point);



void drawOneWays(ezgl::renderer *g, double zoom,bool heavy);



void draw_features(ezgl::renderer *g, double zoom);

/**
 * Provides functions to draw text at certain location with given color.
 * 
 * @param *g: ezgl renderer
  * @param word: text
 * @param color: color of text
 * @param point: coordinate of text, in point2d
 */
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

void reveal_search_bar();

void reveal_search_activate(GtkEntry* entry);

void reveal_search_entry(GtkEntry* entry);


void display_path(const std::vector<StreetIdx> path);







#endif /* M2_HEADER_H */

