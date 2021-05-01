//
//  main.cpp
//  fork_dijkstra
//
//  Created by Jiachen Meng on 2020-12-18.
//  Copyright © 2020 Jiachen Meng. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unordered_map>
#include <map>
#include <time.h>
#include "git2.h"
#include "dijkstra.h"

#define INTERVAL_NUMBER 10

int walk_through_repo(const char * REPO_PATH, int& current_id, std::unordered_map<std::string, int>& commit_to_id, std::unordered_map<int, std::string>& id_to_commit, std::unordered_map<std::string, std::string>& committer){
    // init libgit
    git_libgit2_init();

    git_repository * repo = nullptr;
    git_repository_open(&repo, REPO_PATH);
    git_branch_iterator * branch = nullptr;
    git_branch_iterator_new(&branch, repo, GIT_BRANCH_LOCAL);
    git_reference *ref = nullptr;
    git_branch_t type;
    const char *branch_name = NULL;
    const char *branch_full_name = NULL;
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    //iterate all local branches
    while (!git_branch_next(&ref, &type, branch)){
        git_branch_name(&branch_name, ref);
        std::cout<<branch_name<<std::endl;
        branch_full_name = git_reference_name(ref);
        git_revparse_single(&treeish, repo, branch_name);
        git_checkout_tree(repo, treeish, &opts);
        git_repository_set_head(repo, branch_full_name);
        
        
        // -- create revision walker --
        git_revwalk * walker = nullptr;
        git_revwalk_new(&walker, repo);
        
        // sort log by chronological order
        git_revwalk_sorting(walker, GIT_SORT_NONE);
        
        /* we can choose one from below as the sorting pattern
         
         GIT_SORT_NONE – default reverse chronological order (starts from most recent) of commits as in git
         GIT_SORT_TOPOLOGICAL – topological order, shows no parent before all of its children are shown
         GIT_SORT_TIME – commit timestamp order
         GIT_SORT_REVERSE – commits in reverse order
         */
        
        // start from HEAD
        git_revwalk_push_head(walker);
        
        // -- walk the walk --
        git_oid oid;
        
        while(!git_revwalk_next(&oid, walker))
        {
            // -- get the current commit --
            git_commit * commit = nullptr;
            git_commit_lookup(&commit, repo, &oid);
            
            //insert committer info
            const git_signature* commit_signature = git_commit_author(commit);
            std::string email(commit_signature->email);
            if (committer.find(email) == committer.end()){
                committer.insert(std::make_pair(commit_signature->email, commit_signature->name));
            }
            
            //insert commit and id into unordered map
            std::string commit_id (git_oid_tostr_s(&oid));
            if (commit_to_id.find(commit_id) == commit_to_id.end()){
                std::cout<<commit_id<<std::endl;
                commit_to_id.insert(std::make_pair(commit_id, current_id));
                id_to_commit.insert(std::make_pair(current_id, commit_id));
                current_id++;
            }
            
            // free the commit
            git_commit_free(commit);
            
        }
        git_revwalk_free(walker);
    }
    
    // -- clean up --
    git_reference_free(ref);
    git_object_free(treeish);
    git_branch_iterator_free(branch);
    git_repository_free(repo);

    git_libgit2_shutdown();

    return 0;
}

