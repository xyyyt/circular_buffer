#include <gtest/gtest.h>

#include "array_circular_buffer.hpp"

TEST(array_circular_buffer, test_1)
{
    array_circular_buffer<int> acb;

    EXPECT_EQ(acb.buffer_size(), 0u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    try
    {
        acb.add(42);
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

        acb.add(i);
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
        acb.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }

    {
        int i = 42;

        EXPECT_FALSE(acb.try_get(i));
        EXPECT_EQ(i, 42);
    }


    acb.resize(4);

    EXPECT_EQ(acb.buffer_size(), 4u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    acb.add(1).add(2).add(3).add(4).add(5).add(6);

    EXPECT_EQ(acb.buffer_size(), 4u);
    EXPECT_FALSE(acb.is_empty());
    EXPECT_TRUE(acb.is_full());
    EXPECT_EQ(acb.get(), 3);
    EXPECT_EQ(acb.get(), 4);
    EXPECT_EQ(acb.get(), 5);


    acb.resize(8);

    EXPECT_EQ(acb.buffer_size(), 8u);
    EXPECT_FALSE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    acb.add(7).add(8).add(9);

    EXPECT_EQ(acb.buffer_size(), 8u);
    EXPECT_FALSE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());
    EXPECT_EQ(acb.get(), 6);


    acb.clear();

    EXPECT_EQ(acb.buffer_size(), 8u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());
}

TEST(array_circular_buffer, test_2)
{
    array_circular_buffer<int> acb(9);

    EXPECT_EQ(acb.buffer_size(), 9u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    acb.add(4).add(9).add(14).add(18).add(32).add(54).add(60);

    EXPECT_EQ(acb.buffer_size(), 9u);
    EXPECT_FALSE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());
    EXPECT_EQ(acb.get(), 4);
    EXPECT_EQ(acb.get(), 9);


    array_circular_buffer<int> acb2 = acb;

    EXPECT_EQ(acb2.buffer_size(), 9u);
    EXPECT_FALSE(acb2.is_empty());
    EXPECT_FALSE(acb2.is_full());
    EXPECT_EQ(acb2.get(), 14);
    EXPECT_EQ(acb2.get(), 18);
    EXPECT_EQ(acb2.get(), 32);


    array_circular_buffer<int> acb3 = std::move(acb2);

    EXPECT_EQ(acb2.buffer_size(), 0u);
    EXPECT_TRUE(acb2.is_empty());
    EXPECT_FALSE(acb2.is_full());

    try
    {
        acb2.add(42);
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(
            e.what(),
            std::string("circular buffer doesn't have space memory to store"));
    }


    EXPECT_EQ(acb3.buffer_size(), 9u);
    EXPECT_FALSE(acb3.is_empty());
    EXPECT_FALSE(acb3.is_full());
    EXPECT_EQ(acb3.get(), 54);
    EXPECT_EQ(acb3.get(), 60);

    try
    {
        acb3.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }
}

TEST(array_circular_buffer, test_3)
{
    array_circular_buffer<std::string> acb(42);

    EXPECT_EQ(acb.buffer_size(), 42u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());


    using namespace std::literals::string_literals;

    auto tutu = "tutu"s;
    auto tata = "tata"s;
    auto tete = "tete"s;

    acb.add("titi").add("toto").add(tutu).add(tata).add(tete);

    acb.resize(4);

    EXPECT_EQ(acb.buffer_size(), 4u);
    EXPECT_FALSE(acb.is_empty());
    EXPECT_TRUE(acb.is_full());
    EXPECT_EQ(acb.get(), std::string("toto"));
    EXPECT_EQ(acb.get(), std::string("tutu"));
    EXPECT_EQ(acb.get(), std::string("tata"));


    acb.add("tartar").add("turtur").add("terter");

    acb.resize(1);

    EXPECT_EQ(acb.buffer_size(), 1u);
    EXPECT_FALSE(acb.is_empty());
    EXPECT_TRUE(acb.is_full());
    EXPECT_EQ(acb.get(), std::string("terter"));


    acb.resize(1);

    EXPECT_EQ(acb.buffer_size(), 1u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    try
    {
        acb.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }


    acb.add("tortor").add("tirtir");

    acb.resize(0);

    EXPECT_EQ(acb.buffer_size(), 0u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());


    acb.resize(2);

    EXPECT_EQ(acb.buffer_size(), 2u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    std::string value;

    EXPECT_FALSE(acb.try_get(value));
    EXPECT_EQ(value, std::string(""));


    acb.add("teatea").add("teitei").add("toatoa");

    EXPECT_TRUE(acb.try_get(value));
    EXPECT_EQ(value, std::string("teitei"));
    EXPECT_TRUE(acb.try_get(value));
    EXPECT_EQ(value, std::string("toatoa"));
    EXPECT_EQ(acb.buffer_size(), 2u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    value = "";

    EXPECT_FALSE(acb.try_get(value));
    EXPECT_EQ(value, std::string(""));
}

TEST(array_circular_buffer, test_4)
{
    array_circular_buffer<std::string> acb(0);

    EXPECT_EQ(acb.buffer_size(), 0u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    try
    {
        acb.add("azertyuiop");
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

        acb.add(azertyuiop);
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
        acb.get();
        FAIL() << "expected std::out_of_range";
    }
    catch (const std::out_of_range& e)
    {
        EXPECT_EQ(e.what(), std::string("circular buffer is empty"));
    }


    acb.resize(5);
    acb.clear();

    EXPECT_EQ(acb.buffer_size(), 5u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());


    auto toto = "toto"s;
    auto tata = "tata"s;
    auto tartar = "tartar"s;
    auto terter = "terter"s;

    acb.add("titi")
        .add(toto)
        .add("tutu")
        .add(tata)
        .add("tete")
        .add(tartar)
        .add("turtur")
        .add(terter);

    array_circular_buffer<std::string> acb2;

    acb2 = acb;

    EXPECT_EQ(acb2.buffer_size(), 5u);
    EXPECT_FALSE(acb2.is_empty());
    EXPECT_TRUE(acb2.is_full());
    EXPECT_EQ(acb2.get(), std::string("tata"));
    EXPECT_EQ(acb2.get(), std::string("tete"));
    EXPECT_EQ(acb2.get(), std::string("tartar"));
    EXPECT_EQ(acb2.get(), std::string("turtur"));
    EXPECT_EQ(acb2.get(), std::string("terter"));


    acb2 = std::move(acb);

    EXPECT_EQ(acb.buffer_size(), 0u);
    EXPECT_TRUE(acb.is_empty());
    EXPECT_FALSE(acb.is_full());

    EXPECT_EQ(acb2.buffer_size(), 5u);
    EXPECT_FALSE(acb2.is_empty());
    EXPECT_TRUE(acb2.is_full());
    EXPECT_EQ(acb2.get(), std::string("tata"));
    EXPECT_EQ(acb2.get(), std::string("tete"));
    EXPECT_EQ(acb2.get(), std::string("tartar"));
    EXPECT_EQ(acb2.get(), std::string("turtur"));
    EXPECT_EQ(acb2.get(), std::string("terter"));
}
