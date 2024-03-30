#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include<time.h>
#include <ctime>
#include <math.h>
#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

using namespace std;
#pragma GCC optimize("O3,inline,omit-frame-pointer")

// - Gestion random -
struct xorshift32_state {
    uint32_t a = time(NULL);
};

xorshift32_state xstate;

uint32_t xorshift32(struct xorshift32_state *state)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = state->a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state->a = x;
}

uint32_t xorshift()
{
    return xorshift32(&xstate);
}


// enum GameState{

//     IN_PROGRESS = -1,
//     P1 = 1,
//     P2 = 2,
//     DRAW = 0
// };

class Position{

  private:

  int x;
  int y;
  
  public:

  //contructeurs

    Position() : x(0), y(0)
    {}
  
    Position(double x, double y) : x(x), y(y)
    {}

  //getters et setters
    
    void setX(int x1){
   
        x = x1;
    }

    void setY(int y1){
        
        y = y1;
    }
    
    inline int getX() const{

        return x;
    }

    inline int getY() const{

        return y;
    }

    string print(){

        stringstream cont;
        cont << "(" << this->getX() << ", " << this->getY() << ")";
        string res = cont.str();
        return res;
    }
};


// - Classe representant une grille par un vector 1D avec les methodes associees -
template <class T>
class Grid{

  private:

    vector<T> gridContainer;

  public:
  
    // - constructeur par defaut: creer un vector de taille 1 -
    Grid(): gridContainer(vector<T>(1))
    {}

    // - constructeur parametre: creer un vector de taille specifiee en parametre -
    Grid(int taille): gridContainer(vector<T>(taille))
    {}

    // - methode renvoyant l'element a l'indice i -
    inline T &getValue(int i){

        return gridContainer[i];
    }

    // - methode renvoyant l'element a la place (i, j) (car on stocke une grille 2D sous forme de tableau en 1D) -
    inline T& getValueXY(int i, int j){
        return gridContainer[j * (int) sqrt(gridContainer.size()) + i];
    }


    // - modifier une valeur du conteneur a l'aide de son indice -
    void setValue(int i, T value){

        //if(!gridContainer.empty()) cerr << "vide ton vecteur" << endl;
        //else
        gridContainer.at(i) = value;
    }

    // - modifier une valeur du conteneur a l'aide de coo x et y -
    void setValueXY(int i, int j, T value){

        gridContainer.at(j * (int) sqrt(gridContainer.size()) + i) = value;
    }

    // - methode qui ajoute un element au conteneur -
    void ajouter(T x){

        gridContainer.push_back(x);
    }


    // - renvoie la taille du conteneur -
    inline int getSize() const{

        return gridContainer.size();
    }


};

// - classe abstraite pour "MetaMorpion" et "Morpion" -
template <class T>
class AbstractMorpion{

    // - variables membres et fonctions virtuelles (a redefinir dans les classes Morpion et MetaMorpion) -
    protected:
        
        Grid<T> container;

    public:

        //- constructeur par defaut: creer une Grid de type T et de taille 9 -
        AbstractMorpion(): container(Grid<T>(9))
        {}


        // - constructeur parametre: creer une Grid de type T et de taille passee en parametre -
        AbstractMorpion(int taille): container(Grid<T>(taille))
        {}


        //renvoi la Grid
        inline Grid<T> &getContainer(){

            return container;
        }

        virtual int checkStatus() = 0;
        virtual void print() = 0;
};

// - classe representant un Morpion classique (grille de 9 int representant le symbole present sur la case) -
class Morpion : public AbstractMorpion<int>{

  public:


    // - construit un Morpion classique de 3x3 (9 cases) avec comme valeur 0 (case vide pas encore jouee) -
    Morpion(): AbstractMorpion(9)
    {     
        for(int i = 0; i<9; i++) container.ajouter(0);
    }

    // - Methode pour afficher le contenu du morpion -
    void print(){

        cerr << "[ " << container.getValueXY(0, 0) << " " << container.getValueXY(0, 1) << " " << container.getValueXY(0, 2) << " ]" << endl;
        cerr << "[ " << container.getValueXY(1, 0) << " " << container.getValueXY(1, 1) << " " << container.getValueXY(1, 2) << " ]" << endl;
        cerr << "[ " << container.getValueXY(2, 0) << " " << container.getValueXY(2, 1) << " " << container.getValueXY(2, 2) << " ]" << endl;       
    }


