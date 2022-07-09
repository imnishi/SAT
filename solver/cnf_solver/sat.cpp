#include <bits/stdc++.h>
using namespace std;

//Defining macros
#define rep(i,n) for(int (i)=0;(i)<(n);(i)++)
#define pb push_back
#define all(v) v.begin(),v.end()
#define ll long long int
#define vll vector<int>

//Defining Data-Structures for trees
typedef struct node{
    vector<vector<int> > list;          //vector of vectors to store all lists of literals
    vector<int> tta;                    //stores the truth value of propositions
    int check;                          //used to check satisfiability of tree
    node *left;                         //pointer to left child
    node *right;                        //pointer to right child
    node *parent;                       //pointer to parent
} node;

//Some Global Variables
bool sat;                               //Global sat which is true if we fnd a satisfiable solution otherwise false
vector<int> solprop;                    //This vector stores the possible final solution
int prop;                               //Total number of propositions to be solved
int conflicts;							//to track the number of conflicts during propogation.

// Defining the functions used
node* unit_propagation(node *tree,int p);//Simplifies the node assuming p is true
int best_poss(node *tree);              //This function return the best possible integer which can be removed from the tree-clauses
void make_branch(node *tree , int p);   //Makes the left and right branch of tree
void sat_solver(node *tree);            //Main function which solves sudoku
void pre_process(node *tree);           //For pre-processing the input before sending to sat-solver




int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	
	ifstream infile;
	infile.open("dimacs.txt");

	char ch;
	string s;
	int p,c,k;
	infile>>ch;

	while(ch!='p'){
		getline(infile,s);
		infile>>ch;
	}


	infile>>s;
	infile>>prop>>c;
	vector<vector<int>> clauses;
	rep(i,prop) solprop.pb(0);

	int trueclause=0;

	//Taking the input as a vector of clauses and each clause is a vector of propositions
	for(int i=0;i<c;i++){	
		vector<int> v;
		infile>>k;
		trueclause=0;
		while(k!=0){
			if(find(v.begin(), v.end(), -k) != v.end()){
				//k and -k present in the same clause . thus this clause is always true.
				getline(infile,s);
				k=0;
				trueclause=1;
				continue;
			}
			if(find(v.begin(), v.end(), k) != v.end()){
				//repeated literals are not added
				infile>>k;
				continue;
			}

			v.push_back(k);
			
			infile>>k;
		}


		//if clause is of size 1, it is incorporated into solprop directly and if a conflict
		//occours, Not possible is printed. (unsat).
		if(v.size()==1){
			if(solprop[abs(v[0])-1]==0){
				solprop[abs(v[0])-1] = (v[0]/abs(v[0]));
			}
			else{
				if(solprop[abs(v[0])-1] != v[0]/abs(v[0])){
					cout<<"UNSAT"<<"\n";
					return 0;
				}
			}
		}

		else{
			if(trueclause==0){
				sort(v.begin(),v.end());
				clauses.push_back(v);
			}
		}
	}

	infile.close();

	node *tree = new node;                  //We create a node in which we store the propositions and the possible truth values and nodes for left and right child and parent and check for unsatisfiability
    tree->check = 0;
	tree->parent = NULL;
    tree->list = clauses;
	
	pre_process(tree);

	conflicts=0;
    sat = false;

	sat_solver(tree);
	free(tree);

	//printing output
	if(sat==true){
		cout<<"SAT"<<"\n";
		rep(i,prop){
            if(solprop[i]!=0) cout<<solprop[i]*(i+1)<<" ";
            else cout<<(i+1)<<" ";
		}
		cout<<"\n";
	}
	else cout<<"UNSAT"<<"\n";
	return 0;

}

