#ifndef __C_PRICE_SPELLING__H__
#define __C_PRICE_SPELLING__H__

#include <string>
#include <cmath>
#include <stack>
#include <vector>

#include "clog.h"

using namespace std;

#define MALE_GENDER     0
#define FEMALE_GENDER   1


class C_Price_Spelling
{
    private:
        vector<vector<string>>  spelling_magnitude = {  {"", "", ""}, 
                                                        {"тысяч", "тысяча", "тысячи"},
                                                        {"миллионов", "миллион", "миллиона"},
                                                        {"миллиардов", "миллиард", "миллиарда"},
                                                        {"трилионов", "трилион", "трилиона"},
                                                    };
        vector<string>  spelling_cents =            {"копеек", "копейка", "копейки"};
        vector<string>  spelling_dollars =          {"рублей", "рубль", "рубля"};
        vector<string>  spelling_hundreds =         {"сто", "двасти", "тристо", "четыресто", "пятсот", "шестьсот", "семьсот", "восемьсот", "девятьсот"};
        vector<string>  spelling_tenths =           {"десять", "двадцать", "тридцать", "сорок", "пятьдесят", "шестьдесят", "семдесят", "восемдесят", "девяносто"};
        vector<string>  spelling_11_19 =            {"одиннадцть", "двенадцать", "тринадцать", "четырнадцать", "пятнадцать", "шестнадцать", "семнадцать", "восемнадцать", "девятнадцать"};
        vector<vector<string>>  spelling_digit =    {
                                                        {"один", "два", "три", "четыре", "пять", "шесть", "семь", "восемь", "девять"},
                                                        {"одна", "две", "три", "четыре", "пять", "шесть", "семь", "восемь", "девять"},
                                                    };
        vector<int>     gender_order_of_magnitude = {
                                                        MALE_GENDER, // --- index[0] - doesn't matter
                                                        FEMALE_GENDER, // --- thouthand is female gender
                                                        MALE_GENDER, // --- million is male gender
                                                        MALE_GENDER, // --- milliard is male gender
                                                        MALE_GENDER, // --- trillion is male gender
                                                    };
        double price = 0;
        const long max = 1000000000000000 - 1;

    public:
    			C_Price_Spelling() {};
    			C_Price_Spelling(double param) : price(param) {};
        auto	Set(double param) {price = param; };
        auto	SpellNumber(long, int gender) -> string;
        auto	SpellRubles(long) -> string;
        auto	SpellCents(long) -> string;
        auto	Spelling() -> string;
        auto	GetWhole(double param) -> long;
        auto	GetFraction(double param) -> long;
        auto	DeclensionIndex(long number) -> int;
        auto	SpellOrderOfMagnitude(int order_of_magnitude, long number) -> string;
        auto	SpellUpToThouthand(long, int gender) -> string;
        auto	SpellHundreds(long) -> string;
        auto	Spell11_19(long) -> string;
        auto	SpellTenths(long) -> string;
        auto	SpellDigit(long, int gender) -> string;
};

#endif 