    // - renvoie une liste de positions representant les cases libres jouables dans la grille -
    vector<Position> getEmptyPositions() {
      
      vector<Position> temp;
      
      for (int i = 0; i < 3; i++) {
        
        for (int j = 0; j < 3; j++) {
          
          if (container.getValueXY(i, j) == 0)           
            temp.push_back(Position(i, j));
        }
      }
      
      return temp;
    }

    // - Renvoi status du morpion -
    int checkStatus(){

        int winner;

        // - On check les lignes -
        for (int i=0; i<=2; i++){

            if(container.getValueXY(i, 0) == container.getValueXY(i, 1) && container.getValueXY(i, 1) == container.getValueXY(i, 2) && container.getValueXY(i, 0) != 0){
                
                winner = container.getValueXY(i, 0);
                return winner;
            }
        }

        // - On check les colonnes -
        for(int i=0; i<=2; i++){
            
            if (container.getValueXY(0, i) == container.getValueXY(1, i) && container.getValueXY(1, i) == container.getValueXY(2, i) && container.getValueXY(0, i) != 0){

                winner = container.getValueXY(0, i);
                return winner;
            }
        }

        // - On check la diagonale -
        if(container.getValueXY(0, 0) == container.getValueXY(1, 1) && container.getValueXY(1, 1) == container.getValueXY(2, 2) && container.getValueXY(0, 0) != 0){

            winner = container.getValueXY(0, 0);
            return winner;
        }

        // - On check l'antidiagonale -
        if(container.getValueXY(0, 2) == container.getValueXY(1, 1) && container.getValueXY(1, 1) == container.getValueXY(2, 0) && container.getValueXY(0, 2) != 0){

            winner = container.getValueXY(0, 2);
            return winner;
        }

        // - Si on arrive a ce niveau de la methode, cela veut dire que personne n'a encore gagne cette grille -

        // - Si au moins une case vide sur la grille, alors elle est encore en cours -
        for(int i=0; i<=2; i++){

            for(int j=0; j<=2; j++){

                if(container.getValueXY(i, j) == 0){

                    winner = -1;
                    return winner;
                }
            }
        }

        // - Sinon, c'est que cette grille donne sur une egalite -
        winner = 0;
        return winner;
    }
};

// - classe representant un MetaMorpion (grille de 9 Morpions classiques) -
class MetaMorpion : public AbstractMorpion<Morpion>{

  private:
    
    Position lastP;
    Position lastM;

  public:


    // - constructeur par defaut: construit une grille de 9 morpions -
    MetaMorpion(): AbstractMorpion(9)
    {}


    MetaMorpion& operator=(const MetaMorpion& t)
    {
        return *this;
    }

    void setLastP (Position p){

      lastP = p;
    }

    inline Position getLastP (){

      return lastP;
    }

    void setLastM (Position p){

      lastM = p;
    }

    inline Position getLastM (){

      return lastM;
    }

    // - Methode pour afficher le contenu du morpion -
    void print(){

        getContainer().getValueXY(0, 0).print();
        cerr << endl;
        getContainer().getValueXY(0, 1).print();
        cerr << endl;
        getContainer().getValueXY(0, 2).print();
        cerr << "--" << endl;
        getContainer().getValueXY(1, 0).print();
        cerr << endl;
        getContainer().getValueXY(1, 1).print();
        cerr << endl;
        getContainer().getValueXY(1, 2).print();
        cerr << "--" << endl;
        getContainer().getValueXY(2, 0).print();
        cerr << endl;
        getContainer().getValueXY(2, 1).print();
        cerr << endl;
        getContainer().getValueXY(2, 2).print();       
    }