void common_committer_commit(const char * REPO_PATH, std::string this_email, std::string this_name, std::multimap<time_t, std::pair<std::string, std::string>>& foo, std::ofstream &detail_commits, std::string url){
    //std::unordered_map<std::string, int> foo;
    // init libgit
    git_libgit2_init();

    git_repository * repo = nullptr;
    git_repository_open(&repo, REPO_PATH);
    git_branch_iterator * branch = nullptr;
    git_branch_iterator_new(&branch, repo, GIT_BRANCH_LOCAL);
    git_reference *ref = nullptr;
    git_branch_t type;
    const char *branch_name = NULL;
    const char *branch_full_name = NULL;
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    //detail_commits.open("detail_commits_result.csv");
    
    //iterate all local branches
    while (!git_branch_next(&ref, &type, branch)){
        git_branch_name(&branch_name, ref);
        //std::cout<<branch_name<<std::endl;
        branch_full_name = git_reference_name(ref);
        git_revparse_single(&treeish, repo, branch_name);
        git_checkout_tree(repo, treeish, &opts);
        git_repository_set_head(repo, branch_full_name);
        
        
        // -- create revision walker --
        git_revwalk * walker = nullptr;
        git_revwalk_new(&walker, repo);
        
        // sort log by chronological order
        git_revwalk_sorting(walker, GIT_SORT_NONE);
        
        /* we can choose one from below as the sorting pattern
         
         GIT_SORT_NONE – default reverse chronological order (starts from most recent) of commits as in git
         GIT_SORT_TOPOLOGICAL – topological order, shows no parent before all of its children are shown
         GIT_SORT_TIME – commit timestamp order
         GIT_SORT_REVERSE – commits in reverse order
         */
        
        // start from HEAD
        git_revwalk_push_head(walker);
        
        // -- walk the walk --
        git_oid oid;
        
        while(!git_revwalk_next(&oid, walker))
        {
            // -- get the current commit --
            git_commit * commit = nullptr;
            git_commit_lookup(&commit, repo, &oid);
            
            //insert committer info
            const git_signature* commit_signature = git_commit_author(commit);
            std::string email(commit_signature->email);
            if (email == this_email){
                //struct tm * utc;
                std::time_t t = (long)commit_signature->when.time;
                //utc = gmtime(&t);
                
                /*
                std::cout<<commit_signature->name<<"\t\t\t"<<REPO_PATH<<"\t\t\t"<<ctime(&t)<<"\t\t\t"<<git_oid_tostr_s(&oid)<<"\n";
                detail_commits.open("detail_commits_result.csv");
                
                detail_commits<<std::string (commit_signature->name)<<","<<REPO_PATH<<","<<std::string (ctime(&t))<<","<<git_oid_tostr_s(&oid)<<"\n";
                */
                
                std::string roo = (ctime(&t));
                roo.pop_back();
                
                std::string joo =std::string (commit_signature->name) + "," + url + "," + roo + "," + git_oid_tostr_s(&oid) + "\n";
                detail_commits<<joo;
                foo.insert(std::make_pair(t, std::make_pair(commit_signature->name, url)));
            }
            
            // free the commit
            git_commit_free(commit);
            
        }
        git_revwalk_free(walker);
    }
    
    // -- clean up --
    git_reference_free(ref);
    git_object_free(treeish);
    git_branch_iterator_free(branch);
    git_repository_free(repo);

    git_libgit2_shutdown();

}

std::unordered_map<std::string, int> walk_through_repo_first_parent(const char * REPO_PATH){
    std::unordered_map<std::string, int> result;
    // init libgit
    git_libgit2_init();

    git_repository * repo = nullptr;
    git_repository_open(&repo, REPO_PATH);

    // -- create revision walker --
    git_revwalk * walker = nullptr;
    git_revwalk_new(&walker, repo);
    git_revwalk_simplify_first_parent(walker);

    // sort log by chronological order
    git_revwalk_sorting(walker, GIT_SORT_NONE);
    
    /* we can choose one from below as the sorting pattern
     
    GIT_SORT_NONE – default reverse chronological order (starts from most recent) of commits as in git
    GIT_SORT_TOPOLOGICAL – topological order, shows no parent before all of its children are shown
    GIT_SORT_TIME – commit timestamp order
    GIT_SORT_REVERSE – commits in reverse order
     */

    // start from HEAD
    git_revwalk_push_head(walker);

    // -- walk the walk --
    git_oid oid;

    while(!git_revwalk_next(&oid, walker))
    {
        // -- get the current commit --
        git_commit * commit = nullptr;
        git_commit_lookup(&commit, repo, &oid);

        //insert commit and id into unordered map
        std::string commit_id (git_oid_tostr_s(&oid));
        result.insert(std::make_pair(commit_id, 0));

        // free the commit
        git_commit_free(commit);
        
    }
    
    // -- clean up --
    git_revwalk_free(walker);
    git_repository_free(repo);

    git_libgit2_shutdown();
    
    return result;
}

