// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <libbase64.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>


using std::string;
using std::vector;

static string tobase64_new(const string& message)
{
    if (message.empty())
        return "";
    
    string out;
    out.resize(message.size() * 2 + 2);
    size_t out_size = 0;
    base64_encode(message.c_str(), message.size(), (char*)out.data(), &out_size, BASE64_FORCE_AVX | BASE64_WEB_SAFE);
    out.resize(out_size);
    //std::cout << out.size() << " " << out << std::endl;

    return out;
}

string tobase64_old(const string& message)
{
    if (message.empty())
        return "";
    
    string out;
    out.resize(message.size() * 2 + 2);
    size_t out_size = 0;
    base64_encode(message.c_str(), message.size(), (char*)out.data(), &out_size, BASE64_FORCE_AVX);
    out.resize(out_size);
    //std::cout << out.size() << " " << out << std::endl;
    
    return out;
}

string replaceTo(const string &message) {
    string out(message);
    std::replace(out.begin(), out.end(), '/', '_');
    std::replace(out.begin(), out.end(), '+', '-');
    std::replace(out.begin(), out.end(), '=', '*');
    return out;
}

string unbase64_new(const string& message)
{
    if (message.empty())
        return "";

    string out;
    out.resize(message.size());
    size_t out_size = 0;
    
    if (base64_decode(message.c_str(), message.size(), (char*)out.data(), &out_size, BASE64_FORCE_SSSE3 | BASE64_WEB_SAFE) != 1)
        return "";

    out.resize(out_size);
    //std::cout << out.size() << " " << out << std::endl;
    
    return out;
}

string unbase64_old(const string& message)
{
    if (message.empty())
        return "";
    
    string out;
    out.resize(message.size());
    size_t out_size = 0;
    
    if (base64_decode(message.c_str(), message.size(), (char*)out.data(), &out_size, BASE64_FORCE_AVX) != 1)
        return "";
    
    out.resize(out_size);
    
    return out;
}

string minusToSpace(const std::string &message) {
    std::string in(message);
    std::replace(in.begin(), in.end(), '-', ' ');
    return in;
}

string replaceFrom(const string &message) {
    std::string in(message);
    std::replace(in.begin(), in.end(), '_', '/');
    std::replace(in.begin(), in.end(), '-', '+');
    std::replace(in.begin(), in.end(), '*', '=');
    std::replace(in.begin(), in.end(), ' ', '+');
    return in;
}

void minTest1(const std::string &s) {
    EXPECT_EQ(unbase64_new(tobase64_new(s)), s);
    EXPECT_EQ(unbase64_new(minusToSpace(tobase64_new(s))), s);
    EXPECT_EQ(tobase64_new(s), replaceTo(tobase64_new(s)));
    EXPECT_EQ(unbase64_new(tobase64_new(s)), unbase64_new(replaceTo(tobase64_new(s))));
}

void minTest2(const std::string &s) {
    EXPECT_EQ(unbase64_old(tobase64_old(s)), s);
    EXPECT_EQ(tobase64_old(s).find_first_of("_-*"), string::npos);
    EXPECT_EQ(tobase64_new(s).find_first_of("/+="), string::npos);
    EXPECT_EQ(unbase64_new(replaceTo(tobase64_old(s))), s);
    EXPECT_EQ(unbase64_old(replaceFrom(tobase64_new(s))), s);
}

TEST(base64, old_new_equal)
{
    string s1 = "Hello, World!";
    string s2 =
            "Call me Ishmael. Some years ago--never mind how long precisely--having"
            "little or no money in my purse, and nothing particular to interest me on"
            "shore, I thought I would sail about a little and see the watery part of"
            "the world. It is a way I have of driving off the spleen and regulating"
            "the circulation. Whenever I find myself growing grim about the mouth;"
            "whenever it is a damp, drizzly November in my soul; whenever I find"
            "myself involuntarily pausing before coffin warehouses, and bringing up"
            "the rear of every funeral I meet; and especially whenever my hypos get"
            "such an upper hand of me, that it requires a strong moral principle to"
            "prevent me from deliberately stepping into the street, and methodically"
            "knocking people's hats off--then, I account it high time to get to sea"
            "as soon as I can. This is my substitute for pistol and ball. With a"
            "philosophical flourish Cato throws himself upon his sword; I quietly"
            "take to the ship. There is nothing surprising in this. If they but knew"
            "it, almost all men in their degree, some time or other, cherish very"
            "nearly the same feelings towards the ocean with me.";
    string s3;

    string s4 = "1234567890-=_+!@#$%^&*()qwertyuiop[]asdfghjkl;'zxcvbnm,./";
    string s5;
    for (unsigned char c = (unsigned char)255; c != 0; c--) {
        s5 += c;
    }
    
    string s6(s5);
    std::reverse(s6.begin(), s6.end());
    
    string s7 = "a";
    string s8 = "aa";
    string s9 = "aaa";
    string s10 = "aaaa";
    
    std::vector<string> vs(50);
    for (size_t i = 0; i < vs.size(); i++) {
        const int lenString = rand() % 300;
        for (int j = 1; j <= lenString; j++) {
            vs[i] += (char)(rand() % 256);
        }
    }
    
    minTest1(s1);
    minTest1(s2);
    minTest1(s3);
    minTest1(s4);
    minTest1(s5);
    minTest1(s6);
    minTest1(s7);
    minTest1(s8);
    minTest1(s9);
    minTest1(s10);
    
    for (const string &s: vs) {
        minTest1(s);
    }
    
    /////////
    // OLD //
    /////////
    
    minTest2(s1);
    minTest2(s2);
    minTest2(s3);
    minTest2(s4);
    minTest2(s5);
    minTest2(s6);
    minTest2(s7);
    minTest2(s8);
    minTest2(s9);
    minTest2(s10);
    
    for (const string &s: vs) {
        minTest2(s);
    }
}
