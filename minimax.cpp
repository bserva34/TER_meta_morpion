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

//-----------------------------------------------------------------------------------------------------------------------------------------//

int tictactoe_id_row(int row, int col);
int tictactoe_id_col(int row, int col);
void version(int (&move)[2], int row, int col);
void theme(int (&move)[2], int row, int col);

//-----------------------------------------------------------------------------------------------------------------------------------------//
class State{
  public :
  int win=0; //indique le vainqueur de la partie
  int last_col=-1; //dernier coup colonne joué, -1 si premier tour et aversaire n'a pas joué, est actualisé sinon, et ne change jamais
  int last_row=-1; //idem pour la ligne
  int next_col=-1; //sert à charger le prochain coup, change autant de fois qu'il y a de simulation
  int next_row=-1; //idem pour la ligne
  int game_turn=0; //compte combien de tour on été joué, pour lancer des évaluations statiques differentes
  int free_choice_count=0; //combien d'enfant on eu l'opportunité de jouer où bon leur semblait sur la grille (heuristiques)
  int who_played_last; //identité du joueur de last_col/row, nécessaire de le differencier de player_turn car il existe un décalage dans la récursion
  int occupied[SIZE][SIZE]={}; //grille des cases, 0 si libre, 1 pour le joueur (nous), -1 pour l'adversaire
  int avaible[SIZE]={}; //grille des morpion, meme codage et 2 si remplis sans gagnant
  static constexpr int matrix_point[SIZE][SIZE]= //matrice statique pour les heuristique, et sauver du temps de calcul
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

