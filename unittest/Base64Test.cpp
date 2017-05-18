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


using std::string;
using std::vector;


static string tobase64_old(const string& message)
{
    BIO *bio, *b64;
    FILE* stream;
    int encodedSize = 4*ceil((double)message.size()/3);
    char *buffer = (char *)malloc(encodedSize+1);

    stream = fmemopen(buffer, encodedSize+1, "w");
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    BIO_write(bio, message.c_str(), message.size());
    (void) BIO_flush(bio);
    BIO_free_all(bio);
    fclose(stream);

    string res(buffer);
    free(buffer);

    return res;
}

static string unbase64_old(const string& message)
{
    if (message.empty())
        return "";

    auto message_size = message.size();
    const char* message_ptr = message.c_str();

    /* добавляем в конец = чтобы размер сообщения был корректен
     * В бейс64 один символ кодирует 6 бит. Таки образом, паддинг нужен
     * для выравнивания размера сообщения
     */
    string temp;
    if ((message_size * 6) % 8 != 0) {
        temp = message;

        while ((temp.size() * 6) % 8 != 0)
            temp.append("=");

        message_size = temp.size();
        message_ptr = temp.c_str();
    }


    BIO *b64, *bmem;
    char* buffer = (char*) (malloc(message_size));
    memset(buffer, 0, message_size);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new_mem_buf((char*)message_ptr, message_size);
    bmem = BIO_push(b64, bmem);

    string result;
    int result_size = BIO_read(bmem, buffer, message_size);
    if (result_size > 0)
       result.assign(buffer, result_size);

    BIO_free_all(bmem);
    free(buffer);

    return result;
}

static string tobase64_new(const string& message)
{
    if (message.empty())
        return "";

    string out;
    out.resize(message.size() * 2);
    size_t out_size = 0;
    base64_encode(message.c_str(), message.size(), (char*)out.data(), &out_size, BASE64_FORCE_AVX);
    out.resize(out_size);

    return out;
}

static string unbase64_new(const string& message)
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

static void fill_string_with_random(string& data, size_t size)
{
    if (!size)
    {
        data.clear();
        return;
    }

    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    data.resize(size);

    for (auto& c: data)
        c = alphanum[rand() % (sizeof(alphanum) - 1)];
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

    EXPECT_EQ(tobase64_old(s1), tobase64_new(s1));
    EXPECT_EQ(tobase64_old(s2), tobase64_new(s2));
    EXPECT_EQ(tobase64_old(s3), tobase64_new(s3));

    EXPECT_EQ(unbase64_old(tobase64_old(s1)), s1);
    EXPECT_EQ(unbase64_old(tobase64_old(s2)), s2);
    EXPECT_EQ(unbase64_old(tobase64_old(s3)), s3);

    EXPECT_EQ(unbase64_new(tobase64_old(s1)), s1);
    EXPECT_EQ(unbase64_new(tobase64_old(s2)), s2);
    EXPECT_EQ(unbase64_new(tobase64_old(s3)), s3);

    EXPECT_EQ(unbase64_old(tobase64_new(s1)), s1);
    EXPECT_EQ(unbase64_old(tobase64_new(s2)), s2);
    EXPECT_EQ(unbase64_old(tobase64_new(s3)), s3);
}


class PerformanceTest : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        srand(time(nullptr));

        string_plain.resize(iterations);
        string_base64.resize(iterations);

        for (size_t i = 0; i < iterations; i++)
        {
            fill_string_with_random(string_plain[i], data_size);
            string_base64[i] = tobase64_new(string_plain[i]);
        }
    }

    static constexpr size_t iterations = 10000;
    static constexpr size_t data_size = 32;

    static vector<string> string_plain;
    static vector<string> string_base64;
};

vector<string> PerformanceTest::string_plain;
vector<string> PerformanceTest::string_base64;



TEST_F(PerformanceTest, old_encode)
{
    for (auto& i: string_plain)
    {
        string encoded = tobase64_old(i);
        EXPECT_NE(i, encoded);
        EXPECT_FALSE(encoded.empty());
    }
}

TEST_F(PerformanceTest, new_encode)
{
    for (auto& i: string_plain)
    {
        string encoded = tobase64_new(i);
        EXPECT_NE(i, encoded);
        EXPECT_FALSE(encoded.empty());
    }
}

TEST_F(PerformanceTest, old_decode)
{
    for (auto& i: string_base64)
    {
        string decoded = unbase64_old(i);
        EXPECT_NE(decoded, i);
        EXPECT_FALSE(decoded.empty());
    }
}

TEST_F(PerformanceTest, new_decode)
{
    for (auto& i: string_base64)
    {
        string decoded = unbase64_new(i);
        EXPECT_NE(decoded, i);
        EXPECT_FALSE(decoded.empty());
    }
}
