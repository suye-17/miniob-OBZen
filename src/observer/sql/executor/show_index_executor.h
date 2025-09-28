#pragma once

#include "common/sys/rc.h"


class SQLStageEvent;
class ShowIndexExecutor 
{
public:
    ShowIndexExecutor() = default;
    ~ShowIndexExecutor() = default; 
    RC execute(SQLStageEvent *sql_event);
};