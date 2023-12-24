#include <iostream>
#include <fstream>
#include <gmp.h>

using namespace std;

string mpzToString(mpz_t in){
    string ans;
    mpz_t wholeString;
    mpz_t oneChar;
    mpz_init(wholeString);
    mpz_init(oneChar);
    mpz_set(wholeString, in);
    mpz_set_ui(oneChar, 0);

    while (mpz_cmp_ui(wholeString, 0) > 0){
        mpz_mod_ui (oneChar, wholeString, 1000);
        mpz_tdiv_q_ui (wholeString, wholeString, 1000);
        if (mpz_get_si(oneChar) == 999){
            break;
        }
        char c =  mpz_get_si(oneChar)-100;
        ans = c + ans;
    }
    return ans;
}


int main(){
    string compressedFile;
    int N;
    cout<<"Give name of an compressed file, including file extension .ins. This file MUST be in the same folder as this program\n";
    cin>>compressedFile;
    ifstream compressed (compressedFile);
    string fileName;
    compressed>>fileName;
    ofstream deCompressed ("test.txt");
    compressed>>N;
    cout<<N<<endl;
    int n, r;
    while (compressed>>n>>r){
        cout<<"\n";
        mpz_t radicand;
        mpz_init(radicand);
        mpz_set_ui(radicand,n);
        mpz_t scaleUsable;
        mpz_init(scaleUsable);
        mpz_set_ui(scaleUsable, 1);
        mpz_ui_pow_ui(scaleUsable, 10, r*(3*N-1));
        mpz_mul(radicand, radicand, scaleUsable);
        mpz_root(radicand, radicand, r);
        mpz_out_str(stdout, 10, radicand);
        deCompressed <<mpzToString(radicand);
    }

    compressed.close();
    deCompressed.close();

}