    // - Fonction qui cherche les positions vides sur un morpion en particulier -
    vector<Position> getEmptyPositionSimple(){     

        vector<Position> emptyPos;
    
        vector<Position> listePosition = container.getValueXY(lastP.getX(), lastP.getY()).getEmptyPositions();
        for(int k = 0; k < listePosition.size(); k++){
            
            Position courantMove = Position(lastP.getX()*3+listePosition[k].getX(), lastP.getY()*3+listePosition[k].getY());
            emptyPos.push_back(courantMove);  
        }

        return emptyPos;
    }

    // - Fonction qui cherche les positions vides JOUABLES sur le metamorpion entier -
    vector<Position> getEmptyPositionHard(){

        vector<Position> emptyPos;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if(getContainer().getValueXY(i, j).checkStatus() == -1){
                    vector<Position> listePosition = container.getValueXY(i, j).getEmptyPositions();
                    for(int k = 0; k < listePosition.size(); k++){
                        Position courantMove = Position(i*3+listePosition[k].getX(), j*3+listePosition[k].getY());
                        emptyPos.push_back(courantMove);
                    }
                }          
            }
        }

        return emptyPos;
    }

    // - Renvoie une lsite de positions jouables sur le Metamorpion (en fonction du coup precedent) -
    vector<Position> getValidActions(){

        vector<Position> listePos;

        // - Pour gerer le cas quand le joueur joue en premier et peut jouer n'importe ou dans le board -
        if(lastM.getX()==-1){
            for(int i = 0; i<9; i++){
                for(int j = 0; j<9; j++){
                    
                    // - On liste toutes les possibilites et on les ajoute a la liste -
                    listePos.push_back(Position(i,j));
                }
            }

        // - Sinon, pendant la partie -
        }else{

            // - Cas normal, on joue dans le morpion designe par le dernier coup de l'adversaire - 
            if(getContainer().getValueXY(lastP.getX(), lastP.getY()).checkStatus() == -1){

                listePos = getEmptyPositionSimple();
            
            // - Cas complexe, on joue dans n'importe quel morpion NON-TERMINE -
            }else{

                listePos = getEmptyPositionHard();
            }
        }
        return listePos; 
    }

    // - renvoi le statut pour le metamorpion (renvoie le gagnant OU l'egalite OU -1 si non fini) -
    int checkStatus(){

        int winner;

        // - On check les lignes -
        for (int i=0; i<=2; i++){

            if(container.getValueXY(i, 0).checkStatus() == container.getValueXY(i, 1).checkStatus() && container.getValueXY(i, 1).checkStatus() == container.getValueXY(i, 2).checkStatus() && container.getValueXY(i, 0).checkStatus() != 0){
                
                winner = container.getValueXY(i, 0).checkStatus();
                return winner;
            }
        }

        // - On check les colonnes -
        for(int i=0; i<=2; i++){
            
            if (container.getValueXY(0, i).checkStatus() == container.getValueXY(1, i).checkStatus() && container.getValueXY(1, i).checkStatus() == container.getValueXY(2, i).checkStatus() && container.getValueXY(0, i).checkStatus() != 0){

                winner = container.getValueXY(0, i).checkStatus();
                return winner;
            }
        }

        // - On check la diagonale -
        if(container.getValueXY(0, 0).checkStatus() == container.getValueXY(1, 1).checkStatus() && container.getValueXY(1, 1).checkStatus() == container.getValueXY(2, 2).checkStatus() && container.getValueXY(0, 0).checkStatus() != 0){

            winner = container.getValueXY(0, 0).checkStatus();
            return winner;
        }

        // - On check l'antidiagonale -
        if(container.getValueXY(0, 2).checkStatus() == container.getValueXY(1, 1).checkStatus() && container.getValueXY(1, 1).checkStatus() == container.getValueXY(2, 0).checkStatus() && container.getValueXY(0, 2).checkStatus() != 0){

            winner = container.getValueXY(0, 2).checkStatus();
            return winner;
        }

        // - Si on arrive a ce niveau de la methode, cela veut dire que personne n'a encore gagne cette grille -

        // - Si au moins une case vide sur la grille, alors elle est encore en cours -
        for(int i=0; i<=2; i++){

            for(int j=0; j<=2; j++){

                if(container.getValueXY(i, j).checkStatus() == -1){

                    winner = -1;
                    return winner;
                }
            }
        }

        // - Sinon, c'est que cette grille donne sur une egalite -
        winner = 0;
        return winner;
    }

};


