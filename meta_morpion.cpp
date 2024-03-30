#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <ctime>
#include <math.h>
#include <algorithm>
#include <memory>

#define SIZE 9
#define MAXIPOINT 50000000
using namespace std;
int hMax;
//-----------------------------------------------------------------------------------------------------------------------------------------//
int tictactoe_id_row(int row, int col);
int tictactoe_id_col(int row, int col);
void version(int (&move)[2], int row, int col);
void theme(int (&move)[2], int row, int col);
//-----------------------------------------------------------------------------------------------------------------------------------------//

class State{
  public :
  int win;
  int last_col;
  int last_row;
  int next_col;
  int next_row;
  int game_turn;
  int free_choice_count;
  int who_played_last;
  int occupied[SIZE][SIZE];
  int avaible[SIZE];
  static constexpr int matrix_point[SIZE][SIZE]=
  {
      {30,20,30,20,25,20,30,20,30},
      {24,16,24,16,20,16,24,16,24},
      {30,20,30,20,25,20,30,20,30},
      {24,16,24,16,20,16,24,16,24},
      {36,24,36,24,30,24,36,24,36},
      {24,16,24,16,20,16,24,16,24},
      {30,20,30,20,25,20,30,20,30},
      {24,16,24,16,20,16,24,16,24},
      {30,20,30,20,25,20,30,20,30}
  };

  State();
  int winner(int row);
  int winner_meta();