  int winner();
  int winner_meta();
  int tictactoe_mult();
  bool update_avaible();
};
//-----------------------------------------------------------------------------------------------------------------------------------------//
int State::winner() //inique s'il y a une gagnant pour un morpion donné
{
    if
    (
    (occupied[last_row][0]==who_played_last    && occupied[last_row][1]==who_played_last      && occupied[last_row][2]==who_played_last) ||
    (occupied[last_row][3]==who_played_last    && occupied[last_row][4]==who_played_last      && occupied[last_row][5]==who_played_last) ||
    (occupied[last_row][6]==who_played_last    && occupied[last_row][7]==who_played_last      && occupied[last_row][8]==who_played_last) ||
    (occupied[last_row][0]==who_played_last    && occupied[last_row][3]==who_played_last      && occupied[last_row][6]==who_played_last) ||
    (occupied[last_row][1]==who_played_last    && occupied[last_row][4]==who_played_last      && occupied[last_row][7]==who_played_last) ||
    (occupied[last_row][2]==who_played_last    && occupied[last_row][5]==who_played_last      && occupied[last_row][8]==who_played_last) ||
    (occupied[last_row][0]==who_played_last    && occupied[last_row][4]==who_played_last      && occupied[last_row][8]==who_played_last) ||
    (occupied[last_row][2]==who_played_last    && occupied[last_row][4]==who_played_last      && occupied[last_row][6]==who_played_last)
    )
    {
        return who_played_last;
    }
    else return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int State::winner_meta() //indique s'il y a un gagnant de la grille de méta_morpion
{
    int win_meta;
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
        win_meta = who_played_last;
    }
    else
    {
        int tie = 1;
        for (int i=0;i<SIZE;i++)
        {
            tie*=avaible[i];
        }
        if (tie==0) win_meta = 0;
        else
        {
            int cpt1 = 0;
            int cpt2 = 0;
            for (int i=0;i<SIZE;i++)
            {
                if (avaible[i]!=2)
                {
                    if (avaible[i]) cpt1++;
                    else cpt2++;
                }
            }
            if (cpt1<cpt2)  win_meta = -1;
        }
    }
    return win_meta;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int State::tictactoe_mult() //indique si une case est libre dans une morpion
{
    int mult = 1;
    for (int j=0;j<SIZE;j++)
    {
        mult*=occupied[last_row][j];
    }
    return mult;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
bool State::update_avaible() //actualise le tableau des morpions occupés, et indique si il y a eu une modification
{
    int temp=avaible[last_row];
    int winn=winner();
    // multiplication par 0 qui veut dire libre dans occupied, donc si 1 seul libre, retourne mult==0
    if(tictactoe_mult()!=0 && winn==0) avaible[last_row]=2; //valeur arbitraire pour eviter de refouiller un tableau complet sans gagnant 
    else avaible[last_row]=winn;
    return !(temp==avaible[last_row]); // return true si avaible a été modifié
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
class Node {
public:
    State state; //état de jeu, le plateau
    vector<shared_ptr<Node>> child_array; //structure dynamique des simulations de type noeud shared pointer 
    vector<pair<int,int>> playable_next; //structure ynamique paire, pour les coordonnées
    int score=0; //le score d'une simulation, après évaluation statique
    bool free=false; //booléen indiquant si on peut jouer où l'on veut dans la grille ou seulement un morpion précis
    int alpha; //attribut pour le pruning
    int beta; //attribut pour le pruning

    int playable_next_generate();
    void children_generate(int player_turn);
    void minimax(int h, int player_turn,std::chrono::duration<double> allocated_time);
    int exist_children();
    int eval();
    void sort_playable();
    int possible_horizon(std::chrono::duration<double> allocated_time);
};
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::possible_horizon(std::chrono::duration<double> allocated_time) //tentative d'alterer l'horizon en fonction du temps restant et du nombre de simulations à parcourir
{   
    int h=2;
    if (playable_next.size()<7) h=4;
    if (playable_next.size()<3) h=6;
    if (playable_next.size()==0 || allocated_time < 0.2s) h=0;
    return h;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::playable_next_generate(){ //gérère le vector de simulation, gère toutes les situations possibles, et empêche la création si le jeu est considéré comme terminé
    int next_children_count=0;
    if(state.win==0)
    {  
        if(this->state.last_row==-1)
        {
            for (int i = 0;i<SIZE;i++)
            {
                for (int j = 0;j<SIZE;j++)
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
                for (int i = 0;i<SIZE;i++)
                {
                    for (int j = 0;j<SIZE;j++)
                    {   
                        if((state.avaible[i]==0 && state.occupied[i][j]==0)) //evaluation paresseuse optimisante
                        { 
                            playable_next.push_back(make_pair (i, j));
                            next_children_count++;
                        }
                    }
                } 
                if (next_children_count>0) free=true;
            } else // si pas de probleme de disponibilité sur le morpion destination, alors on fait un inverse de occupied du morpion local
            {
                for (int k = 0;k<SIZE;k++)
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
void Node::sort_playable(){ //tri en mettant les coups défavorables en premier (bord d'abord, puis coin et millieu), semble contre intuitif mais plusieurs test, l'inverse, un melange, un tri sur les morpions pltuto que les cases... on été fait, et c'est celui-ci qui procure un maximum d'élagage
    vector<pair<int,int>> playable_next_2;

    for(int i=0;i<playable_next.size();i++){
        if (playable_next[i].second!=0 && playable_next[i].second!=2 && playable_next[i].second!=4 && playable_next[i].second!=6 && playable_next[i].second!=8){
            playable_next_2.push_back(make_pair(playable_next[i].first,playable_next[i].second));
        }
    }
    for(int i=0;i<playable_next.size();i++){
        if (playable_next[i].second==0 || playable_next[i].second==2 || playable_next[i].second==4 || playable_next[i].second==6 || playable_next[i].second==8){
            playable_next_2.push_back(make_pair(playable_next[i].first,playable_next[i].second));
        }
    }
    playable_next.swap(playable_next_2);
} 
//-----------------------------------------------------------------------------------------------------------------------------------------//
void Node::children_generate(int player_turn) //crée un noeud, et le configure, puis l'ajoute au vector chargée de le stocker
{
  shared_ptr<Node> child = make_shared<Node>();
  child->state=this->state;
  child->state.free_choice_count=0;
  child->state.last_row=state.next_row;
  child->state.last_col=state.next_col;
  child->state.who_played_last=player_turn;
  child->state.game_turn++;
  child->state.occupied[child->state.last_row][child->state.last_col]=player_turn;
  if(child->state.update_avaible()) child->state.win=child->state.winner_meta();
  this->child_array.push_back(child);
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::exist_children() //cherche à partir de coordonnées si le vector child_array contient une simulation particulière
{
    for (int i = 0; i<this->child_array.size();i++)
    {
        if (child_array[i]->state.last_row == state.next_row && child_array[i]->state.last_col == state.next_col ) return i; 
    }
    return -1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int Node::eval()
{
    int sum=0; // on remet à zéro car une simulation suivante peut être plus defavorable donc recalculer de 0 (perte de points car l'adversaire s'interpose par exemple)
    if (state.win!=0) return sum=state.win*MAXIPOINT/2;
    if(state.game_turn<10) // les premiers coups
    {
        for (int i=0;i<SIZE;i++)
        {
            for (int j=0;j<SIZE;j++)
            {
                int turn=state.occupied[i][j]; //identifie qui possère la case : nous (1) ou l'adversaire (-1) ou personne (0)
                sum+=state.matrix_point[i][j]*turn; // points de cases pondérées possédées. Pour chaque cases du meta morpion, evaluer sa valeur, sommer les  points de chaque case
            }
        }
    }
    sum-=state.free_choice_count*10;  // chaque coup libre pour l'adversaire est une penalité
    for (int i=0;i<SIZE;i++)
    {
        int coef;
        if(i==4) coef=6;  // centre du meta
        else if(i==0 || i==2 || i==6 || i==8) coef=5; // coins du meta
        else coef=4; // le reste du meta

        int turn;

        //evaluation paresseuse
        if(state.avaible[i]!=0 && state.avaible[i]==state.who_played_last) sum+=200*coef*state.who_played_last;
        if(state.avaible[i]==0)
        {
            //recherche alignements de cases
            for (int j=0;j<SIZE;j++)
            {
                turn=state.occupied[i][j];
                if (turn!=0)
                {
                    //colonne
                    if ((state.occupied[i][(j+3)%9]==turn || state.occupied[i][(j+6)%9]==turn) && (state.occupied[i][(j+3)%9]==0 || state.occupied[i][(j+6)%9]==0)) 
                        sum+=turn*coef; //va être compté 2 fois, exemple (0 et 3) et (3 et 0)
                    //ligne 
                    if ((state.occupied[i][(j+1)%3+(j/3)*3]==turn || state.occupied[i][(((j+2)%3)+(j/3)*3)]==turn) && (state.occupied[i][(j+1)%3+(j/3)*3]==0 || state.occupied[i][(((j+2)%3)+(j/3)*3)]==0))
                        sum+=turn*coef;
                    //diagonale '\' 
                    if (j==0||j==4||j==8)
                    {
                        if ((state.occupied[i][((j+4)%12)]==turn || state.occupied[i][((j+8)%12)]==turn) && (state.occupied[i][((j+4)%12)]==0 || state.occupied[i][((j+8)%12)]==0))
                            sum+=turn*coef;
                    }
                    //diagonale '/'
                    if (state.occupied[i][2]==turn)
                    {
                        if ((state.occupied[i][4]==turn || state.occupied[i][6]==turn) && (state.occupied[i][4]==0 || state.occupied[i][6]==0))
                            sum+=turn*coef; // compte 2 fois aussi avec le if suivant
                    }
                    if (state.occupied[i][4]==turn)
                    {
                        if ((state.occupied[i][2]==turn || state.occupied[i][6]==turn) && (state.occupied[i][2]==0 || state.occupied[i][6]==0))
                            sum+=turn*coef;
                    }
                    if (state.occupied[i][6]==turn)
                    {
                        if ((state.occupied[i][4]==turn || state.occupied[i][2]==turn) && (state.occupied[i][4]==0 || state.occupied[i][2]==0))
                            sum+=turn*coef;
                    }
                }
            }
        }

        //recherche alignements de morpions
        turn=state.avaible[i];
        if (turn!=0)
        {
            //colonne
            if ((state.avaible[(i+3)%9]==turn || state.avaible[(i+6)%9]==turn) && (state.avaible[(i+3)%9]==0 || state.avaible[(i+6)%9]==0))
                sum+=turn*coef; //va être compté 2 fois, exemple (0 et 3) et (3 et 0)
            //ligne 
            if ((state.avaible[(i+1)%3+(i/3)*3]==turn || state.avaible[(((i+2)%3)+(i/3)*3)]==turn) && (state.avaible[(i+1)%3+(i/3)*3]==0 || state.avaible[(((i+2)%3)+(i/3)*3)]==0))
                sum+=turn*coef;
            // diagonale '\' 
            if (i==0||i==4||i==8)
            {
                if ((state.avaible[((i+4)%12)]==turn || state.avaible[((i+8)%12)]==turn) && (state.avaible[((i+4)%12)]==0 || state.avaible[((i+8)%12)]==0))
                    sum+=turn*coef;
            }
        }
        //diagonale '/'
        if (state.avaible[2]==turn)
        {
            if ((state.avaible[4]==turn || state.avaible[6]==turn) && (state.avaible[4]==0 || state.avaible[6]==0))
                sum+=turn*coef;
        }
        if (state.avaible[4]==turn)
        {
            if ((state.avaible[2]==turn || state.avaible[6]==turn) && (state.avaible[2]==0 || state.avaible[6]==0))
                sum+=turn*coef;
        }
        if (state.avaible[6]==turn)
        {
            if ((state.avaible[4]==turn || state.avaible[2]==turn) && (state.avaible[4]==0 || state.avaible[2]==0))
                sum+=turn*coef;
        }
    }
    return sum;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void Node::minimax(int h, int player_turn, std::chrono::duration<double> allocated_time)
{ 
    std::chrono::system_clock::time_point chrono_start_minimax = std::chrono::system_clock::now();
    int k=exist_children(); // retourne l'indice du fils qui existe deja sinon -1
    if (k==-1) // fils non trouvé
    {   
        children_generate(player_turn); //crée la simulation
        k=child_array.size()-1; // le fils n'existait pas, maintenant qu'il est créé, k est son indice
        child_array[k]->playable_next_generate(); //actualise le vector de simulations de la génération suivante
        child_array[k]->sort_playable(); //tri
    }

    // cas d'arret
    if (h<=0 || child_array[k]->playable_next.size()<=0) // troisième condition le temps, non fonctionnelle pour le moment
    {   
        if (child_array[k]->playable_next.size()>0) //s'il reste des simulations, donc que la partie n'est pas fini on traite le transfert alpha/beta pour le cas particulier ou h vaudrait 0, résultat d'un calcul de l'horizon en fonction du temps par exemple
        {
            child_array[k]->alpha=alpha;
            child_array[k]->beta=beta; 
        }
        score=child_array[k]->eval(); // c'est this, le père (B) qui stocke le calcul du fils (C), pour pouvoir y avoir accès depuis l'étage supérieur (A) de la recursion comme étant le score du fils
    } else
    {
        score=MAXIPOINT*player_turn; //on défini un infini different selon le type noeud min/min

        for (int i=0; i<child_array[k]->playable_next.size(); ++i) 
        {   
            std::chrono::system_clock::time_point chrono_end_minimax = std::chrono::system_clock::now();
            //allocated_time -=(chrono_end_minimax-chrono_start_minimax);
            //h=child_array[k]->possible_horizon(allocated_time);
            child_array[k]->state.next_row=child_array[k]->playable_next[i].first; //on charge les coordonnées
            child_array[k]->state.next_col=child_array[k]->playable_next[i].second;
            child_array[k]->alpha=alpha; //on transmert alpha et bêta, les avoir en attribut évite de les avoir en paramètre reference et d'avoir à effectuer un calcul dans l'appel de minimax
            child_array[k]->beta=beta;
            child_array[k]->minimax(h-1, -player_turn, allocated_time);
            if (player_turn==1)
            {   //noeud min
                beta=min(beta,child_array[k]->score);
                score=min(score,child_array[k]->score);
                if (child_array[k]->free) state.free_choice_count++; // pour notre heuristique on comptabilise combien de fois un noeud (notre coup) offre l'opportunité à ses enfants (l'adversaire) de jouer librement sur toute la grille
            }
            else 
            {   //noeud max
                alpha=max(alpha,child_array[k]->score);
                score=max(score,child_array[k]->score);
            }
            if (alpha>=beta) //élagage
            {
                break;
            } 
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int tictactoe_id_row(int row,int col) //convertisseur [morpion][case], retourne la ligne dans notre système
{
    return (row/3)*3 + (col/3);
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
int tictactoe_id_col(int x,int y) //convertisseur [morpion][case], retourne la colonne dans notre système
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
void version(int (&move)[2], int row,int col) // on ne passe pas directement x et y en référence pour éviter les effets de bord dans theme qui teste version
{
    move[0] = tictactoe_id_row(row,col);
    move[1] = tictactoe_id_col(row,col);
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
void theme(int (&move)[2],int row,int col) //traduit de nos coordonnées vers celles de codingame, inverse de version
{
    bool translated=false;
    for (int i=0;i<SIZE;i++)
    {
        if (!translated)
        {
            for (int j=0;j<SIZE;j++)
            {   
                version(move,i,j); //bijection donc reversible facilement
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
    Node root = Node(); //sert à s'abstraire de l'instance courante, pour travailler dessus, la modifier, la comparer... 
    shared_ptr<Node> child0(new Node()); //instance appelante de minimax de départ
    root.child_array.push_back(child0);
    child0 = nullptr;
    shared_ptr<Node> best_child; //pour stocker la simulation qui nous interessera pour remplacer child0
    int move[2]={}; //tableau passé en paramètre des fonctions de conversion de coordonnées
    int row, col, nb_child; // variables pour eviter la desynchronisation
    bool first_turn=true; //booléen indiquant le tout premier tour de jeu
//-----------------------------------------------------------------------------------------------------------------------------------------//
    while (1) 
    {
        std::chrono::duration<double> max_time = 0.095s; //0.1sec / tour à partir du second tour, on laisse une marge d'erreur à la plate-forme
        cin >> root.child_array[0]->state.last_row >> root.child_array[0]->state.last_col; cin.ignore(); //entrées fournies par codingame, les coups (ligne/colonne) adverse
        std::chrono::system_clock::time_point chronoStart = std::chrono::system_clock::now(); //top départ chrono de notre tour après la première entrée
        version(move,root.child_array[0]->state.last_row,root.child_array[0]->state.last_col); //convertie depuis les coordonnées codingames vers notre système de coordonnées

        cin >> nb_child; cin.ignore(); //entrée codingame, le nombre de coups possibles
        for (int i = 0; i < nb_child; i++) {
            cin >>row>>col; cin.ignore(); // entrée codingame les coups possibles, on utilise notre propre génération de coups, mais on le laisse pour éviter de désynchroniser les entrées utiles (coups adverses)
        }

        // faire descendre sur l'arbre après le coup adverse
        if (move[0]!=-1 && move[1]!=-1) //si l'adversaire à joué, car au premier tour si nous avons la main, les valeurs sont -1 (ligne) et -1 (colonne)
        {   
            root.child_array[0]->state.next_row=move[0]; //on charge dans l'instance appelante les prochains coups jouables du point de vue de notre arbre (déjà joué du point de vue de codingame)
            root.child_array[0]->state.next_col=move[1];
            int opp = root.child_array[0]->exist_children(); //on cherche si on connait déjà ce coup, comme fils de notre instance courante, si oui on retourne l'indice dans le vector où il est stocké, sinon -1
            if (opp==-1) //on n'a pas trouvé le fils, ca peut être parce qu'on a pas eu le temps de le créer (si on stop minimax avec le chrono), ça fait aussi office de debug si pour une raison ou une autre le fils n'a pas bien été crée, élagué etc
            { //entrer dans ce bloc signifie perdre tout l'horizon connu de l'arbre, on génère une descendance qu'on ne connaissait pas et on s'apprete à la faire devenir l'instance courante
                root.child_array[0]->children_generate(-1); //on crée la simulation, à partir de next_row/col qui on été chargée préalablement des valeurs du coup adversaire
                opp=0; //d'indice 0 car c'est le seul élément du vector de l'instance courante
            }
            best_child=root.child_array[0]->child_array[opp]; //on charge la simulation dans un receptacle pour augmenter son nombre de références (smart pointer) avant de le transformer en instance courante
            root.child_array[0]=best_child; //échange d'instance courante de père à fils, après le coup de l'adversaire
        }

        if (first_turn && move[1]==-1) // si premier à jouer, jouer un coup particulier pour avoir une longueur d'avance sur l'horizon
        {   
            root.child_array[0]->playable_next.push_back(make_pair (4, 2)); // 4 pour morpion central, 2 pour case coin haut droite (voir matrice static de State pour les coefficients)
            first_turn=false; //on ne veut rentrer dans cette condition qu'une seule fois
            max_time = 0.98s; //au premier tour on dispose d'une seconde de temps
        }
        else if (root.child_array[0]->playable_next.size()==0) //si on ne connait aucun fils de l'objet courant, on les génère
        {
            root.child_array[0]->playable_next_generate(); //remplie le vector pair playable_next qui contient les coordonnées des simulations jouables
        }    
        root.child_array[0]->sort_playable(); //trier les simulations pour optimiser l'élagage

        // int h=root.child_array[0]->possible_horizon(max_time); // pas encore opérationnel
        int h=2; //fonctionne avec h3 mais obtient de meilleur resultat avec h2, la raison est inconnue, h4 timeout 
        root.score=-MAXIPOINT-1; //infini arbitraire, négatif car on est sur un noeud max dans l'appel du main
        root.child_array[0]->alpha=-MAXIPOINT; //-infini pour alpha de l'élagage

        for (int i=0; i<root.child_array[0]->playable_next.size(); ++i) //boucle des simulations
        {   
            root.child_array[0]->beta=MAXIPOINT; //+infini pour alpha de l'élagage, important qu'il soit dans la boucle contrairement à alpha, car c'est une boucle de noeud max
            std::chrono::system_clock::time_point chronoEnd = std::chrono::system_clock::now(); //point temporel pour calculer combien de temps s'est écoulé depuis le début du tour
            std::chrono::duration<double> allocated_time =(max_time-(chronoEnd-chronoStart))/(root.child_array[0]->playable_next.size()-i); //durée qui selon le temps accordée (1sec ou 0,1sec) va partager le temps restant en fonction du nombre de simulations
            
            root.child_array[0]->state.next_row=root.child_array[0]->playable_next[i].first; //coordonnée ligne chargée dans l'instance appelante
            root.child_array[0]->state.next_col=root.child_array[0]->playable_next[i].second; //coordonnée colonne chargée dans l'instance appelante
            root.child_array[0]->minimax(h, 1, allocated_time); //retour void, c'est le score de la simulation qui accueille le résultat
            if(root.score<root.child_array[0]->score) //on compare le score de la simulation avec le score enregistré, on ne fait pas directement un max car on a besoin de stocker l'identité de la meilleure simulation
            {
                root.score=root.child_array[0]->score; //actualise le meilleur score
                best_child=root.child_array[0]->child_array[i]; //enregistre l'adresse de la meilleure simulation
            }
        }

        root.child_array[0]=best_child; //échange de l'instance appelante de père à fils, après notre coup
        theme(move,best_child->state.last_row,best_child->state.last_col); //on traduit le coup de notre système de coordonnées vers ceui de codingame
        cout <<move[0]<<" "<<move[1]<< endl; //on joue le coup
    }
}