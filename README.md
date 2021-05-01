# hard-fork-analysis
This is the baseline tool for hard fork analysis in GitHub.
It provides a detailed breakdown of each commit, the belongingship (upstream or fork) of each commit, and a time-window-slicing summary of common contrubutors' activities in different repos.

## Authors 
Jiachen Meng (jiachen"dot"meng"at"mail.utoronto.ca)\
Under supervision of Prof. Shurui Zhou

## What you need to run the code
Note that I wrote and ran the code in Xcode with MacOS Catalina\
\
All the code in this repo\
[libgit2 library](https://libgit2.org/) installed

## Procedure
0. You may need to change the first few lines in main(). Make sure all the local path variables and command line strings work on your own machine. Basically the repos are downloaded to the specified location and the remaining program runs locally. Depending on your machine, the libgit2 installation could be messy. For MacOS users, I recommend use the dynamic library (sth like "libgit2.1.1.dylib").
1. Follow the instruction in cout. Provide the GitHub URL of upstream and fork repos. It supports multiple forks of the same upstream. Use "end" to indicate all forks are entered.
2. The output in cout can be safely ingonred. All useful information has been summarized in three .csv files. They are in the same directory as the compiled program. detail_commits_result.csv should be intuitive itself. time_window_result.csv summarizes the activiteis of common contributors. Note that the number of time windows can be changed by INTERVAL_NUMBER. dijkstra_commits_result.csv gives the result of running Dijkstra on the repos graph. Each row is a commit (could be from upstream or any fork) and each col stands for a branch (shown by the last commit in that branch). 0 means the commit is on that branch. 1 means the commit could go to that branch, but it needs to travel across different repos (i.e. itself is in a different branch). INF means it cannot go to that branch. Try to draw a simple graph if this doesn't make sense.
3. To test if the program runs as expected on your end, you can compare the three generated files against my sample in this repo. Use https://github.com/jiachen-meng/simple_fork for upstream and https://github.com/chason-alt/simple_fork_alt for fork.
