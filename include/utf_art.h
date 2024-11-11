#pragma once

#include "board.h"
#include <iosfwd>

class UtfArt
{
public:
    void readBoard(const Board &b);
    std::ostream &print(std::ostream &out);

private:
    std::vector<char *> m_output;
};