void pre_process(node *tree){
	//To process the clauses according to solprop vector
	//if a true proposition is present, then that particular clause is removed
	//if a false proposition is present, then that proposition is skipped.
	//newly formed single sized clauses are removed and added to the solprop as true literals.
	vector<int> nsol;
	nsol.pb(0);
	while(nsol.size()>0){
		nsol.clear();
		vector<vector<int>> inp;
		rep(i,tree->list.size()){
			vector<int> try1;
			rep(j,tree->list[i].size()){
				if(tree->list[i][j] == solprop[abs(tree->list[i][j])-1]*abs(tree->list[i][j])){
					try1.clear();
					break;
				}
				if(tree->list[i][j] == (-1)*solprop[abs(tree->list[i][j])-1]*abs(tree->list[i][j])){
					continue;
				}
				else try1.pb(tree->list[i][j]);
			}
			if(try1.size()==0) continue;
			if(try1.size()==1){
				solprop[abs(try1[0])-1] = (try1[0]/abs(try1[0]));
				nsol.pb(0);
			}
			else{
				inp.pb(try1);
			}
		}
		tree->list=inp;
	}
	
    //To remove duplicate clauses:

	vector<vector<int>> inp;
    sort(tree->list.begin(),tree->list.end());//To remove duplicate elements we sort accoring to first index
    rep(i,tree->list.size()){
        if(i==0){
            inp.pb(tree->list[i]);           //We insert first element
            continue;
        }
        if(tree->list[i]==tree->list[i-1]) continue;//If both vectors are same then we don't insert
        //As elements are sorted according to 1st element so if 2 vectors will be equal they will have common first element
        inp.pb(tree->list[i]);                      //else we insert
    }
    tree->list = inp;                               //We assign tree->list to input
    tree->tta = solprop;
    return;
}

void sat_solver(node *tree){
    if((sat==true)||(tree->check == 2))    //If we find a solution or we find a contradiction we return
        return;
    int p = best_poss(tree);               //We find the integer which makes the solving easiest
    if((sat==true)||(tree->check ==2))     //If we find a solution or we find a contradiction we return
        return;
    make_branch(tree,p);                   //We try for solutin assuming p to true in left node and it to be false in right node

    sat_solver(tree->left);                //Solving sudoku assuming p to be true
    free(tree->left);
    if(sat==true)                          //If we donot a solution return
        return ;
    sat_solver(tree->right);               //If assuming p to be true doesnot give a solution i.e. left node we assume p to be false and solve right node
    free(tree->right);
}

void make_branch(node *tree , int p){
	// making right and left branch for the tree
    node *rtree = new node;                 
    node *ltree = new node;                 
    tree->left = ltree;                     
    tree->right = rtree;                    
    ltree->parent = tree;                   
    rtree->parent = tree;                   
    ltree->check = 0;                       //Assuming a solution exists for left node till now
    rtree->check  = 0;                      //Assuming a solution exists for right node till now

    //For creation of left branch where p is true
    ltree->tta = tree->tta;                 //The assigned values to propositions in parent node must be satisfied in left node
    ltree->tta[p-1] = 1;                      //Assuming p to be true for left tree
    ltree->list = tree->list;               //The list of literals for left tree is same as for parent tree

    //If p is true so -p is false

    unit_propagation(ltree,p);             //Now we propogate in set of literals and minimise number of literals
                                           //until no literal of size 2 is left or size of set of literals is 0


    //For creation of right branch where p is false
    rtree->tta = tree->tta;               //The assigned values to propositions in parent node must be satisfied in right node
    rtree->tta[p-1] = -1;                   //Assuming p to be false for right tree
    rtree->list = tree->list;             //The list of literals for left tree is same as for parent tree

    unit_propagation(rtree,-p);           //Now we propogate in set of literals and minimise number of literals
                                          //until no literal of size 2 is left or size of set of literals is 0
    return;
}

