/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m3.h"
#include "m1.h"
#include <iostream>
#include <climits>
#include <unordered_map>
#include <queue>


class Node{
private:
    IntersectionIdx ID;
    StreetSegmentIdx leading;
    double time;
    std::vector<StreetSegmentIdx> outgoing;
public:   
    Node(int id,double t, std::vector<StreetSegmentIdx> out);      
    
};

Node::Node(int id, double t, std::vector<StreetSegmentIdx> out){
    ID = id;
    leading = 0;
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
    
    bool operator<(waveElement& rhs){
        if(traveltime >= rhs.traveltime) return true;
        else return false;
    }
};

std::vector<Node> Graph;
std::priority_queue<waveElement> wavefront;

void loadGraph(){
    int num = getNumIntersections();
    for (int i = 0; i < num; i++){
        std::vector<StreetSegmentIdx> outgoing = findStreetSegmentsOfIntersection(i);
        Node currNode(i, INT_MAX, outgoing);
        Graph.push_back(currNode);
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

std::vector<StreetSegmentIdx> findPathBetweenIntersections(const IntersectionIdx intersect_id_start, 
        const IntersectionIdx intersect_id_destination, const double turn_penalty){
    std::vector<int> q;
    return q;
    
    
}