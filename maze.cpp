#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <list>
#include <set>
#include <cstdlib>
using namespace std;
namespace maze {
  struct pos {
    int x,y;
  };
  
  struct maze {
    vector<vector<bool> > wall;
    list<pos> moves;
    pos start;
    int rows,colls;
  };

  bool valid_coords(maze& m,int x,int y) {
    if(x<0) return false;
    if(y<0) return false;
    if(x>=m.rows) return false;
    if(y>=m.colls) return false;
    return true;
  }

  void read_maze(maze& m,const char* fname) {
    ifstream fin(fname);
    char c;
    string line;
    fin>>m.rows>>c>>m.colls;
    fin>>m.start.x>>c>>m.start.y;
    getline(fin,line);
    vector<bool> each_row(m.colls,false);
    m.wall.resize(m.rows,each_row);
    for(int i=0;i<m.rows;++i) {
      string line;
      getline(fin,line);
      for(int j=0;j<m.colls;++j) {
	c = line[j];
	switch(c) {
	case 'x': m.wall[i][j] = true; break;
	case ' ': m.wall[i][j] = false; break;
	default: break;
	};
      }
    }
  }

  void display_maze(maze& m) {
    typedef vector<char> char_vec;
    char_vec r(m.colls,' ');
    vector<char_vec> s(m.rows,r);
    for(int i=0;i<m.rows;++i) 
      for(int j=0;j<m.colls;++j) {
	if(m.wall[i][j])
	  s[i][j]='x';
	else 
	  s[i][j]=' ';
      }

  
    {
      pos* p = new pos;
      pos q;
      list<pos>::iterator iter = m.moves.begin();
      while(iter!=m.moves.end()) {
	s[iter->x][iter->y] = '.';
	++iter;
      };
    }

    for(int i=0;i<m.rows;++i) {
      for(int j=0;j<m.colls;++j)
	cout<<s[i][j];
      cout<<endl;
    }
    cout<<endl;

    cout<<"Moves : "<<endl;
    {
      list<pos>::iterator iter = m.moves.begin();
      while(iter!=m.moves.end()) {
	pos& p = *iter;
	cout<<"["<<p.x<<","<<p.y<<"]"<<endl;
	++iter;
      };
    }
  }
  

  bool solve_maze(maze& m) {
    int infinity = 999999999;
    typedef vector<int> int_vec;
    int_vec each_row(m.colls,infinity);
    vector<int_vec> moveId(m.rows,each_row); // int moveId[m.rows][m.colls];
    set<pair<int,int> > changedLocs;
    moveId[m.start.x][m.start.y]=0;
    changedLocs.insert(pair<int,int>(m.start.x,m.start.y));
    int deltas[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,-1},{-1,1}};
    while(!changedLocs.empty()) {
      set<pair<int,int> >::iterator iter = changedLocs.begin();
      pair<int,int> cur = *iter;
      changedLocs.erase(iter);
      for(int dir = 0;dir<8;dir++) {
	int newX = cur.first+deltas[dir][0];
	int newY = cur.second+deltas[dir][1];
	if(!valid_coords(m,newX,newY))
	  continue;
	if (!m.wall[newX][newY]) {
	  int curDistance = moveId[newX][newY];
	  for(int dir1 = 0;dir1<8;dir1++) {
	    int newX1 = newX+deltas[dir1][0];
	    int newY1 = newY+deltas[dir1][1];
	    if(valid_coords(m,newX1,newY1) &&
	       moveId[newX1][newY1] != infinity &&
	       !m.wall[newX1][newY1]){
	      int potentialNewDistance = moveId[newX1][newY1]+1;
	      if(potentialNewDistance < curDistance) {
		moveId[newX][newY] = potentialNewDistance;
		changedLocs.insert(pair<int,int>(newX,newY));
	      }
	    }
	  }
	}
      }
    }
  
    pos minExit;
    int minDistance = infinity;
    {
      int i,j;
      i=0; // top row 
      for(j=0;j<m.colls;j++)
	if(!m.wall[i][j] && 
	   !(i==m.start.x && j==m.start.y) && 
	   moveId[i][j]<minDistance) {
	  minExit.x = i;
	  minExit.y = j;
	  minDistance = moveId[i][j];
	}

      j=m.colls-1; // right side collumn
      for(i=0;i<m.rows;i++)
	if(!m.wall[i][j] &&
	   !(i==m.start.x && j==m.start.y) &&
	   moveId[i][j]<minDistance) {
	  minExit.x = i;
	  minExit.y = j;
	  minDistance = moveId[i][j];
	}

      i=m.rows-1; // bottom row
      for(j=0;j<m.colls;j++)
	if(!m.wall[i][j] &&
	   !(i==m.start.x && j==m.start.y) &&
	   moveId[i][j]<minDistance) {
	  minExit.x = i;
	  minExit.y = j;
	  minDistance = moveId[i][j];
	}

      j=0; // left side collumn
      for(i=0;i<m.rows;i++) 
	if(!m.wall[i][j] &&
	   !(i==m.start.x && j==m.start.y) &&
	   moveId[i][j]<minDistance) {
	  minExit.x = i;
	  minExit.y = j;
	  minDistance = moveId[i][j];
	}
    }
  
    if (minDistance == infinity) {
      m.moves.push_front(m.start);
      return false;
    }
    // trace back route... 
  
    {
      pos cur = minExit;
      do {
	m.moves.push_front(cur);
	for(int dir=0;dir<8;dir++) {
	  int newX = cur.x+deltas[dir][0];
	  int newY = cur.y+deltas[dir][1];
	  if(!valid_coords(m,newX,newY))
	    continue;
	  if(moveId[newX][newY]<moveId[cur.x][cur.y]) {
	    cur.x = newX;
	    cur.y = newY;
	    break;
	  }
	}
      } while(cur.x != m.start.x || cur.y != m.start.y); 
      m.moves.push_front(cur);
    }
    return true;
  }
}
using namespace maze;
int main(int argc,char** argv) {
  std::cout<<" argv[0] : "<<argv[0]<<std::endl;
  std::cout<<" argv[1] : "<<argv[1]<<std::endl;
  std::cout<<" argc : "<< argc << std::endl;
  maze::maze mz;
  maze::read_maze(mz,argv[1]);
  if(maze::solve_maze(mz)) 
    maze::display_maze(mz);
  else {
    cout<<" the maze cannot be solved"<<endl;
    maze::display_maze(mz);
  }
}
