/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3_header.h
 * Author: tanshiq1
 *
 * Created on March 29, 2021, 3:46 PM
 */

#ifndef M3_HEADER_H
#define M3_HEADER_H
#include <unordered_map>
#include <vector>
#include "StreetsDatabaseAPI.h"

const int NOEDGE = -1; 

//Node is used for the graph, each node represents an intersection
class Node{
public:
    IntersectionIdx ID;
    StreetSegmentIdx leading; //intersection used to reach this node
    double time; //total traveltime to node
    std::vector<StreetSegmentIdx> outgoing; //outgoing street segments of each intersection
    Node(int id,double t,const std::vector<StreetSegmentIdx> &out); 
    
};



struct waveElement{
    Node *node;
    StreetSegmentIdx edgeID;
    double traveltime;
    
    waveElement(Node *n, int id, double time){
        node = n;
        edgeID = id;
        traveltime = time;
    }

};
void loadGraph();

extern double max_speed; //max speed of the map

extern std::vector<StreetSegmentInfo> segmentInfo;

extern std::vector<std::pair<double,double>> intersectionPosition;

double findEuclidianDistance(std::pair<double,double> first,std::pair<double,double> second);

IntersectionIdx findOtherIntersection(IntersectionIdx start, StreetSegmentIdx outgoing);

bool operator<(const waveElement& lhs,const waveElement& rhs);

std::vector<StreetSegmentIdx> traceBack(int destination);

bool path(Node* source_node, IntersectionIdx destination, double turn_penalty);

std::vector<std::vector<StreetSegmentIdx>> findPath(Node* source_node, std::vector<IntersectionIdx>& intersections_dest, double turn_penalty);

std::vector<std::vector<std::vector<StreetSegmentIdx>>> findAllPaths(std::vector<IntersectionIdx>& intersections_dest,const double turn_penalty);
#endif /* M3_HEADER_H */

