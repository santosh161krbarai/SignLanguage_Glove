# include<stdlib.h>
# include <iostream>

using namespace std;

class node {
    int value;
    node *next;
public :
    node(int value){

        this->value=value;
        this->next=NULL;
    }
   void print(){
    cout << value << endl;
   }
};

int main(){
    node Node(11);
    Node.print();
}