void latest_commit(const char * REPO_PATH, int current_id, std::unordered_map<std::string, int> commit_to_id, std::unordered_map<int, std::string> id_to_commit, std::vector<std::string>& latest_commits_id){
    // init libgit
    git_libgit2_init();
    std::string commit_id;

    git_repository * repo = nullptr;
    git_repository_open(&repo, REPO_PATH);
    git_branch_iterator * branch = nullptr;
    git_branch_iterator_new(&branch, repo, GIT_BRANCH_LOCAL);
    git_reference *ref = nullptr;
    git_branch_t type;
    const char *branch_name = NULL;
    const char *branch_full_name = NULL;
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    //iterate all local branches
    while (!git_branch_next(&ref, &type, branch)){
        git_branch_name(&branch_name, ref);
        std::cout<<branch_name<<std::endl;
        branch_full_name = git_reference_name(ref);
        git_revparse_single(&treeish, repo, branch_name);
        git_checkout_tree(repo, treeish, &opts);
        git_repository_set_head(repo, branch_full_name);
        
        // -- create revision walker --
        git_revwalk * walker = nullptr;
        git_revwalk_new(&walker, repo);
        
        // sort log by chronological order
        git_revwalk_sorting(walker, GIT_SORT_NONE);
        
        /* we can choose one from below as the sorting pattern
         
         GIT_SORT_NONE – default reverse chronological order (starts from most recent) of commits as in git
         GIT_SORT_TOPOLOGICAL – topological order, shows no parent before all of its children are shown
         GIT_SORT_TIME – commit timestamp order
         GIT_SORT_REVERSE – commits in reverse order
         */
        
        // start from HEAD
        git_revwalk_push_head(walker);
        
        // -- this would be the most recent commit --
        git_oid oid;
        /*
         //only one commit in repo
         if (git_revwalk_next(&oid, walker) != 0){
         commit_id = git_oid_tostr_s(&oid);
         // -- clean up --
         git_revwalk_free(walker);
         git_repository_free(repo);
         
         git_libgit2_shutdown();
         return commit_id;
         }*/
        
        git_revwalk_next(&oid, walker);
        git_commit * commit = nullptr;
        git_commit_lookup(&commit, repo, &oid);
        latest_commits_id.push_back(git_oid_tostr_s(&oid));
        
        git_commit_free(commit);
        git_revwalk_free(walker);
    }
    
    // -- clean up --
    git_reference_free(ref);
    git_object_free(treeish);
    git_branch_iterator_free(branch);
    git_repository_free(repo);
    git_libgit2_shutdown();
}