class State {

  private:
     
    MetaMorpion board;
    int playerNo;
    int visitCount;
    double winScore;

  public:

    //contructeurs
    State(): board(MetaMorpion())
    {}

    State(MetaMorpion board, int playerNo) : board(board), playerNo(playerNo), visitCount(0), winScore(0)
    {} 

    //getters et setters

    inline int getOpponent(){

      return 3-playerNo;
    }

    void setPlayerNo(int playerNum){

      this->playerNo = playerNum;
    }

    void setWinScore(int winScore){

      this->winScore = winScore;
    }

    void setBoard (MetaMorpion p){
        board = p;
    }

    inline MetaMorpion &getBoard(){
        return board;
    }

    inline int &getVisitCount (){
        return visitCount;
    }
    inline double &getWinScore(){
        return winScore;
    }

    inline int &getPlayerNo(){
        return playerNo;
    }

    void togglePlayer(){
        playerNo=3-playerNo;
    }
    
    //methodes

    void incrementVisit(){

      visitCount = visitCount + 1;
    }

    void addScore(int score){

      winScore = winScore + score;
    }
};

class Node {
private:

    
public:

    State state;       
    shared_ptr<Node> parent;
    vector<shared_ptr<Node>> childArray;

    Node() : state(State()), parent(nullptr)
    {}

    Node(State s, Node* p, vector<shared_ptr<Node>> c) : state(s), parent(p), childArray(c)
    {}
    
    Node(State s): state(s)
    {}
    
    void setParent(shared_ptr<Node> p){
        parent = p;
    }
    
    inline shared_ptr<Node> &getParent(){
        return parent;
    }
    
    inline State &getState(){
        return state;
    }
    
    inline vector<shared_ptr<Node>> &getChildArray(){
        return childArray;
    }


    //methodes

    shared_ptr<Node> &getRandomChildNode(){
        
        int random = xorshift()%childArray.size();
        return childArray[random];
    }

    // - Renvoie le noeud fils ayant le meilleur score, si plusieurs noeuds, en choisi un au hasard -
    shared_ptr<Node> &getChildWithMaxScore(){
        
        int rep=0;
        double scoreTemp=-1;

        bool doublon = false;
        vector<int> listeNoeudsDoublons;

        for (int i = 0; i < childArray.size(); i++){

            double quotient = childArray[i]->getState().getWinScore()/childArray[i]->getState().getVisitCount();
            
            if (quotient > scoreTemp){

                listeNoeudsDoublons.clear();
                listeNoeudsDoublons.push_back(i);

                rep = i;
                scoreTemp = quotient;
            
            }else if(quotient == scoreTemp && scoreTemp != -1){
                if(!doublon) doublon = true;
                listeNoeudsDoublons.push_back(i);
            }
        }

        if(!doublon) return childArray[rep];
        else return childArray[listeNoeudsDoublons[xorshift()%listeNoeudsDoublons.size()]];
    }