  int tictactoe_mult(int row);
  bool update_avaible(int row);
};
//-----------------------------------------------------------------------------------------------------------------------------------------//
State::State() : win(0), last_col(-1), last_row(-1), next_col(-1), next_row(-1), free_choice_count(0), who_played_last(0),  game_turn(0)
{
    for (int i=0;i<SIZE;i++)
    {
        avaible[i]=0;
        for (int j=0;j<SIZE;j++)
        {
            occupied[i][j]=0;
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int State::winner(int row)
{
    if
    (
    (occupied[row][0]==who_played_last    && occupied[row][1]==who_played_last      && occupied[row][2]==who_played_last) ||
    (occupied[row][3]==who_played_last    && occupied[row][4]==who_played_last      && occupied[row][5]==who_played_last) ||
    (occupied[row][6]==who_played_last    && occupied[row][7]==who_played_last      && occupied[row][8]==who_played_last) ||
    (occupied[row][0]==who_played_last    && occupied[row][3]==who_played_last      && occupied[row][6]==who_played_last) ||
    (occupied[row][1]==who_played_last    && occupied[row][4]==who_played_last      && occupied[row][7]==who_played_last) ||
    (occupied[row][2]==who_played_last    && occupied[row][5]==who_played_last      && occupied[row][8]==who_played_last) ||
    (occupied[row][0]==who_played_last    && occupied[row][4]==who_played_last      && occupied[row][8]==who_played_last) ||
    (occupied[row][2]==who_played_last    && occupied[row][4]==who_played_last      && occupied[row][6]==who_played_last)
    )
    {
        return who_played_last;
    }
    else
    {
        return 0;
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int State::winner_meta()
{
    if
    (
    (avaible[0]==who_played_last  && avaible[1]==who_played_last      && avaible[2]==who_played_last) ||
    (avaible[3]==who_played_last  && avaible[4]==who_played_last      && avaible[5]==who_played_last) ||
    (avaible[6]==who_played_last  && avaible[7]==who_played_last      && avaible[8]==who_played_last) ||
    (avaible[0]==who_played_last  && avaible[3]==who_played_last      && avaible[6]==who_played_last) ||
    (avaible[1]==who_played_last  && avaible[4]==who_played_last      && avaible[7]==who_played_last) ||
    (avaible[2]==who_played_last  && avaible[5]==who_played_last      && avaible[8]==who_played_last) ||
    (avaible[0]==who_played_last  && avaible[4]==who_played_last      && avaible[8]==who_played_last) ||
    (avaible[2]==who_played_last  && avaible[4]==who_played_last      && avaible[6]==who_played_last)
    )
    {
        win=who_played_last;
        return who_played_last;
    }
    else
    {
        return 0;
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int State::tictactoe_mult(int row)
{
    int mult = 1;
    for (int j=0;j<SIZE;j++)
    {
        mult*=occupied[row][j];
    }
    return mult;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
bool State::update_avaible(int x)
{
    int temp=avaible[x];
    int winn=winner(x);
    if(tictactoe_mult(x)!=0 && winn==0) // multiplication par 0 qui veut dire libre dans occupied, donc si 1 seul libre, retourne mult==0
    {
        avaible[x]=2; //valeur arbitraire pour eviter de refouiller un tableau complet sans gagnant 
    }
    else
    {
        avaible[x]=winn;
    }
    return !(temp==avaible[x]); // return true si avaible a été modifié
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
class Node {
public:
    State state;
    weak_ptr<Node> parent;
    vector<shared_ptr<Node>> child_array;
    int children_count;
    vector<pair<int,int>> playable_next;
    int score;
    bool free;
    int alpha;
    int beta;

public:
    Node();
    int playable_next_generate();
    void children_generate(int player_turn);
    void minimax(int h, int player_turn);
    int exist_children();
    int eval();
    void playable_sort();
};
//-----------------------------------------------------------------------------------------------------------------------------------------//
Node::Node() : children_count(0), free(false), score(0)
{
    vector<shared_ptr<Node>> child_array;
    state=State();
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void Node::playable_sort(){
    vector<pair<int,int>> playable_next_2;

    for(int i=0;i<playable_next.size();i++){
        if (playable_next[i].second!=1 && playable_next[i].second!=3 && playable_next[i].second!=5 && playable_next[i].second!=7){
            playable_next_2.push_back(make_pair(playable_next[i].first,playable_next[i].second));
        }
    }
    for(int i=0;i<playable_next.size();i++){
        if (playable_next[i].second==1 || playable_next[i].second==3 || playable_next[i].second==5 || playable_next[i].second==7){
            playable_next_2.push_back(make_pair(playable_next[i].first,playable_next[i].second));
        }
    }

    playable_next.swap(playable_next_2);
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::playable_next_generate(){
    int next_children_count=0;
    if(state.win==0)
    {  
        if(this->state.last_row==-1)
        {
            for (int i = 0;i<9;i++)
            {
                for (int j = 0;j<9;j++)
                {   
                    if (state.occupied[i][j]==0)
                    {
                        playable_next.push_back(make_pair (i, j));
                        next_children_count++;
                    }
                }
            }
        } else
        {   
            if(this->state.avaible[this->state.last_col]!=0) // si le morpion numero id_col est indisponible alors on rend dispo tout le meta morpion sauf ce qui n'est pas dispo (inverse d'occupied) en plus du morpion actuellement indisponible qui n'est pas forcement à jour dans avaible à cause des passages par references, puisque c'est dangereux de le modifier dans les simulations
            {
                for (int i = 0;i<9;i++)
                {
                    for (int j = 0;j<9;j++)
                    {   
                        if((state.avaible[i]==0 && state.occupied[i][j]==0)) //evaluation paresseuse optimisante
                        { 
                            playable_next.push_back(make_pair (i, j));
                            next_children_count++;
                        }
                    }
                } 
                if (next_children_count>0)
                {
                    free=true;
                }
            } else // si pas de probleme de disponibilité sur le morpion destination, alors on fait un inverse de occupied du morpion local
            {
                for (int k = 0;k<9;k++)
                {
                    if(state.occupied[state.last_col][k]==0)
                    {
                        playable_next.push_back(make_pair (state.last_col, k));
                        next_children_count++;
                    }
                }
            } 
        }
    }
    return next_children_count;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void Node::children_generate(int player_turn)
{
  shared_ptr<Node> child = make_shared<Node>();
  child->state=this->state;
  child->state.free_choice_count=0;
  child->state.last_row=state.next_row;
  child->state.last_col=state.next_col;
  child->state.who_played_last=player_turn;
  child->state.game_turn++;
  child->state.occupied[child->state.last_row][child->state.last_col]=player_turn;
  if(child->state.update_avaible(child->state.last_row))
  {
    child->state.win=child->state.winner_meta();
  }
  this->child_array.push_back(child);
  this->children_count++;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::exist_children()
{
    for (int i = 0; i<this->child_array.size();i++)
    {
        if (child_array[i]->state.last_row == state.next_row && child_array[i]->state.last_col == state.next_col )
        {
            return i;
        }    
    }
return -1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::eval()
{
    //int sum = rand()%100 +1; //* (rand() % 1 -0.5);  
    int sum=0; // une simulation suivante peut être plus defavorable donc recalculer de 0 (perte de points car l'adversaire s'interpose par exemple)
    if(state.game_turn<10) // les premiers coups
    {
        for (int i=0;i<SIZE;i++)
        {
            for (int j=0;j<SIZE;j++)
            {
                int turn=state.occupied[i][j];
                sum+=state.matrix_point[i][j]*turn; // points de cases pondérées possédées. Pour chaque cases du meta morpion, evaluer sa valeur, sommer les  points de chaque case
            }
        }
    }
    //sum-=state.free_choice_count*30;  // chaque coup libre pour l'adversaire est une penalité
    for (int i=0;i<SIZE;i++)
    {
        int coef;
        if(i==4){coef=6;}  // centre du meta
        else if(i==0 || i==2 || i==6 || i==8){coef=5;} // coins du meta
        else {coef=4;} // le reste du meta
        if(state.avaible[i]!=0 && state.avaible[i]==state.who_played_last) //evaluation paresseuse
        {
            sum+=200*coef*state.who_played_last;
        } 
        if(state.avaible[i]==0)
        {
            for (int j=0;j<SIZE;j++)
            {
                int turn=state.occupied[i][j];
                if (turn!=0)
                {
                    //verifie si une deuxième case en colonne comporte le meme symbole, sans être bloqué par l'adversaire sur le 3ème (donc libre)
                    if ((state.occupied[i][(j+3)%9]==turn || state.occupied[i][(j+6)%9]==turn) && (state.occupied[i][(j+3)%9]==0 || state.occupied[i][(j+6)%9]==0))
                    {
                        sum+=turn*coef; //va être compté 2 fois, exemple (0 et 3) et (3 et 0)
                    }
                    //verifie si une deuxième case en ligne comporte le meme symbole, sans être bloqué par l'adversaire sur le 3ème (donc libre)
                    if ((state.occupied[i][(j+1)%3+(j/3)*3]==turn || state.occupied[i][(((j+2)%3)+(j/3)*3)]==turn) && (state.occupied[i][(j+1)%3+(j/3)*3]==0 || state.occupied[i][(((j+2)%3)+(j/3)*3)]==0))
                    {
                        sum+=turn*coef;
                    }
                    //verifie si une deuxième case en diagonale \ comporte le meme symbole, sans être bloqué par l'adversaire sur le 3ème (donc libre)
                    if (j==0||j==4||j==8)
                    {
                        if ((state.occupied[i][((j+4)%12)]==turn || state.occupied[i][((j+8)%12)]==turn) && (state.occupied[i][((j+4)%12)]==0 || state.occupied[i][((j+8)%12)]==0))
                        {
                            sum+=turn*coef;
                        }
                    }
                    //verifie si une deuxième case en diagonale / comporte le meme symbole, sans être bloqué par l'adversaire sur le 3ème (donc libre)
                    if (state.occupied[i][2]==turn)
                    {
                        if ((state.occupied[i][4]==turn || state.occupied[i][6]==turn) && (state.occupied[i][4]==0 || state.occupied[i][6]==0))
                        {
                            sum+=turn*coef; // compte 2 fois aussi avec le if suivant
                        }
                    }
                    if (state.occupied[i][4]==turn)
                    {
                        if ((state.occupied[i][2]==turn || state.occupied[i][6]==turn) && (state.occupied[i][2]==0 || state.occupied[i][6]==0))
                        {
                            sum+=turn*coef;
                        }
                    }
                    if (state.occupied[i][6]==turn)
                    {
                        if ((state.occupied[i][4]==turn || state.occupied[i][2]==turn) && (state.occupied[i][4]==0 || state.occupied[i][2]==0))
                        {
                            sum+=turn*coef;
                        }
                    }
                }
            }
        }
    }
    //cerr<<sum<<endl;
    return sum;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void Node::minimax(int h, int player_turn)
{
    //cerr<<h<<" "<<player_turn<<" "<<state.next_row<<state.next_col<<" "<<alpha<<" "<<beta<<endl;
    int k=exist_children();// retourne l'indice du fils qui existe deja sinon -1
    if (k==-1)
    {   
        children_generate(player_turn);
        k=child_array.size()-1; // le fils n'existait pas, maintenant qu'il est créé, k est son indice
        int next_children_count=child_array[k]->playable_next_generate();
        child_array[k]->playable_sort();
        
    }
    else
    {
       /* if(h==hMax)
        {
            h+=2;
            hMax+=2;
        }   */  
    }

    // cas d'arret
    if (h<=0 || child_array[k]->playable_next.size()<=0)
    {   
        if (child_array[k]->playable_next.size()>0)
        {
            child_array[k]->alpha=this->alpha;
            child_array[k]->beta=this->beta; 
        }
        score=child_array[k]->eval();
        //cerr<<"score "<<score<<" "<<child_array[k]->state.last_row<<child_array[k]->state.last_col<<endl;
    } else
    {
        score=MAXIPOINT*player_turn;
        for (int i=0; i<child_array[k]->playable_next.size(); ++i) 
        {
            child_array[k]->state.next_row=child_array[k]->playable_next[i].first;
            child_array[k]->state.next_col=child_array[k]->playable_next[i].second;
            child_array[k]->alpha=this->alpha;
            child_array[k]->beta=this->beta;
            child_array[k]->minimax(h-1,-player_turn);
            if (player_turn==1)
            { //noeud min
                beta=min(beta,child_array[k]->score);
                score=min(score,child_array[k]->score);
                if (child_array[k]->free)
                {
                    state.free_choice_count++;
                }
            }
            else 
            { //noeud max 
                alpha=max(alpha,child_array[k]->score);
                score=max(score,child_array[k]->score);
            }
            if (alpha>=beta)
            {
                break;
            } 
            //cerr<<h<<" "<<player_turn<<" "<<state.next_row<<state.next_col<<" "<<alpha<<" "<<beta<<" "<<score<<endl;
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//convertisseur [morpion][case]
int tictactoe_id_row(int row,int col)
{
    return (row/3)*3 + (col/3);
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int tictactoe_id_col(int x,int y)
{
    int id_col = y;
    if(x==0 || x==3 or x==6)
    {
        if (y>2 && y<6) {id_col-=3;}
        if (y>5) {id_col-=6;}
    }
    if(x==1 || x==4 || x==7)
    {
        if (y<3) {id_col+=3;}
        if (y>5) {id_col-=3;}
    }
    if(x==2 || x==5 || x==8)
    {
        if (y<3) {id_col+=6;}
        if (y>2 && y<6) {id_col+=3;}
    }
    return id_col;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void version(int (&move)[2], int row,int col) // on ne passe pas directement x et y en référence pour éviter les effets de bord dans theme qui teste version pour emprunter le chemin inverse de sa bijection
{
    move[0] = tictactoe_id_row(row,col);
    move[1] = tictactoe_id_col(row,col);
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void theme(int (&move)[2],int row,int col)
{
    bool translated=false;
    for (int i=0;i<SIZE;i++)
    {
        if (!translated)
        {
            for (int j=0;j<SIZE;j++)
            {   //bijection donc reversible facilement
                version(move,i,j);
                if ((move[0]==row) && (move[1]==col))
                {
                    move[0]=i;
                    move[1]=j;
                    translated=true;
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------------------------------//
int main()
{
    std::chrono::system_clock::time_point chronoStart = std::chrono::system_clock::now();
    Node root = Node();
    shared_ptr<Node> child0(new Node());
    root.child_array.push_back(child0);
    child0 = nullptr;
    shared_ptr<Node> best_child;
    int move[2]={};
    int h;
    int row, col, nb_child; // variables pour eviter la desynchronisation

//-----------------------------------------------------------------------------------------------------------------------------------------//
    while (1) {

        cin >> root.child_array[0]->state.last_row >> root.child_array[0]->state.last_col; cin.ignore();
        version(move,root.child_array[0]->state.last_row,root.child_array[0]->state.last_col);

       cin >> nb_child; cin.ignore();
        for (int i = 0; i < nb_child; i++) {
            cin >>row>>col; cin.ignore();
        }

        // faire descendre sur l'arbre après le coup adverse
        if (move[0]!=-1 && move[1]!=-1)
        {   
            root.child_array[0]->state.next_row=move[0];
            root.child_array[0]->state.next_col=move[1];
            int opp = root.child_array[0]->exist_children();
            if (opp==-1)
            {
                root.child_array[0]->children_generate(-1);
                opp=0;
            }
            best_child=root.child_array[0]->child_array[opp];
            root.child_array[0]=best_child;
        }

        if (root.child_array[0]->playable_next.size()==0)
        {
            root.child_array[0]->playable_next_generate();

            //affichage avant tri
            cerr<<"avant tri : ";
            for (int i=0; i<root.child_array[0]->playable_next.size(); ++i) 
            {  
                cerr<<root.child_array[0]->playable_next[i].first<<root.child_array[0]->playable_next[i].second<<" ";
            }
            cerr<<endl;

            root.child_array[0]->playable_sort();
            //soit tu rajoutes le tri ici, soit dans png (playable next generate), à voir si teux penses qu'on à besoin de l'appeler qu'en debut de partie, ou plusieurs fois par tours...
        }  
        cerr<<"coup jouable : ";
        for (int i=0; i<root.child_array[0]->playable_next.size(); ++i) 
        {  
            cerr<<root.child_array[0]->playable_next[i].first<<root.child_array[0]->playable_next[i].second<<" ";
        }
        cerr<<endl;

        h=2; // appel à possibleDepth() 
        hMax=4;
        root.score=-MAXIPOINT-1;
        root.child_array[0]->alpha=-MAXIPOINT;
        root.child_array[0]->beta=MAXIPOINT;

        for (int i=0; i<root.child_array[0]->playable_next.size(); ++i) 
        {   
            root.child_array[0]->state.next_row=root.child_array[0]->playable_next[i].first;
            root.child_array[0]->state.next_col=root.child_array[0]->playable_next[i].second;

            //cerr<<"while minimax : "<<root.child_array[0]->state.next_row<<root.child_array[0]->state.next_col<<" "<<root.child_array[0]->alpha<<" "<<root.child_array[0]->beta<<endl;
            root.child_array[0]->minimax(h,1);
            //root.child_array[0]->alpha=max(root.child_array[0]->alpha,root.child_array[0]->score);
            //cerr<<"best vs minimax : "<<root.score<<" "<<root.child_array[0]->score<<endl;
            if(root.score<root.child_array[0]->score)
            {
                root.score=root.child_array[0]->score;
                best_child=root.child_array[0]->child_array[i];
                //cerr<<" &  found : "<<root.child_array[0]->child_array[i]<<endl;
            }
            

            //cerr<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
        }
        
        //cerr<<"root child : "<<root.child_array[0]<<endl;
        best_child->state.occupied[best_child->state.last_row][best_child->state.last_col]=1;
        best_child->state.update_avaible(best_child->state.last_row);
        root.child_array[0]=best_child;
        //cerr<<"nouveau root child : "<<root.child_array[0]<<endl;
        //cerr<<"childcountalafin : "<<root.child_array[0]->children_count<<endl;
        theme(move,best_child->state.last_row,best_child->state.last_col);
        cout <<move[0]<<" "<<move[1]<< endl;

        //cerr<<"------------------------------------"<<endl;
    }
}