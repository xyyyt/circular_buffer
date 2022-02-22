#include <gtest/gtest.h>

#include "list_circular_buffer.hpp"

TEST(list_circular_buffer, test_1)
{
    list_circular_buffer<int> lcb;

    EXPECT_EQ(lcb.buffer_size(), 0u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    try
    {
        lcb.add(42);
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(
            e.what(),
            std::string("circular buffer doesn't have space memory to store"));
    }

    try
    {
        int i = 42;

        lcb.add(i);
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(
            e.what(),
            std::string("circular buffer doesn't have space memory to store"));
    }

    try
    {
        lcb.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }

    {
        int i = 42;

        EXPECT_FALSE(lcb.try_get(i));
        EXPECT_EQ(i, 42);
    }


    lcb.resize(4);

    EXPECT_EQ(lcb.buffer_size(), 4u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    lcb.add(1).add(2).add(3).add(4).add(5).add(6);

    EXPECT_EQ(lcb.buffer_size(), 4u);
    EXPECT_FALSE(lcb.is_empty());
    EXPECT_TRUE(lcb.is_full());
    EXPECT_EQ(lcb.get(), 3);
    EXPECT_EQ(lcb.get(), 4);
    EXPECT_EQ(lcb.get(), 5);


    lcb.resize(8);

    EXPECT_EQ(lcb.buffer_size(), 8u);
    EXPECT_FALSE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    lcb.add(7).add(8).add(9);

    EXPECT_EQ(lcb.buffer_size(), 8u);
    EXPECT_FALSE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());
    EXPECT_EQ(lcb.get(), 6);


    lcb.clear();

    EXPECT_EQ(lcb.buffer_size(), 8u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());
}

TEST(list_circular_buffer, test_2)
{
    list_circular_buffer<int> lcb(9);

    EXPECT_EQ(lcb.buffer_size(), 9u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    lcb.add(4).add(9).add(14).add(18).add(32).add(54).add(60);

    EXPECT_EQ(lcb.buffer_size(), 9u);
    EXPECT_FALSE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());
    EXPECT_EQ(lcb.get(), 4);
    EXPECT_EQ(lcb.get(), 9);


    list_circular_buffer<int> lcb2 = lcb;

    EXPECT_EQ(lcb2.buffer_size(), 9u);
    EXPECT_FALSE(lcb2.is_empty());
    EXPECT_FALSE(lcb2.is_full());
    EXPECT_EQ(lcb2.get(), 14);
    EXPECT_EQ(lcb2.get(), 18);
    EXPECT_EQ(lcb2.get(), 32);


    list_circular_buffer<int> lcb3 = std::move(lcb2);

    EXPECT_EQ(lcb2.buffer_size(), 0u);
    EXPECT_TRUE(lcb2.is_empty());
    EXPECT_FALSE(lcb2.is_full());

    try
    {
        lcb2.add(42);
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(
            e.what(),
            std::string("circular buffer doesn't have space memory to store"));
    }


    EXPECT_EQ(lcb3.buffer_size(), 9u);
    EXPECT_FALSE(lcb3.is_empty());
    EXPECT_FALSE(lcb3.is_full());
    EXPECT_EQ(lcb3.get(), 54);
    EXPECT_EQ(lcb3.get(), 60);

    try
    {
        lcb3.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }
}

TEST(list_circular_buffer, test_3)
{
    list_circular_buffer<std::string> lcb(42);

    EXPECT_EQ(lcb.buffer_size(), 42u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());


    using namespace std::literals::string_literals;

    auto tutu = "tutu"s;
    auto tata = "tata"s;
    auto tete = "tete"s;

    lcb.add("titi").add("toto").add(tutu).add(tata).add(tete);

    lcb.resize(4);

    EXPECT_EQ(lcb.buffer_size(), 4u);
    EXPECT_FALSE(lcb.is_empty());
    EXPECT_TRUE(lcb.is_full());
    EXPECT_EQ(lcb.get(), std::string("toto"));
    EXPECT_EQ(lcb.get(), std::string("tutu"));
    EXPECT_EQ(lcb.get(), std::string("tata"));


    lcb.add("tartar").add("turtur").add("terter");

    lcb.resize(1);

    EXPECT_EQ(lcb.buffer_size(), 1u);
    EXPECT_FALSE(lcb.is_empty());
    EXPECT_TRUE(lcb.is_full());
    EXPECT_EQ(lcb.get(), std::string("terter"));


    lcb.resize(1);

    EXPECT_EQ(lcb.buffer_size(), 1u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    try
    {
        lcb.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }


    lcb.add("tortor").add("tirtir");

    lcb.resize(0);

    EXPECT_EQ(lcb.buffer_size(), 0u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());


    lcb.resize(2);

    EXPECT_EQ(lcb.buffer_size(), 2u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    std::string value;

    EXPECT_FALSE(lcb.try_get(value));
    EXPECT_EQ(value, std::string(""));


    lcb.add("teatea").add("teitei").add("toatoa");

    EXPECT_TRUE(lcb.try_get(value));
    EXPECT_EQ(value, std::string("teitei"));
    EXPECT_TRUE(lcb.try_get(value));
    EXPECT_EQ(value, std::string("toatoa"));
    EXPECT_EQ(lcb.buffer_size(), 2u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    value = "";

    EXPECT_FALSE(lcb.try_get(value));
    EXPECT_EQ(value, std::string(""));
}

TEST(list_circular_buffer, test_4)
{
    list_circular_buffer<std::string> lcb(0);

    EXPECT_EQ(lcb.buffer_size(), 0u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    try
    {
        lcb.add("azertyuiop");
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(
            e.what(),
            std::string("circular buffer doesn't have space memory to store"));
    }


    using namespace std::literals::string_literals;

    try
    {
        auto azertyuiop = "azertyuiop"s;

        lcb.add(azertyuiop);
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(
            e.what(),
            std::string("circular buffer doesn't have space memory to store"));
    }

    try
    {
        lcb.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }


    lcb.resize(5);
    lcb.clear();

    EXPECT_EQ(lcb.buffer_size(), 5u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());


    auto toto = "toto"s;
    auto tata = "tata"s;
    auto tartar = "tartar"s;
    auto terter = "terter"s;

    lcb.add("titi")
        .add(toto)
        .add("tutu")
        .add(tata)
        .add("tete")
        .add(tartar)
        .add("turtur")
        .add(terter);

    list_circular_buffer<std::string> lcb2;

    lcb2 = lcb;

    EXPECT_EQ(lcb2.buffer_size(), 5u);
    EXPECT_FALSE(lcb2.is_empty());
    EXPECT_TRUE(lcb2.is_full());
    EXPECT_EQ(lcb2.get(), std::string("tata"));
    EXPECT_EQ(lcb2.get(), std::string("tete"));
    EXPECT_EQ(lcb2.get(), std::string("tartar"));
    EXPECT_EQ(lcb2.get(), std::string("turtur"));
    EXPECT_EQ(lcb2.get(), std::string("terter"));


    lcb2 = std::move(lcb);

    EXPECT_EQ(lcb.buffer_size(), 0u);
    EXPECT_TRUE(lcb.is_empty());
    EXPECT_FALSE(lcb.is_full());

    EXPECT_EQ(lcb2.buffer_size(), 5u);
    EXPECT_FALSE(lcb2.is_empty());
    EXPECT_TRUE(lcb2.is_full());
    EXPECT_EQ(lcb2.get(), std::string("tata"));
    EXPECT_EQ(lcb2.get(), std::string("tete"));
    EXPECT_EQ(lcb2.get(), std::string("tartar"));
    EXPECT_EQ(lcb2.get(), std::string("turtur"));
    EXPECT_EQ(lcb2.get(), std::string("terter"));
}