int create_edges_from_parents(const char * REPO_PATH, Graph& g, std::unordered_map<std::string, int>& commit_to_id){
    
    //load all first parent commits in this branch
    std::unordered_map<std::string, int> result;
    // init libgit
    git_libgit2_init();

    git_repository * repo = nullptr;
    git_repository_open(&repo, REPO_PATH);
    
    git_branch_iterator * branch = nullptr;
    git_branch_iterator_new(&branch, repo, GIT_BRANCH_LOCAL);
    git_reference *ref = nullptr;
    git_branch_t type;
    const char *branch_name = NULL;
    const char *branch_full_name = NULL;
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    //iterate all local branches
    while (!git_branch_next(&ref, &type, branch)){
        git_branch_name(&branch_name, ref);
        std::cout<<branch_name<<std::endl;
        branch_full_name = git_reference_name(ref);
        git_revparse_single(&treeish, repo, branch_name);
        git_checkout_tree(repo, treeish, &opts);
        git_repository_set_head(repo, branch_full_name);
        
        // -- create revision walker --
        git_revwalk * walker = nullptr;
        git_revwalk_new(&walker, repo);
        git_revwalk_simplify_first_parent(walker);
        
        // sort log by chronological order
        git_revwalk_sorting(walker, GIT_SORT_NONE);
        
        /* we can choose one from below as the sorting pattern
         
         GIT_SORT_NONE – default reverse chronological order (starts from most recent) of commits as in git
         GIT_SORT_TOPOLOGICAL – topological order, shows no parent before all of its children are shown
         GIT_SORT_TIME – commit timestamp order
         GIT_SORT_REVERSE – commits in reverse order
         */
        
        // start from HEAD
        git_revwalk_push_head(walker);
        
        // -- walk the walk --
        git_oid oid;
        
        while(!git_revwalk_next(&oid, walker))
        {
            // -- get the current commit --
            git_commit * commit = nullptr;
            git_commit_lookup(&commit, repo, &oid);
            
            //insert commit and id into unordered map
            std::string commit_id (git_oid_tostr_s(&oid));
            result.insert(std::make_pair(commit_id, 0));
            
            // free the commit
            git_commit_free(commit);
            
        }
        
        // -- clean up --
        git_revwalk_free(walker);
        
        //__________________________________________________________________________
        
        //now starts to create edges
        
        // -- create revision walker --
        walker = nullptr;
        git_revwalk_new(&walker, repo);
        git_revwalk_simplify_first_parent(walker);
        
        // sort log by chronological order
        git_revwalk_sorting(walker, GIT_SORT_NONE);
        
        /* we can choose one from below as the sorting pattern
         
         GIT_SORT_NONE – default reverse chronological order (starts from most recent) of commits as in git
         GIT_SORT_TOPOLOGICAL – topological order, shows no parent before all of its children are shown
         GIT_SORT_TIME – commit timestamp order
         GIT_SORT_REVERSE – commits in reverse order
         */
        
        // start from HEAD
        git_revwalk_push_head(walker);
        // -- walk the walk --
        
        while(!git_revwalk_next(&oid, walker))
        {
            // -- get the current commit --
            git_commit * commit = nullptr;
            git_commit_lookup(&commit, repo, &oid);
            
            std::string commit_id (git_oid_tostr_s(&oid));
            std::cout<<commit_id<<std::endl;
            auto commit_itr = commit_to_id.find(commit_id);
            if (commit_itr == commit_to_id.end()){
                std::cout<<"commit not found in graph\n";
                return -1;
            }
            int commit_in_graph = commit_itr->second;
            
            // check parent number
            int parent_number = git_commit_parentcount(commit);
            git_commit * parent = nullptr;
            //create edges from each parent to commit
            for (int i = 0; i < parent_number; i++){
                git_commit_parent(&parent, commit, i);
                const git_oid* parent_oid = git_commit_id(parent);
                std::string parent_id (git_oid_tostr_s(parent_oid));
                std::cout<<"\t\t"<<parent_id<<std::endl;
                auto parent_itr = commit_to_id.find(parent_id);
                
                if (parent_itr == commit_to_id.end()){
                    std::cout<<"parent not found in graph\n";
                    return -1;
                }
                
                int parent_in_graph = parent_itr->second;
                
                //check if parent is in the same repo
                //git_commit* foo = nullptr;
                //not in the same branch, weight = 1
                if (result.find(parent_id) == result.end()){
                    g.add_edge_directed(parent_in_graph, commit_in_graph, 1);
                    std::cout<<"haha\n";
                }
                
                //in the same branch, weight = 0
                else{
                    g.add_edge_directed(parent_in_graph, commit_in_graph, 0);
                }
                
                git_commit_free(parent);
            }
            
            
            // free the commit
            git_commit_free(commit);
        }
        git_revwalk_free(walker);
    }
    
    // -- clean up --
    git_reference_free(ref);
    git_object_free(treeish);
    git_branch_iterator_free(branch);
    git_repository_free(repo);
    git_libgit2_shutdown();

    return 0;
}

