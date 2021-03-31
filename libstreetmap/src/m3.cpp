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
        for(int j = 0;j < outgoing.size();j++){
            StreetSegmentInfo street_seg_info = getStreetSegmentInfo(outgoing[j]);
            if(i != street_seg_info.to){
                adjacent[i].insert(std::make_pair(outgoing[j], street_seg_info.to));
            }
            else if(!street_seg_info.oneWay){
                adjacent[i].insert(std::make_pair(outgoing[j], street_seg_info.from));
            }
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
                    Node* toNode = Graph[it->second];
                    waveElement elem(toNode,it->first,findStreetSegmentTravelTime(it -> first)+currNode -> time);
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
    bool found = path(start,intersect_id_destination);
    if(found){
        fpath = traceBack(intersect_id_destination);
    }
    Graph.clear();
    return fpath;
    
    
}