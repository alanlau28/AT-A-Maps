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


std::vector<Node*> Graph;



bool operator<(const waveElement& lhs,const waveElement& rhs){
    if(lhs.traveltime >= rhs.traveltime) return true;
    else return false;
}


void loadGraph(){
    int num = getNumIntersections();
    adjacent.resize(num);
    for (int i = 0; i < num; i++){
        std::vector<StreetSegmentIdx> outgoing = findStreetSegmentsOfIntersection(i);
        //Node* currNode = new Node(i, INT_MAX, outgoing);
        Graph.push_back(new Node(i, INT_MAX, outgoing));
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


bool path(Node* source_node, IntersectionIdx destination,double turn_penalty){

    std::priority_queue<waveElement> wavefront;
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

                auto it = adjacent[currNode -> ID].find(currNode -> outgoing[i]);
                if(it != adjacent[currNode -> ID].end()){
                    double travelTime = findStreetSegmentTravelTime(it -> first)+ currNode -> time;
                    if(currNode -> leading != -1){
                        StreetSegmentInfo infoTo = getStreetSegmentInfo(currNode -> outgoing[i]);
                        StreetSegmentInfo infoFrom = getStreetSegmentInfo(currNode -> leading);
                        if(infoTo.streetID != infoFrom.streetID){ 
                            travelTime += turn_penalty;
                           
                        }
                    }
                    Node* toNode = Graph[it->second];
                    waveElement elem(toNode,it->first,travelTime);
                    wavefront.push(elem); 
                } 
            }
        } 
    }
    return false;
}

std::vector<StreetSegmentIdx> traceBack(int destination){
    std::vector<StreetSegmentIdx> finalpath;
    Node *currNode = Graph[destination];

    int reachingEdge = currNode -> leading;
    while(reachingEdge != -1){
        finalpath.push_back(reachingEdge);
        StreetSegmentInfo info = getStreetSegmentInfo(reachingEdge);
        if(info.from == currNode->ID) currNode = Graph[info.to];
        else currNode = Graph[info.from];
        reachingEdge = currNode -> leading;
    }
    std::reverse(finalpath.begin(),finalpath.end());
    return finalpath;
}

std::vector<StreetSegmentIdx> findPathBetweenIntersections(const IntersectionIdx intersect_id_start, 
    const IntersectionIdx intersect_id_destination, const double turn_penalty){
    loadGraph();
    std::vector<StreetSegmentIdx> fpath;
    Node *start = Graph[intersect_id_start];
    bool found = path(start,intersect_id_destination,turn_penalty);
    if(found){
        fpath = traceBack(intersect_id_destination);
    }
    
    while(!Graph.empty()){ 
        delete Graph.back();
        Graph.pop_back();
    }
    return fpath;

    
}


double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, 
                                const double turn_penalty){
    if(path.size()==0) return 0.0;
    
    double total_travel_time;
    StreetSegmentIdx next;
    for (int i = 0; i < path.size()-1; i++){
        next = path[i+1];
        total_travel_time += findStreetSegmentTravelTime(path[i]);
        if(getStreetSegmentInfo(path[i]).streetID != getStreetSegmentInfo(next).streetID ){
            total_travel_time += turn_penalty;
        }
    }
    total_travel_time += findStreetSegmentTravelTime(*(path.end()-1));
    
    return total_travel_time;
}

