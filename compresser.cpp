#include <iostream>
#include <fstream>
#include <gmp.h>
#include <set>

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
        char c =  mpz_get_si(oneChar)-100;
        ans = c + ans;
    }
    return ans;
}

unsigned int calcInitBottom (mpz_t in, int scale){
    mpz_t result;
    mpz_init(result);
    mpz_set(result, in);
    mpz_mul(result, result, result);
    mpz_t scaleUsable;
    mpz_init(scaleUsable);
    mpz_set_ui(scaleUsable, 1);
    mpz_ui_pow_ui(scaleUsable, 10, 2*(scale-1));
    mpz_fdiv_q(result, result, scaleUsable);
    return mpz_get_si(result);
}

bool checkSuccsfulXR (mpz_t x, int r, int scale, mpz_t goal){
    mpz_t radicand;
    mpz_init(radicand);
    mpz_set(radicand,x);
    mpz_t scaleUsable;
    mpz_init(scaleUsable);
    mpz_set_ui(scaleUsable, 1);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(radicand, radicand, scaleUsable);
    mpz_root(radicand, radicand, r);
    return mpz_cmp(radicand, goal) == 0;
}

bool isXGreater(mpz_t x, int r, int scale, mpz_t goal){
    mpz_t radicand;
    mpz_init(radicand);
    mpz_set(radicand,x);
    mpz_t scaleUsable;
    mpz_init(scaleUsable);
    mpz_set_ui(scaleUsable, 1);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(radicand, radicand, scaleUsable);
    mpz_root(radicand, radicand, r);
    //mpz_out_str(stdout,10,radicand);
    return mpz_cmp(radicand, goal) > 0;
}

void calcNewBot (mpz_t bot, int r){
    int scale = 100;
    mpz_t radicand;
    mpz_init(radicand);
    mpz_set(radicand,bot);
    mpz_t scaleUsable;
    mpz_init(scaleUsable);
    mpz_set_ui(scaleUsable, 1);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(radicand, radicand, scaleUsable);
    mpz_root(radicand, radicand, r);
    mpz_mul(radicand, radicand, bot);
    mpz_ui_pow_ui(scaleUsable, 10, (scale-1));
    mpz_fdiv_q(radicand, radicand, scaleUsable);
    mpz_set(bot, radicand);
}

void calcNewTop (mpz_t top, int r){
    int scale = 100;
    mpz_t radicand;
    mpz_init(radicand);
    mpz_set(radicand,top);
    mpz_t scaleUsable;
    mpz_init(scaleUsable);
    mpz_set_ui(scaleUsable, 1);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(radicand, radicand, scaleUsable);
    mpz_root(radicand, radicand, r);
    mpz_mul(radicand, radicand, top);
    mpz_ui_pow_ui(scaleUsable, 10, (scale-1));
    mpz_cdiv_q(radicand, radicand, scaleUsable);
    mpz_set(top, radicand);
}

int main(){
    string unCompressedFile;
    int N;
    cout<<"Give name of an uncompressed file, including file extension (e.g. .txt, .cpp). This file MUST be in the same folder as this program\n";
    cin>>unCompressedFile;
    cout<<"Give number of characters to compress at a time, bigger = more compresion\n";
    cin>>N;
    ifstream unCompressed (unCompressedFile);
    ofstream compressed ("compressed.ins");
    compressed << unCompressedFile << " "<< N <<"\n";

    char c;
    mpz_t asciiForm;
    mpz_init(asciiForm);
    mpz_set_ui(asciiForm,0);

    int counter = 0;

    while (unCompressed.get(c)){
        counter++;
        mpz_mul_ui(asciiForm, asciiForm, 1000);
        mpz_add_ui(asciiForm, asciiForm, c + 100);
        if (counter == N){
            bool flag = false;
            mpz_t bot;
            mpz_init_set_ui(bot, calcInitBottom(asciiForm, mpz_sizeinbase(asciiForm, 10)));
            mpz_t top;
            mpz_init_set_ui(top, mpz_get_ui(bot) + 1);
            //cout<<bot<<" "<<top<<endl;
            int r = 2;
            mpz_t x;
            if (checkSuccsfulXR(bot, 2, mpz_sizeinbase(asciiForm, 10), asciiForm)){
                flag = true;
                mpz_init_set(x, bot);
            }
            if (checkSuccsfulXR(top, 2, mpz_sizeinbase(asciiForm, 10), asciiForm)){
                flag = true;
                mpz_init_set(x, top);
            }
            while (!flag){
                mpz_t mid;
                mpz_init(mid);
                while (!flag && mpz_cmp(bot, top) <= 0){
                    mpz_add(mid, bot, top);
                    mpz_tdiv_q_ui(mid, mid, 2);
                    //cout<<top<<" "<<bot<<" "<<mid<<endl;
                    //mpz_out_str(stdout,10,asciiForm);
                    //cout<<endl<<mpz_sizeinbase(asciiForm, 10)<<endl;
                    if (checkSuccsfulXR(mid, r, mpz_sizeinbase(asciiForm, 10), asciiForm)){
                        flag = true;
                        mpz_init_set(x, mid);
                    } else if (isXGreater(mid, r, mpz_sizeinbase(asciiForm, 10), asciiForm)){
                        mpz_sub_ui(top, mid, 1);
                    } else {
                        mpz_add_ui(bot, mid, 1);
                    }
                }
                if (!flag){
                    if (mpz_cmp(mid, top) > 0){
                        mpz_set (bot, top);
                        mpz_set (top, mid);
                    } else {
                        mpz_set (top, bot);
                        mpz_set (bot, mid);
                    }
                    calcNewTop(top, r);
                    calcNewBot(bot, r);
                    r++;
                }
            }
            //cout << x << " "<< r <<"\n";
            //compressed << x << " "<< r <<"\n";
            //mpz_out_str(stdout,10,asciiForm);
            mpz_out_str(stdout,10,x);
            mpz_out_str(*compressed,10,x);
            cout<<mpzToString(x)<<endl;
            compressed << mpzToString(x)<< " "<< r <<"\n";
            cout<<"\n";
            mpz_set_ui(asciiForm,0);

            counter = 0;
        }
    }


    //mpz_out_str(stdout,10,asciiForm);
    cout<<endl;

    //calcInitBottom(asciiForm, mpz_sizeinbase(asciiForm, 10));
    //checkSuccsfulXR(3, 2, mpz_sizeinbase(asciiForm, 10), asciiForm);

    cout << endl<<mpzToString(asciiForm);


    unCompressed.close();
    compressed.close();
    return 0;
}