    // - Creer une liste de state representant tous les coup possibles a partir du noeud actuel -
    vector<State> getAllPossibleStatesEnhanced(){

        vector<State> tempStateArray;
        
        // - On liste les coups possibles au tour suivant -
        vector<Position> listePossiblePlays = state.getBoard().getValidActions();

        // - Pour chacun de ces coups -
        for(int i = 0; i < listePossiblePlays.size(); i++){
            
            // - On creer une copie de notre plateau courant -
            MetaMorpion boardForCurrentPlay = state.getBoard();

            // - On lui ajoute notre coup -
            boardForCurrentPlay.getContainer().getValueXY(listePossiblePlays[i].getX()/3, listePossiblePlays[i].getY()/3).getContainer().setValueXY(listePossiblePlays[i].getX()%3, listePossiblePlays[i].getY()%3, 3 - state.getPlayerNo());
            
            // - On met a jour les variables membres -
            boardForCurrentPlay.setLastM(Position(int (listePossiblePlays[i].getX()/3), int (listePossiblePlays[i].getY()/3)));
            boardForCurrentPlay.setLastP(Position(listePossiblePlays[i].getX()%3, listePossiblePlays[i].getY()%3));

            // - On creer un state associe a ce board modifie, et on l'ajoute a la liste de state -
            State currentState = State(boardForCurrentPlay, 3 - state.getPlayerNo());
            tempStateArray.push_back(currentState);
        }

        // - On renvoie cette liste de state -
        return tempStateArray;
    }
};


  //-------------------------SELECTION--------------------------------


    shared_ptr<Node> findBestNodeWithUCT(shared_ptr<Node> node) {
        
        // - On liste tous les fils de node -
        vector<shared_ptr<Node>> list = node->getChildArray();

        int rep=0;
        double uctTemp = 0;
        double uctCurrent;

        // - On stocke ici une partie couteuse du calcul -
        double logTotalVisit = log(node->getState().getVisitCount());

        // - Pour chaque fils -
        for (int i = 0; i < list.size(); i++){
            
            
            // - Si le noeud n'a jamais ete visite , il faut le visiter en priorite -
            if (list[i]->getState().getVisitCount() == 0){

                rep = i;
                break;

            // - Sinon -
            }else{

                // - On stocke une deuxieme partie couteuse du calcul -
                double sqrtLogRes = sqrt(logTotalVisit / (double) list[i]->getState().getVisitCount());

                // - On amorce le reste du calcul de l'UCT -
                uctCurrent = ((double) list[i]->getState().getWinScore() / (double) list[i]->getState().getVisitCount()) + 1.41 * sqrtLogRes;

                // - Si on trouve une valeur plus elevee que la plus elevee jusqu'alors, elle devient la nouvelle valeur plus elevee -
                if(uctCurrent > uctTemp){
                
                    rep = i;
                    uctTemp = uctCurrent;
                }
            }
        }

        return list[rep];
    }
 

    shared_ptr<Node> selectPromisingNode(shared_ptr<Node> rootNode) {

        shared_ptr<Node> node = rootNode;

        while (node->getChildArray().size() != 0) {

            node = findBestNodeWithUCT(node);
        }

        return node;
    }


//-------------------------EXPANSION--------------------------------

    // - on etend le noeud choisi en lui creant autant de noeuds fils que de possibilite de jeu -
    inline void expandNode(shared_ptr<Node> node) {
        
        // - on liste les states suivant a partir du noeud courant -
        vector<State> possibleStates = node->getAllPossibleStatesEnhanced(); 

        // - on les parcours tous pour ainsi creer des noeuds fils -
        for(int i = 0; i < size(possibleStates); i++){
            
            shared_ptr<Node> newNode = make_shared<Node>(possibleStates[i]);

            newNode->setParent(node);
            node->getChildArray().push_back(newNode);
        }
    }

//-------------------------SIMULATION------------------------------


// - On simule une partie entiere a partir du noeud choisi et on voit quel est le resultat de celle-ci au final -
int simulateRandomPlayout(shared_ptr<Node> node) {

    // - On initialise le statut actuel du noeud, statut que l'on va actualiser en simulant la partie petit a petit -
    MetaMorpion boardAJouer = node->getState().getBoard();
    
    int boardStatus = boardAJouer.checkStatus();
    int player = node->getState().getPlayerNo();

    while (boardStatus == -1) {

        // - On change le joueur qui va jouer, et on genere un coup aleatoire -   
        player = 3-player;

            // - On liste les coups possible au tour courant et on en choissi un au hasard -
            vector<Position> possiblePlays = boardAJouer.getValidActions();
            Position move = possiblePlays[xorshift()%possiblePlays.size()]; 

            // - On place ce coup sur la grille et on update les variables associees -
            boardAJouer.getContainer().getValueXY(move.getX()/3, move.getY()/3).getContainer().setValueXY(move.getX()%3, move.getY()%3, player);
            boardAJouer.setLastM(Position(int (move.getX()/3), int (move.getY()/3)));
            boardAJouer.setLastP(Position(move.getX()%3, move.getY()%3));

        // - On update la variable status -
        boardStatus = boardAJouer.checkStatus();
    }

    // - on renvoie le statut de la partie -
    return boardStatus;
}

//----------------------BACKPROPAGATION----------------------------

