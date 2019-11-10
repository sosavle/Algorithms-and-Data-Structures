//~Luis Sosa Villegas

#include <iostream>
using namespace std;
struct IntVector{
 protected:
    int size;
    int capacity;
    int* data;

 public:
    IntVector(){
        capacity = 0;
        size = 0;
        data = NULL;
    }
    
    // ADDED: Copy constructor 
    // Used in factorial function to instantiate "Result" from "Current"
    // Because otherwise they would be pointing to the same data
    IntVector(const IntVector& source){
        capacity = source.capacity;
        size = source.size;
        data = new int[capacity];
        for(int i=0;i<size;i++){
            data[i] = source.data[i];
        }
    }
    
    double back(){
        if(size<=0){
            cerr << "Error: attempted back() on empty vector!";
            exit(1);
        }
        else{
            return this->data[size-1];
        }
    }
    
    void push_back(int dat){
        if(size>=capacity){
            resize(size*2+1);
        }
        data[size] = dat;
        ++size;
    }
    
    void pop_back(){
        if(size<=0){
            cerr << "Error: attempted pop_back() on empty vector!";
            exit(1);
        }
        --size;
    }
    
    void resize(int newCap){
        int* temp = data;
        capacity = newCap;
        data = new int[capacity]();
        for(int i=0;i<size;i++){
            data[i] = temp[i];
        }
        delete[] temp;
    }
    
    //Modified to return stretch the vector and return zero if
    //asked for an out-of-range index (used in multiplication).
    int& operator[](int position){
        if(position<0){ 
            cerr << "Error: attempted access on negative index";
            exit(1);
        }
        else if(position>=size){
            for(int i=size; i<=position; ++i){
                push_back(0);
            }
        }
        return data[position];
    }
    
    int getSize(){
        return size;
    }
    
    void clear(){
        size = 0;
    }
}; 

//////////////////
// THE NEW STUFF 
// -- BIG INTS
/////////////////

class BigInt{
   protected:
    IntVector digits;
    int size = 0;
    int zero = 0;
   
   public:
     //Shortcut for BigInt::digits.push_back
    void push_back(int x){
       digits.push_back(x);
       ++size;
   }
   
    //Constructor
    BigInt(string input=""){
        int length = input.length();
        for(int i=length-1; i>=0; --i){
            int digitInt = input[i]-'0';
            push_back(digitInt);
       }
    }
    
    //Reverses digits to output format
    void reverse(){
       for(int i = 0; i<size/2; ++i){
           int temp = digits[i];
           digits[i] = digits[size-1-i];
           digits[size-1-i] = temp;
       }
   }
    
    //Shortcut for BigInt::digits.resize 
    void set_size(int s){
        size = s;
        digits.resize(s);
    }
    
    // Prints the BigInt
    void print(){
        for(int i=size-1; i>=0; --i){
            cout<<digits[i];
        }
    }

    //Shortcut for BigInt::digits.pop_back 
   void pop_back(){
       digits.pop_back();
       --size;
   }
   
   //Shortcut for BigInt::digits[]
   int& operator[](int x){
           return digits[x];
            set_size(x);
   }
   
   //Removes leading zeros
   void remove_zeros(){
       for(int i=size-1; i>0; --i){
           if(digits[i]==0){
               pop_back();
           }
           else{
               break;
           }
       }
   }
   
   //Refactors a BigInt, dealing with "digits" that are > 9
   //  -Used in multiplication
   BigInt digitize(int s){
       BigInt digitizedInt;
       digitizedInt.set_size(s);
       int carry = 0;
       
       for(int i=0; i<size; ++i){
           if(digits[i]+carry>=10){
               digitizedInt[i] = (digits[i]+carry)%10;
               carry = (digits[i]+carry)/10;
           }
           else{
              digitizedInt[i] = digits[i]+carry; 
              carry = 0;
           }
       }
       digitizedInt[0] += carry;
       return digitizedInt;
   }
   
   // PLUS
   BigInt operator+(BigInt x){
       BigInt result;
       int rSize = max(size,x.size);
       int carry = 0;
    
       for(int i=0; i<rSize; ++i){
           int sum = this->digits[i] + x[i] + carry;
           if(sum<10){
               result.push_back(sum);
               carry = 0;
           }
           else{
               result.push_back(sum%10);
               carry = sum/10;
           }
       }
       if(carry!=0){
           result.push_back(carry);
       }
    return result;
   }
   
   // MINUS
   BigInt operator-(BigInt x){
       BigInt result;
       int rSize = max(size,x.size);
       int carry = 0;
    
       for(int i=0; i<rSize; ++i){
           int substraction = this->digits[i] - x[i] - carry;
           if(substraction>=0){
               result.push_back(substraction);
               carry = 0;
           }
           else{
               result.push_back(substraction + 10);
               carry = 1;
           }
       }
    result.remove_zeros();
    return result;
   }
   
   // TIMES
   BigInt operator*(BigInt x){
        BigInt result;
        result.set_size(size + x.size);
    
       for(int i=0; i<size; ++i){
           for(int j=0; j<x.size; ++j){ 
               int r = x.digits[j]*this->digits[i];
                   result[j+i] += r;
            }
       }
    BigInt q;
    q = result.digitize(size + x.size);
    q.remove_zeros();
    return q;
   }
   
  // FACTORIAL
  BigInt operator!(){
        BigInt one("1");
        BigInt result = *this;
        BigInt current = (result - one);
        while(current.size>1 || current[0] > 1){
            result = (result*(current));
            current = (current-one);
        }
        return result;
   }
};

int main(){
    string operation;
    string a;
    string b;
    BigInt result;
    
    cout << "Welcome to the BigInt calculator.\n";
    
    cout << "\nPlease enter operand A: ";
    cin >> a;
    BigInt first(a);
    
    cout << "\nPlease enter an operation to perform: ";
    cin >> operation;
    if(operation == "!"){
        result = !first;
        first.print(); cout << "! = "; result.print(); cout << endl;
        exit(0);
    }
    
    cout << "\nPlease enter operand B: ";
    cin >> b;
    BigInt second(b);
    
    
    if(operation == "+"){
        result = first+second;
    }
    else if(operation == "-"){
        result = first-second;
    }
    else if(operation == "*"){
        result = first*second;
    }
    else{
        cout << "The operation " << operation << " is not supported\n";
        exit(1);
    }
    first.print(); cout << " " << operation << " "; second.print(); cout << " = "; result.print(); cout << endl;
