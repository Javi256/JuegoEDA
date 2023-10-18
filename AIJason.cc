#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Jason


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  const vector<Dir> dirs = {Up,Down,Left,Right};


  /**
   * Play method, invoked once per each round.
   */

  bool enemigo(int u_id) {
    for (auto d : dirs) {
      Pos new_pos = unit(u_id).pos + d;
      if (pos_ok(new_pos)) {
        int id_cell = cell(new_pos.i,new_pos.j).id;
        if (cell(new_pos.i,new_pos.j).food) {
          move(u_id, d);
          return true;
        }

        else if (id_cell != -1 and unit(id_cell).type == Zombie) {
          move(u_id, d);
          return true;
        }

        else if (id_cell != -1 and unit(id_cell).type == Alive) {
          int und = unit(id_cell).player;
          int yo = me();
          if (und != yo) {
            move(u_id, d);
            return true;
          }
        }
      }
    }
    return false;
  }

  Dir encontrar_camino(vector< vector<pair<Pos, Dir>> >& Ant, Pos new_pos, Pos pos_u) {
    int f = new_pos.i;
    int c = new_pos.j;
    while(Ant[f][c].first != pos_u) {
      int f_aux = f;
      f = Ant[f][c].first.i;
      c = Ant[f_aux][c].first.j;
    }
    return Ant[f][c].second;
  }

  bool bfs_iterative(int u_id) {
    queue<Pos> Q;
    vector< vector<int> > Dist(board_rows(), vector<int> (board_cols(), -1)); //Si -1 --> no visitado
    vector< vector<pair<Pos, Dir>> > Ant(board_rows(),  vector<pair<Pos, Dir>> (board_cols())); //p
    Pos pos_u = unit(u_id).pos;
    Dist[pos_u.i][pos_u.j] = 0; 
    Q.push(pos_u);

    while(not Q.empty()) {
        Pos pos = Q.front();
        Q.pop();

        for (auto d: dirs) {
            Pos new_pos = pos + d;
            if (pos_ok(new_pos)) {
                int id_cell = cell(new_pos.i,new_pos.j).id;
                if (cell(new_pos.i,new_pos.j).type != Waste and Dist[new_pos.i][new_pos.j] == -1) {
                  Q.push(new_pos);
                  Dist[new_pos.i][new_pos.j] = Dist[pos.i][pos.j] + 1;
                  Ant[new_pos.i][new_pos.j] = {pos, d}; 

                  if (id_cell != -1 and unit(id_cell).type == Alive) {
                    int und = unit(id_cell).player;
                    if (und != me()) {
                      Dir dir = encontrar_camino(Ant, new_pos, pos_u);
                      move(u_id, dir);
                      return true;
                    }
                  }
                  
                  else if (id_cell != -1 and unit(id_cell).type == Zombie and random(0,3) <= 2) {
                    Dir dir = encontrar_camino(Ant, new_pos, pos_u);
                    move(u_id, dir);
                    return true;
                    if (Dist[new_pos.i][new_pos.j] % 2 == 0) {
                      move(u_id, dir);
                      return true;
                    }
                  }

                  else if (cell(new_pos.i,new_pos.j).food) {
                    if (Dist[new_pos.i][new_pos.j] >= 15) return false;
                    Dir dir = encontrar_camino(Ant, new_pos, pos_u);
                    move(u_id, dir);
                    return true;
                  }
                }
            }
        }
    }
    return false;
  }

  bool pos_vacias(int u_id) {
    for (auto d : dirs) {
      Pos new_pos = unit(u_id).pos + d;
      if (pos_ok(new_pos)) {
        int id_cell = cell(new_pos.i,new_pos.j).id;
        if (cell(new_pos.i,new_pos.j).type == Street and (id_cell != -1 or id_cell != me())) {
          move(u_id, d);
          return true;
        }
      }
    }
    return false;
  }

  virtual void play () {
    vector<int> alive = alive_units(me());

    for (int i = 0; i < int(alive.size()); ++i) {
      int id = alive[i];
      if (!enemigo(id)) {
        if (!bfs_iterative(id)) {
          if (!pos_vacias(id)) {
            Dir d = dirs[random(0,dirs.size()-1)];
            Pos new_pos = unit(id).pos + d;
            if (pos_ok(new_pos) and cell(new_pos.i,new_pos.j).type != Waste) move(id,d);
          }
        }
      }
    }
  }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);