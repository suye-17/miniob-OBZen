#include <iostream>
#include <string>
#include "src/observer/sql/parser/parse.h"
#include "src/observer/sql/parser/parse_defs.h"

int main() {
    const char* sql = "select * from test;";
    std::cout << "Testing SQL: " << sql << std::endl;
    
    ParsedSqlResult result;
    parse(sql, &result);
    
    std::cout << "Parse result size: " << result.sql_nodes().size() << std::endl;
    
    if (!result.sql_nodes().empty()) {
        auto& node = result.sql_nodes()[0];
        std::cout << "Node flag: " << node->flag << std::endl;
        if (node->flag == SCF_ERROR) {
            std::cout << "Error: " << node->error.error_msg << std::endl;
        }
    }
    
    return 0;
}
