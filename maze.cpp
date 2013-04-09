#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <list>
#include <set>
#include <cstdlib>

struct pos {
  int x,y;
};

struct maze {
  std::vector<std::vector<bool> > wall;
  std::list<pos> moves;
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
  std::ifstream fin(fname);
  char c;
  std::string line;
  fin>>m.rows>>c>>m.colls;
  fin>>m.start.x>>c>>m.start.y;
  getline(fin,line);
  m.wall.resize(m.rows,std::vector<bool>(m.colls,false));
  for(int i=0;i<m.rows;++i) {
    std::string line;
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
  std::vector<std::vector<char> > s(m.rows,std::vector<char>(m.colls,' '));
  for(int i=0;i<m.rows;++i) 
    for(int j=0;j<m.colls;++j) {
      if(m.wall[i][j])
	s[i][j]='x';
      else 
	s[i][j]=' ';
    }

  
  {
    std::list<pos>::iterator iter = m.moves.begin();
    while(iter!=m.moves.end()) {
      pos& p = *iter;
      s[p.x][p.y] = '.';
      ++iter;
    };
  }
  for(int i=0;i<m.rows;++i) {
    for(int j=0;j<m.colls;++j)
      std::cout<<s[i][j];
    std::cout<<std::endl;
  }
  std::cout<<std::endl;

  std::cout<<"Moves : "<<std::endl;
  {
    std::list<pos>::iterator iter = m.moves.begin();
    while(iter!=m.moves.end()) {
      pos& p = *iter;
      std::cout<<"["<<p.x<<","<<p.y<<"]"<<std::endl;
      ++iter;
    };
  }
}

bool solve_maze(maze& m) {
  int infinity = 999999999;
  std::vector<std::vector<int> > moveId(m.rows,std::vector<int>(m.colls,infinity));
  std::set<std::pair<int,int> > changedLocs;
  moveId[m.start.x][m.start.y]=0;
  changedLocs.insert(std::pair<int,int>(m.start.x,m.start.y));
  const int deltas[]={-1,0,1};
  while(!changedLocs.empty()) {
    std::set<std::pair<int,int> >::iterator iter = changedLocs.begin();
    std::pair<int,int> cur = *iter;
    for(int i=0;i<3;i++)
      for(int j=0;j<3;j++) {
	if(i==1 && j==1)
	  continue;
	int newX = cur.first+deltas[i];
	int newY = cur.second+deltas[j];
	if(!valid_coords(m,newX,newY))
	  continue;
	if (!m.wall[newX][newY]) {
	  int curDistance = moveId[newX][newY];
	  int newDistance = curDistance;
	  for(int p=0;p<3;p++) 
	    for(int q=0;q<3;q++) {
	      if(p==1&&q==1)
		continue;
	      int newX1 = newX+deltas[p];
	      int newY1 = newY+deltas[q];
	      if(!valid_coords(m,newX1,newY1))
		continue;
	      if(moveId[newX1][newY1] == infinity)
		continue;
	      if(m.wall[newX1][newY1])
		continue;
	      int potentialNewDistance = moveId[newX1][newY1]+1;
	      if (potentialNewDistance < newDistance)
		newDistance = potentialNewDistance;
	    }
	  if(newDistance<curDistance) {
	    moveId[newX][newY] = newDistance;
	    changedLocs.insert(std::pair<int,int>(newX,newY));
	  }
	}
      }
    changedLocs.erase(iter);
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
    int deltas[8][2] = {{0,1},{0,-1},{1,0},{-1,0},
		       {1,1},{1,-1},{-1,-1},{-1,1}};
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
    } while(!(cur.x == m.start.x && cur.y == m.start.y)); 
    m.moves.push_front(cur);
  }
  return true;
}


void random_maze(maze& m) {
  m.rows = 10;
  m.colls = 20;
  srand ( time(NULL) );
  m.wall.resize(m.rows,std::vector<bool>(m.colls,false));
  for(int i=0;i<m.rows;++i) 
    for(int j = 0;j<m.colls;++j)
      m.wall[i][j]=rand()%4==1;
  if(0) { 
    switch(rand()%4) {
    case 0:
      m.start.x = 0;
      m.start.y = rand()%m.colls;
      break;
    case 1:
      m.start.x = rand()%m.rows;
      m.start.y = m.colls-1;
      break;
    case 2:
      m.start.x = m.rows-1;
      m.start.y = rand()%m.colls;
      break;
    case 3:
      m.start.x = rand()%m.rows;
      m.start.y = 0;
      break;
    }
    m.wall[m.start.x][m.start.y] = false;    
  }
  else 
    {
      do {
	m.start.x = rand()%m.rows;
	m.start.y = rand()%m.colls;
      } while(m.wall[m.start.x][m.start.y]);
    }
  
}

int main(int argc,char** argv) {
  maze mz;
  if(argc>1) 
    read_maze(mz,argv[1]);
  else
    random_maze(mz);

  if(solve_maze(mz)) 
    display_maze(mz);
  else {
    std::cout<<" the maze cannot be solved"<<std::endl;
    display_maze(mz);
  }
}
