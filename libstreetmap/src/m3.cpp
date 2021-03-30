/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m3.h"
#include "m3_header.h"
#include "m2_header.h"
#include "m1.h"
#include <iostream>
#include <climits>
#include <unordered_map>
#include <queue>
#include <algorithm>

std::vector<std::unordered_map<StreetSegmentIdx,IntersectionIdx>> adjacent;

class Node{
public:
    IntersectionIdx ID;
    StreetSegmentIdx leading;
    double time;
    std::vector<StreetSegmentIdx> outgoing;
    Node* prev = nullptr;
    Node(int id,double t, std::vector<StreetSegmentIdx> out);      
    
};

Node::Node(int id, double t, std::vector<StreetSegmentIdx> out){
    ID = id;
    leading = -1;
    time = t;
    outgoing = out;
}

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

std::vector<Node> Graph;
std::priority_queue<waveElement> wavefront;

bool operator<(const waveElement& lhs,const waveElement& rhs){
        if(lhs.traveltime >= rhs.traveltime) return true;
        else return false;
}

void loadGraph(){
    int num = getNumIntersections();
    for (int i = 0; i < num; i++){
        std::vector<StreetSegmentIdx> outgoing = findStreetSegmentsOfIntersection(i);
        Node currNode(i, INT_MAX, outgoing);
        Graph.push_back(currNode);
    }
    num = getNumStreetSegments();
    adjacent.resize(num);
    for (int i = 0; i < num; i++){
        StreetSegmentInfo street_seg_info = getStreetSegmentInfo(i);
        if(street_seg_info.oneWay){
            adjacent[i].insert(std::make_pair(i, street_seg_info.to));
            }
        else{
            adjacent[i].insert(std::make_pair(i, street_seg_info.from));
            adjacent[i].insert(std::make_pair(i, street_seg_info.to));
        }
    }
}



/*for all intersections:
 *    for i in findStreetSegmentsOfIntersection(intersection)
 *    
 */


/*
 initialize(graph, source)
 *   source.time = 0
 *   source.leading= null
 *   for otherNodes in Graph:
 *        otherNodes.time = INT_MAX;
 *        otherNodes.leading = null;
 
 */

/*
 * relax(start, end, edge)
 *    if (end.time) > (start.time) + edge
 *        end.time = (start.time) + edge
 *        end.leading = edge
 */

bool path(Node* source_node, IntersectionIdx destination){
    waveElement source(source_node,-1, 0);
    wavefront.push(source);
    while(!wavefront.empty()){
        waveElement wave = wavefront.top();
        wavefront.pop();
        Node *currNode = wave.node;
        
        if(wave.traveltime < currNode -> time){
            currNode -> leading = wave.edgeID;
            currNode -> time = wave.traveltime;
        
            if(currNode -> ID == destination){
                return true;
            }

            for(int i = 0; i < currNode -> outgoing.size(); i++){
                auto it = adjacent[currNode -> outgoing[i]].find(currNode -> outgoing[i]);
                while(it != adjacent[i].end()){
                    Node* toNode = &Graph[it -> second];
                    toNode -> prev = currNode;
                    waveElement elem(toNode,it->first,findStreetSegmentTravelTime(it -> first));
                    wavefront.push(elem);
                    it++; 
                } 
            }
        } 
    }
    return false;
}

std::vector<StreetSegmentIdx> traceBack(int destination){
    std::vector<StreetSegmentIdx> finalpath;
    Node *currNode = &Graph[destination];
    int prev = currNode -> leading;
    while(prev != -1){
        finalpath.push_back(prev);
        currNode = currNode -> prev;
        prev = currNode -> leading;
    }
    std::reverse(finalpath.begin(),finalpath.end());
    return finalpath;
}

std::vector<StreetSegmentIdx> findPathBetweenIntersections(const IntersectionIdx intersect_id_start, 
    const IntersectionIdx intersect_id_destination, const double turn_penalty){
    loadGraph();
    std::vector<StreetSegmentIdx> fpath;
    Node *start = &Graph[intersect_id_start];
    bool found = path(start,intersect_id_destination);
    if(found){
        return traceBack(intersect_id_destination);
    }
    //c
    return fpath;
    
    
}