void backPropagation(shared_ptr<Node> nodeToExplore, int playerNo, int playoutResult) {

    shared_ptr<Node> tempNode = nodeToExplore;
    // - boucle tant que le parent du noeud actuel n'est pas null  -
    while (tempNode->getParent() != nullptr) {

        // - On incremente le nbr de visite du noeud -
        tempNode->getState().incrementVisit();

        // - Et si c'est un noeud avec une partie ou l'on gagne, on augmente le score de victoires -
        if (playerNo == playoutResult) {

            tempNode->getState().addScore(2);
        }
        
        // - On remonte dans l'arbre -
        tempNode = tempNode->getParent();
    }
}


void MonteCarloTreeSearch(shared_ptr<Node> rootNode, int playerNo) {

    shared_ptr<Node> promisingNode = selectPromisingNode(rootNode);
    
    //cerr<<"Selection: Reussie "<< endl;

    int status = promisingNode->getState().getBoard().checkStatus();
    shared_ptr<Node> nodeToExplore = promisingNode;
    int playoutResult;

    // - si la partie dans ce noeud prometteur est encore en cours -
    if (status == -1) {
        
        // - on expand le noeud prometeur en lui creant autant de noeud fils que de coup jouable par notre bot -
        expandNode(promisingNode);

        //cerr<<"Expansion: Reussie "<<endl;
    
        // - si le noeud prometteur a un nombre de noeuds fils non nul -        
        if (promisingNode->getChildArray().size() > 0) {
            
            // - on selectionne un des noeuds fils (crees lors de l'expansion) au hasard -
            nodeToExplore = promisingNode->getRandomChildNode();
        }


        // - On simule une partie entiere au hasard sur le noeud choisi -
        
        playoutResult = simulateRandomPlayout(nodeToExplore);

        //cerr<<"Simulation: Reussie "<<endl;
    }else{
        
       //cerr << "Loser promissing node , skipping Simulation" << endl;
       nodeToExplore->getState().setWinScore(-1);
       playoutResult = -1;
    }

    // - Et on remonte l'arbre pour actualiser les stats des noeuds en fonction du resultat de la partie simulee -
    backPropagation(nodeToExplore, playerNo, playoutResult);

    //cerr << "BackPropagation: Reussie " <<endl;
    //cerr << " = = = = = = = = = = = " << endl;
}