node* unit_propagation(node *tree,int p){       
    //Here, we propagate into tree assuming p is true and further simplifying the clauses
	//The clauses of size 2 containing -p are first broken down...the other literal has to be true because -p is false.
	//this other literal is stored in a new vector chan1 and the tree is further simplified recursively taking literals in chan1 as true.
	//in this process a conflict may occour and funcn is returned.

	vector<vector<int> > lisfin;
    if(p<0){
        if(tree->tta[-p-1]==1){
            conflicts++;
            tree->check=2;
            return tree;
        }
        tree->tta[-p-1] = -1;
    }
    else{
        if(tree->tta[p-1]==-1){
            conflicts++;
            tree->check=2;
            return tree;
        }
        tree->tta[p-1] = 1;
    }
    vector<int> chan1;

    rep(i,tree->list.size()){//For clauses containg -p, with size 2, the other literal has to be true
        if((tree->list[i].size()==2)&&(binary_search(all(tree->list[i]), -p ))){
                int tr12 = tree->list[i][0];
                if(tr12==-p) tr12 = tree->list[i][1];
                chan1.pb(tr12);
        }
    }

    rep(i,chan1.size()){
        if(chan1[i]<0){
            if(tree->tta[(-1)*chan1[i] - 1]==1){
                    conflicts++;
                    tree->check = 2;
                    return tree;
            }
            tree->tta[chan1[i]*(-1) - 1]= -1;
        }
        else{
            if(tree->tta[chan1[i]-1]==-1){
                    conflicts++;
                    tree->check = 2;
                    return tree;
            }
            tree->tta[chan1[i]-1]=1;
        }
    }
    rep(i,tree->list.size()){
        if(binary_search(all(tree->list[i]),-p))
            if(tree->list[i].size()==2) continue;
        vector<int> inp;//For putting in the input removing p
        rep(j,tree->list[i].size()) if(tree->list[i][j]!=-p) inp.pb(tree->list[i][j]);
        //If p is false so -p is true so we remove those cases
        if (binary_search(all(inp), p )) continue;
        lisfin.pb(inp);
    }
    tree->list = lisfin;
    rep(i,chan1.size()){
        tree = unit_propagation(tree,chan1[i]);
    }
    return tree;

}

int best_poss(node *tree){                                        //Returns the proposition to evalauate further with best propagation
    //This function actually returns the proposition which is present most favourably for further tree propogation.
	//when a proposition is present in a clause of size 2, it treated twice as favourable as present elsewhere.
	//Also, if the clauses are finished, then the satisfiablity is true and solprop is returned.
	//Also, if the freq of all propositions present is atmost 1, then they are directly added to solprop and satisfiablity is true.
	
	
    if(tree->list.size()==0){                                     //If no clauses are left then we have satisfiability and we return
        solprop = tree->tta;                                          //We assign tree-> truth table to final solution
        sat = true;
        tree->check = 2;
        return 0;
    }
    int freq[prop+1] = {0};                                       //To calculate frequency of all propositions
    int freqmax = 0;                                              //proposition corresponding to max frequency element
    int doufreq[prop+1] = {0};                                    //To count the number of literals in which ith literal has 2 size in one of the clauses
    int best  =  0;                                               //It stores the best proposed proposition for evalauation
    rep(i,tree->list.size()){
        rep(j,tree->list[i].size()){
            int a = tree->list[i][j];                              //We increase the frequency of the proposition
            if(a<0) a*=(-1);
            freq[a]++;
            if(tree->list[i].size()==2){                           //We increase doufreq by 2 of ith proposition if proposition occurs in literal of freq 2
                doufreq[a]+=2;
            }
        }
    }
    rep(i,prop+1){
        if(freq[freqmax]<freq[i]) freqmax = i;                     //If double freq +freq of any prop is max then we select it as best possible proposition for evaluation
        if((doufreq[best]+freq[best])<(doufreq[i]+freq[i])) best = i;
    }
    int flag  = 0;
    if(freq[freqmax]==1){                           //If maximum freq is 1
        rep(i,tree->list.size()){
            if(flag==1) break;
            flag = 1;
            rep(j,tree->list[i].size()){
                int t = tree->list[i][j];          
                if(t<0){
                    t *= (-1);
                    if(tree->tta[t-1]==0||tree->tta[t-1]==-1){
                            flag = 0;              //First place where the literal can be made satisfiable tta of proposition is assigned false and proposition is made false
                            tree->tta[t-1] = -1;
                            break;
                    }
                }
                else{
                    if(tree->tta[t-1]==0||tree->tta[t-1]==1){
                            flag = 0;              //First place where the literal can be made satisfiable tta of proposition is assigned false and proposition is made true
                            tree->tta[t-1] = 1;
                            break;
                    }
                }
            }
        }
        if(flag==1){
            conflicts++;
            tree->check = 2;                    //If assigned all propositions any possible value cannot satisfy the literals then no solution exists
        }
        else{
            rep(i,solprop.size()) solprop[i] = tree->tta[i];   //Otherwise we have obtained a solution
            sat = true;
        }
        return 0;
    }
    return best;                               //Return the most favourable proposition
}