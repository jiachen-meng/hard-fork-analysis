//
//  dijkstra.h
//  fork_dijkstra
//
//  Created by Jiachen Meng on 2020-12-18.
//  Copyright © 2020 Jiachen Meng. All rights reserved.
//

#ifndef dijkstra_h
#define dijkstra_h

#include <stdio.h>
#include <vector>
#include <queue>
#include <utility>
#include <functional>

//an implementation of Dijkstra's Algorithm with min-heap priority queue
//need to model other given data structures into this Graph

class myComparator
{
public:
    int operator() (const std::pair<int, int> p1, const std::pair<int, int> p2)
    {
        return p1.first > p2.first;
    }
};

class Graph{
private:
    int n;
    std::vector<std::vector<std::pair<int, int>>> adj;
    
public:
    Graph (int size);
    void add_edge_undirected(int u, int v, int w);
    void add_edge_directed(int u, int v, int w);
    void shortest_path(int s, std::unordered_map<int, std::string> id_to_commit, std::vector<std::string> latest_commits_id, std::ofstream &dijkstra_commits);
};

Graph::Graph(int size){
    this->n = size;
    adj.resize(size);
}

void Graph::add_edge_undirected(int u, int v, int w){
    adj[u].push_back(std::make_pair(v, w));
    adj[v].push_back(std::make_pair(u, w));
}

void Graph::add_edge_directed(int u, int v, int w){
    adj[u].push_back(std::make_pair(v, w));
}

void Graph::shortest_path(int s, std::unordered_map<int, std::string> id_to_commit, std::vector<std::string> latest_commits_id, std::ofstream &dijkstra_commits){
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, myComparator> pq;
    std::vector<int> distance(this->n, INT_MAX);
    
    pq.push(std::make_pair(0, s));
    distance[s] = 0;
    
    while(!pq.empty()){
        int u = pq.top().second;
        pq.pop();
        
        for (int i = 0; i < adj[u].size(); i++){
            int v = adj[u][i].first;
            int cost = adj[u][i].second;
            
            if (distance[v] > distance[u]+cost){
                distance[v] = distance[u]+cost;
                pq.push(std::make_pair(distance[v], v));
            }
        }
    }
    
    printf("\nVertex Commit ID  \t\t\t\t\t\t Distance from Source\n");
    for (int i = 0; i < id_to_commit.size(); ++i)
        //printf("%s \t\t %d\n", id_to_commit.find(i)->second, distance[i]);
        if (std::find(latest_commits_id.begin(), latest_commits_id.end(), id_to_commit.find(i)->second) != latest_commits_id.end()){
            if (distance[i] == INT_MAX){
                std::cout<<id_to_commit.find(i)->second<<"\t\t"<<"INFINITY"<<"\n";
            }
            else{
                std::cout<<id_to_commit.find(i)->second<<"\t\t"<<distance[i]<<"\n";
            }
        }
        
    dijkstra_commits<<id_to_commit.find(s)->second<<",";
    for (int i = 0; i < id_to_commit.size(); ++i)
    //printf("%s \t\t %d\n", id_to_commit.find(i)->second, distance[i]);
    if (std::find(latest_commits_id.begin(), latest_commits_id.end(), id_to_commit.find(i)->second) != latest_commits_id.end()){
        if (distance[i] == INT_MAX){
            dijkstra_commits<<"INFINITY"<<",";
        }
        else{
            dijkstra_commits<<distance[i]<<",";
        }
    }
    dijkstra_commits<<"\n";
}




#endif /* dijkstra_h */
