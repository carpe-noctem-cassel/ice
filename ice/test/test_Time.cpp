/*
 * test_Time.cpp
 *
 *  Created on: May 26, 2014
 *      Author: sni
 */

#include <iostream>
#include <string>
#include <thread>
#include <time.h>
#include <typeinfo>
#include <memory>

#include "ice/Time.h"

#include "gtest/gtest.h"

namespace
{

class TimeTest : public ::testing::Test
{
protected:
  time_t start_time_;

  virtual void SetUp()
  {
    start_time_ = time(NULL);
  }

  virtual void TearDown()
  {
    const time_t end_time = time(NULL);

    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long.";
  }
};


TEST_F(TimeTest, isLower)
{
  ice::SimpleTimeFactory ttf;
  auto t0 = ttf.createTime();

  std::this_thread::sleep_for(std::chrono::milliseconds {1});

  auto t1 = ttf.createTime();

  EXPECT_TRUE(t0 < t1);
  EXPECT_FALSE(t1 < t0);
}


TEST_F(TimeTest, timeOut)
{
  ice::SimpleTimeFactory ttf;
  auto t0 = ttf.createTime();

  std::cout << t0 << std::endl;

  EXPECT_FALSE(ttf.checkTimeout(t0, 500));

  std::this_thread::sleep_for(std::chrono::milliseconds {1000});

  EXPECT_TRUE(ttf.checkTimeout(t0, 500));
}

}

