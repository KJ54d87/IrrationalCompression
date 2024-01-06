#include <iostream>
#include <fstream>
#include <gmp.h>
#include <set>
#include<cstring>
#include<string>

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

mpz_t result;
mpz_t radicand;
mpz_t scaleUsable;

unsigned int calcInitBottom (mpz_t in, int scale){
    mpz_set(result, in);
    mpz_mul(radicand, result, result);
    mpz_ui_pow_ui(scaleUsable, 10, 2*(scale-1));
    mpz_fdiv_q(result, radicand, scaleUsable);
    return mpz_get_si(result);
}

bool checkSuccsfulXR (mpz_t x, int r, int scale, mpz_t goal){
    mpz_set(radicand,x);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(result, radicand, scaleUsable);
    mpz_root(radicand, result, r);
    /**mpz_out_str(stdout,10,radicand);
    cout<<endl;
    mpz_out_str(stdout,10,goal);
    cout<<endl;**/
    return  mpz_cmp(radicand, goal) == 0;
}

bool isXGreater(mpz_t x, int r, int scale, mpz_t goal){
    mpz_set(radicand,x);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(result, radicand, scaleUsable);
    mpz_root(radicand, result, r);
    /**mpz_out_str(stdout,10,radicand);
    cout<<endl;
    mpz_out_str(stdout,10,goal);
    cout<<endl;
    cout<<mpz_cmp(radicand, goal)<<endl;**/
    return mpz_cmp(radicand, goal) > 0;
}

void calcNewBot (mpz_t bot, int r, int scale){
    mpz_set(radicand,bot);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(result, radicand, scaleUsable);
    mpz_root(radicand, result, r);
    mpz_mul(result, radicand, bot);
    mpz_ui_pow_ui(scaleUsable, 10, (scale-1));
    mpz_fdiv_q(radicand, result, scaleUsable);
    mpz_set(bot, radicand);
}

void calcNewTop (mpz_t top, int r, int scale){
    mpz_set(radicand,top);
    mpz_ui_pow_ui(scaleUsable, 10, r*(scale-1));
    mpz_mul(result, radicand, scaleUsable);
    mpz_root(radicand, result, r);
    mpz_mul(result, radicand, top);
    mpz_ui_pow_ui(scaleUsable, 10, (scale-1));
    mpz_cdiv_q(radicand, result, scaleUsable);
    mpz_set(top, radicand);
}

//todo, test a run with size of 11200

void calcIrrational(mpz_t asciiForm, FILE * compressed, int scale){
    bool flag = false;
    mpz_t bot;
    mpz_init_set_ui(bot, calcInitBottom(asciiForm, mpz_sizeinbase(asciiForm, 10)));
    //cout<<calcInitBottom(asciiForm, mpz_sizeinbase(asciiForm, 10))<<endl;
    mpz_t top;
    mpz_init(top);
    mpz_add_ui(top, bot, 1);
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
    mpz_t mid;
    mpz_init(mid);
    while (!flag){
        while (!flag && mpz_cmp(bot, top) <= 0){
            mpz_add(mid, bot, top);
            mpz_fdiv_q_ui(mid, mid, 2);
            if (mpz_sizeinbase(mid, 10)+1000 > 2147483647){
                cout<<"Error, mid to big";
                    break;
            }
            //cout<<top<<" "<<bot<<" "<<mid<<endl;
            //cout<<endl<<mpz_sizeinbase(asciiForm, 10)<<endl;
            //cout<<mpz_sizeinbase(mid, 10)<<endl;
        if (checkSuccsfulXR(mid, r, scale * 3, asciiForm)){
                flag = true;
                mpz_init_set(x, mid);
            } else if (isXGreater(mid, r, scale * 3, asciiForm)){
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
            calcNewTop(top, r, mpz_sizeinbase(mid, 10)+1);
            calcNewBot(bot, r, mpz_sizeinbase(mid, 10)+1);
            // cout<< mpz_sizeinbase(mid, 10)<<endl;
            // cout<< mpz_sizeinbase(mid, 10)<<endl;
            r++;
        }
    }
    mpz_clear(mid);
    //cout << x << " "<< r <<"\n";
    //compressed << x << " "<< r <<"\n";
    //mpz_out_str(stdout,10,asciiForm);
    mpz_clear(top);
    mpz_clear(bot);
    //mpz_out_str(stdout,10,x);
    //cout<<endl;
    //cout<<"1";
    mpz_out_str(compressed, 62,x);
    //mpz_get_str(out, 10, x);
    //compressed << out<< " "<< r <<"\n";
    int buffer[] = {r};
    const char * char_array = (" "+to_string(r) + "\n").c_str();
    fputs(char_array, compressed);
    //fwrite (buffer , sizeof(int), sizeof(buffer), compressed);

    mpz_clear(x);
    cout<<"1 done"<<endl;
}

int main(){
    mpz_inits(result, radicand, scaleUsable);
    string unCompressedFile;
    int N;
    cout<<"Give name of an uncompressed file, including file extension (e.g. .txt, .cpp). This file MUST be in the same folder as this program\n";
    cin>>unCompressedFile;
    cout<<"Give number of characters to compress at a time, bigger = more compresion\n";
    cin>>N;
    ifstream unCompressed (unCompressedFile);
    FILE * compressed;
    compressed = fopen("compressed.ins", "wb");
    string header = unCompressedFile + " " + to_string(N) + "\n";
    const char * char_array = header.c_str();

    fputs(char_array, compressed);
    //compressed << unCompressedFile << " "<< N <<"\n";

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
            calcIrrational(asciiForm, compressed, counter);
            mpz_set_ui(asciiForm,0);

            counter = 0;
        }
    }


    //mpz_out_str(stdout,10,asciiForm);
    cout<<endl;

    //calcInitBottom(asciiForm, mpz_sizeinbase(asciiForm, 10));
    //checkSuccsfulXR(3, 2, mpz_sizeinbase(asciiForm, 10), asciiForm);

    cout << endl<<mpzToString(asciiForm);
    if (counter > 0){
        calcIrrational(asciiForm, compressed, counter);
        mpz_set_ui(asciiForm,0);
    }


    unCompressed.close();
    //compressed.close();
    fclose(compressed);
    return 0;
}