int main(int argc, const char * argv[]) {
    //system("cd ~/Documents/simple_fork && git log");
    std::string upstream, fork, upstream_url;
    std::vector <std::string> fork_url;
    //***need to clone the repos manually before running the program***//
    
    //prepare a csv file
    std::ofstream time_window;
    std::ofstream detail_commits;
    std::ofstream dijkstra_commits;
    time_window.open ("time_window_result.csv");
    detail_commits.open("detail_commits_result.csv");
    dijkstra_commits.open("dijkstra_commits_result.csv");
    
    std::cout<<"Please provide the Github URL of upstream repo.\nFor example, https://github.com/pytorch/pytorch\n";
    std::cin>>upstream_url;
    std::string foo = "cd ~/Documents/repo_fetch && git clone ";
    foo += upstream_url;
    foo += ".git upstream";
    const char* command = foo.c_str();
    system(command);
    
    foo = "cd ~/Documents/repo_fetch/upstream && for remote in `git branch -r | grep -v /HEAD`; do git checkout --track $remote ; done";
    command = foo.c_str();
    system(command);
    
    bool end_of_fork = false;
    int number_of_fork = 0;
    while (!end_of_fork){
        std::string tmp;
        std::cout<<"Please provide the Github URL of fork repo.\nFor example, https://github.com/dummyuser/pytorch\nWhen finish all forks, enter \"end\"\n";
        std::cin>>tmp;
        if (tmp == "end"){
            end_of_fork = true;
            break;
        }
        fork_url.push_back(tmp);
        number_of_fork++;
        tmp = "cd ~/Documents/repo_fetch && git clone " + tmp;
        tmp += ".git fork";
        tmp += std::to_string(number_of_fork);
        const char* command_tmp = tmp.c_str();
        system(command_tmp);
        
        tmp = "cd ~/Documents/repo_fetch/fork";
        tmp += std::to_string(number_of_fork);
        tmp += " && for remote in `git branch -r | grep -v /HEAD`; do git checkout --track $remote ; done";
        command_tmp = tmp.c_str();
        system(command_tmp);
    }
    
    //std::cout<<"Please provide the local path to upstream repo.\nFor example, /path/to/local/pytorch/pytorch\n";
    //std::cin>>upstream;
    //std::cout<<"Please provide the local path to fork repo.\nFor example, /path/to/local/dummyuser/pytorch\n";
    //std::cin>>fork;
    //upstream += ".git";
    //fork += ".git";
    std::string upstream_local_string = "/Users/jiachenmeng/Documents/repo_fetch/upstream";
    std::string fork_local_string = "/Users/jiachenmeng/Documents/repo_fetch/fork";
    
    auto upstream_local = upstream_local_string.c_str();
    
    std::unordered_map<std::string, int> commit_to_id;
    std::unordered_map<int, std::string> id_to_commit;
    std::vector<std::string> latest_commits_id;
    
    //email as key, name as data
    std::unordered_map<std::string, std::string> upstream_committer;
    std::unordered_map<std::string, std::string> fork_committer;
    
    int current_id = 0;
    
    walk_through_repo(upstream_local, current_id, commit_to_id, id_to_commit, upstream_committer);
    for (int i = 1; i <= number_of_fork; i++){
        std::string foo = fork_local_string + std::to_string(i);
        auto fork_local = foo.c_str();
        walk_through_repo(fork_local, current_id, commit_to_id, id_to_commit, fork_committer);
    }
    
    int V = (int)commit_to_id.size();
    Graph g(V);
    
    create_edges_from_parents(upstream_local, g, commit_to_id);
    for (int i = 1; i <= number_of_fork; i++){
        std::string foo = fork_local_string + std::to_string(i);
        auto fork_local = foo.c_str();
        create_edges_from_parents(fork_local, g, commit_to_id);
    }
    
    latest_commit(upstream_local, current_id, commit_to_id, id_to_commit, latest_commits_id);
    for (int i = 1; i <= number_of_fork; i++){
        std::string foo = fork_local_string + std::to_string(i);
        auto fork_local = foo.c_str();
        latest_commit(fork_local, current_id, commit_to_id, id_to_commit, latest_commits_id);
    }
    
    
    std::string target_commit;
    /*
    std::cout<<"Please provide the target source commit id.\nFor example, 9f0cb4b09857571fcb698917c777dd0a8837c4c1\n";
    std::cin>>target_commit;
    
    auto target_itr = commit_to_id.find(target_commit);
    if (target_itr == commit_to_id.end()){
        std::cout<<"target source commit not found in graph\n";
        return -1;
    }
    int target_in_graph = target_itr->second;
    */
    
    dijkstra_commits<<",";
    for (int i = 0; i < id_to_commit.size(); ++i){
        if (std::find(latest_commits_id.begin(), latest_commits_id.end(), id_to_commit.find(i)->second) != latest_commits_id.end()){
            dijkstra_commits<<id_to_commit.find(i)->second<<",";
        }
    }
    dijkstra_commits<<"\n";
    for (auto qoo = commit_to_id.begin(); qoo != commit_to_id.end(); qoo++){
        g.shortest_path(qoo->second, id_to_commit, latest_commits_id, dijkstra_commits);
    }
    
    
    //g.shortest_path(target_in_graph, id_to_commit, latest_commits_id);
    
    std::unordered_map<std::string,std::string> common_committer;
    std::cout<<"\nThese are the common committers to upstream and forks:\n";
    printf("Committer Name \t\t\t Email\n");
    for (auto itr = fork_committer.begin(); itr != fork_committer.end(); itr++){
        std::string this_email = itr->first;
        if (upstream_committer.find(this_email) != upstream_committer.end()){
            common_committer.insert(std::make_pair(itr->first, itr->second));
            std::cout<<itr->second<<"\t\t\t"<<itr->first<<"\n";
        }
    }
    
    std::cout<<"\nCommon committers' commits to upstream and forks:\n";
    printf("Committer Name \t\t\t Repo \t\t\t Time \t\t\t Commit ID\n");
    
    std::multimap <time_t, std::pair<std::string, std::string>> boo;
    for (auto itr = common_committer.begin(); itr != common_committer.end(); itr++){
        std::string this_email = itr->first;
        std::string this_name = itr->second;
        
        common_committer_commit(upstream_local, this_email, this_name, boo, detail_commits, upstream_url);
        for (int i = 1; i <= number_of_fork; i++){
            std::string foo = fork_local_string + std::to_string(i);
            auto fork_local = foo.c_str();
            common_committer_commit(fork_local, this_email, this_name, boo, detail_commits, fork_url[i-1]);
        }
        
    }
    
    /*
    struct tm earliest_tm;
    struct tm latest_tm;
    std::string earliest = boo.begin()->first;
    std::string latest = boo.rbegin()->first;
    const char* earliest_char = earliest.c_str();
    const char* latest_char = latest.c_str();
    
    strptime(earliest_char, "%a %b %d %H:%M:%S %Y", &earliest_tm);
    strptime(latest_char, "%a %b %d %H:%M:%S %Y", &latest_tm);
    */
    time_t earliest_t = boo.begin()->first;
    time_t latest_t = boo.rbegin()->first;
    
    //time difference between earliest and latest activities
    double total_window = std::difftime(latest_t, earliest_t);
    double interval = total_window/INTERVAL_NUMBER;
    for (int i = 1; i <= INTERVAL_NUMBER; i++){
        time_t upper = earliest_t + i * interval;
        time_t lower = earliest_t + (i-1) * interval;
        std::vector<std::pair<std::string, std::string>> table;
        
        for (auto itr = boo.begin(); itr != boo.end(); itr++){
            /*
            std::string this_time = itr->first;
            const char* this_time_char = this_time.c_str();
            struct tm this_tm;
            strptime(this_time_char, "%a %b %d %H:%M:%S %Y", &this_tm);
             */
            time_t this_t = itr->first;
            
            if (this_t > upper){
                break;
            }
            if (this_t < lower){
                continue;
            }
            
            table.push_back(itr->second);
            
        }
        
        sort(table.begin(), table.end());
        std::map<std::pair<std::string, std::string>, int> goo;
        for_each( table.begin(), table.end(), [&goo]( std::pair<std::string, std::string> pair ){ goo[pair]++; } );
        table.erase(unique(table.begin(), table.end() ), table.end());
        std::string upper_string = ctime(&upper);
        std::string lower_string = ctime(&lower);
        std::cout<<"\n From "<<lower_string<<" to "<<upper_string<<":\n";
        time_window<<"\n From "<<lower_string<<" to "<<upper_string<<":\n";
        for( auto p : goo ) {
            std::cout << p.first.first<< "\t\t"<<p.first.second<< "\t\t" << p.second << std::endl;
            time_window<<p.first.first<< ","<<p.first.second<< "," << p.second << "\n";
        }
        /*
        for (auto itr = table.begin(); itr != table.end(); itr++){
            std::cout<<itr->first<<"\t\t\t"<<itr->second<<"\n";
        }
         */
    }
    
    
    
    for (auto itr = boo.begin(); itr != boo.end(); itr++){
        std::string goo = ctime(&(itr->first));
        std::cout<<goo<<"\t\t\t"<<itr->second.first<<"\t\t\t"<<itr->second.second<<"\n";
    }
    
    //clean the local repo_fetch folder
    /*
    std::string clean_command_string = "cd ~/Documents/repo_fetch && rm *";
    const char* clean_command = clean_command_string.c_str();
    system(clean_command);
     */
    return 0;
}
