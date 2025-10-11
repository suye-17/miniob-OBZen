/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2024/05/29.
//

#pragma once

#include "common/value.h"
#include "common/sys/rc.h"

class Aggregator
{
public:
  virtual ~Aggregator() = default;

  virtual RC accumulate(const Value &value) = 0;
  virtual RC evaluate(Value &result)        = 0;

protected:
  Value value_;
};

class SumAggregator : public Aggregator
{
public:
  RC accumulate(const Value &value) override;
  RC evaluate(Value &result) override;
};

class CountAggregator : public Aggregator
{
public:
  CountAggregator() : count_(0) {}
  RC accumulate(const Value &value) override;
  RC evaluate(Value &result) override;

private:
  int count_;
};

class AvgAggregator : public Aggregator
{
public:
  AvgAggregator()
  {
    count_           = 0;
    sum_initialized_ = false;
  }
  RC accumulate(const Value &value) override;
  RC evaluate(Value &result) override;

private:
  Value sum_;
  int   count_;
  bool  sum_initialized_;
};

class MaxAggregator : public Aggregator
{
public:
  MaxAggregator() { has_value_ = false; }
  RC accumulate(const Value &value) override;
  RC evaluate(Value &result) override;

private:
  Value max_value_;
  bool  has_value_;
};

class MinAggregator : public Aggregator
{
public:
  MinAggregator() { has_value_ = false; }
  RC accumulate(const Value &value) override;
  RC evaluate(Value &result) override;

private:
  Value min_value_;
  bool  has_value_;
};