int main()
{

    // - section lancee une fois (avant le debut du jeu) - 

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::duration<long, std::milli> elapsed;

    // - On initialise notre noeud racine -
    shared_ptr<Node> root = make_shared<Node>();

    root->getState().setPlayerNo(2);
    root->getState().getBoard().setLastM(Position(-1,-1));
    root->getState().getBoard().setLastP(Position(-1,-1));

    int playerNo=2;
    int turn = 0;

    // - boucle de jeu (relancee a chaque fois que c'est notre tour) -
    while (1) {

        // - recuperation des entrees -
        int opponent_col;
        int opponent_row;
        cin >> opponent_row >> opponent_col; cin.ignore();
        
        int valid_action_count;
        cin >> valid_action_count; cin.ignore();
        for (int i = 0; i < valid_action_count; i++) {
            int row;
            int col;
            cin >> row >> col; cin.ignore();
        }

        
        // - On execute ce code qu'on on est pas de le cas "on commence au premier tour" -
        if(opponent_col != -1 || opponent_row != -1){
            
            // - On "convertit" les coordonnees recues en entrée pour pouvoir les utiliser dans nos structures de données -
            int morpionDesigneX = opponent_row / 3;
            int morpionDesigneY = opponent_col / 3;
            int caseDesigneX = opponent_row % 3;
            int caseDesigneY = opponent_col % 3;
 
            // - on avance dans les fils du root dans l'arbre et on cherche si on a deja simulé le coup que l'adversaire vient de faire (pour gagner du temps et memoire) -          
            int i = 0;
            bool found = false;

            while(i < root->getChildArray().size() && !found){
                // - On cherche le noeud dont le dernier morpion joué correspond au morpion ou a joué l'adversaire -
                if(root->getChildArray()[i]->getState().getBoard().getLastM().getX() == morpionDesigneX && root->getChildArray()[i]->getState().getBoard().getLastM().getY() == morpionDesigneY){
                    // - Une fois un noeud avec le bon morpion trouvé, il faut regarder si c'est le bon coup -
                    if(root->getChildArray()[i]->getState().getBoard().getLastP().getX() == caseDesigneX && root->getChildArray()[i]->getState().getBoard().getLastP().getY() == caseDesigneY && root->getChildArray()[i]->getState().getBoard().getContainer().getValueXY(morpionDesigneX, morpionDesigneY).getContainer().getValueXY(caseDesigneX, caseDesigneY) == (3-playerNo)){

                        // - On a trouvé le bon noeud généré donc on avance sur lui -
                        root = root->getChildArray()[i];
                        found = true;
                    }
                }

                i++;
            }

            // - Si on n'a pas trouve de noeud correspondant, on le creer (on ne rentre quasiment pas ici, au vu du nombre de MCTS, tous les noeuds sont deja crees) -  
            if(!found){
                cerr << "creation" << endl;
                // - on modifie le root et en ajoutant le nouveau coup de l'adversaire -
                root->getState().setPlayerNo(3-playerNo);
                root->getState().getBoard().getContainer().getValueXY(morpionDesigneX, morpionDesigneY).getContainer().setValueXY(caseDesigneX, caseDesigneY, root->getState().getPlayerNo());
                root->getState().getBoard().setLastP(Position(caseDesigneX, caseDesigneY));
                root->getState().getBoard().setLastM(Position(morpionDesigneX, morpionDesigneY));

                //on supprime les fils du root, ils ne sont plus representatifs de la suite de la partie
                if(root->getChildArray().size() > 0){
                    for(int i = 0; i < root->getChildArray().size(); i++){
                        root->getChildArray().erase(root->getChildArray().begin()+i);
                    }
                }
            }        
        
            // - On lance une boucle de MCTS en fonction du temps qu'il reste et du numero de tour -
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::chrono::duration<long, std::milli> elapsed;
            
            // - On choisi le temps de la boucle en fonction de si c'est notre premier tour ou pas -
            int tps;
            
            if(turn == 0){
                
                tps=1000; 
                turn++;    

            }else tps=90;
            

            int cpt = 0;
            do{
        
                MonteCarloTreeSearch(root, playerNo);
                cpt++;

                end = std::chrono::steady_clock::now();
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            }while(elapsed.count() < tps);    //enlever le commentaire si timeout


            cerr << "Nbr MCTS fait: " << cpt << endl;
        

            // - On cherche le noeud avec le meilleur "score" (nbrVictoires/nbrVisites) et il devient la nouvelle racine -
            root = root->getChildWithMaxScore();


            // - On convertit nos coordonnees de sortie de facon a s'accorder a la facon dont Conding Game les attends -
            int morpionSortieX = root->getState().getBoard().getLastM().getX();
            int morpionSortieY = root->getState().getBoard().getLastM().getY();
            int caseSortieX = root->getState().getBoard().getLastP().getX();            
            int caseSortieY = root->getState().getBoard().getLastP().getY();
            int cooFinaleX = morpionSortieX * 3 + caseSortieX;
            int cooFinaleY = morpionSortieY * 3 + caseSortieY;

            // - on renvoie sur la sortie standard les valeurs de case a jouer choisies -
            cout<<cooFinaleX<<" "<<cooFinaleY<<endl;


        // - TOUR 1 si on commence: ON a 1000ms pour faire du mcts et jouer un coup -
        }else{

            playerNo=1;

            do{
                MonteCarloTreeSearch(root, playerNo);

                end = std::chrono::steady_clock::now();
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            }while(elapsed.count() < 2900);

            // - On joue en fonction du meilleur noeud fils calcule avec les multiples MCTS -
            root = root->getChildWithMaxScore();
            int aJouerX = (root->getState().getBoard().getLastM().getX()) * 3 + root->getState().getBoard().getLastP().getX();
            int aJouerY = (root->getState().getBoard().getLastM().getY()) * 3 + root->getState().getBoard().getLastP().getY();
            
            turn++;
            cout<<aJouerX<<" "<<aJouerY<<endl;        }
    }
    
    return 0;    
}