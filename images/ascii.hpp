#ifndef ASCII_H_
#define ASCII_H_

#include<unordered_map>
#include<array>
#include<optional>
#include<cstdint>
#include<ranges>

// Some, but not all, ASCII characters.

namespace ascii{

struct CharMatrix{
    const int numRows;
    const int numCols;
    std::array<std::array<std::uint8_t, 5>, 7> backing{};
    CharMatrix(): numRows{}, numCols{}{};
    CharMatrix(std::initializer_list<std::initializer_list<std::uint8_t>> init):
        numRows{static_cast<int>(init.size())},
        numCols{static_cast<int>(init.begin()->size())}{
        for (auto [row, backing_row]: std::views::zip(init, backing))
        {
            std::copy(row.begin(), row.end(), backing_row.begin());
        }
    }
};

using CharMatrix_ = std::array<std::array<std::uint8_t, 5>, 7>;

std::optional<CharMatrix> get_char_grid(char const ch)
{
    static std::array<std::optional<CharMatrix>, 94> charMap = {
    /*   ch  val  idx */
       /* !   33    0 */ CharMatrix{{0,1,0},
                                    {0,1,0},
                                    {0,1,0},
                                    {0,1,0},
                                    {0,1,0},
                                    {0,0,0},
                                    {0,1,0}},

       /* "   34    1 */ CharMatrix{{1,0,1},
                                    {1,0,1},
                                    {0,0,0},
                                    {0,0,0},
                                    {0,0,0},
                                    {0,0,0},
                                    {0,0,0}},

       /* #    35   2*/  CharMatrix{{0,1,0,1,0},
                                    {0,1,0,1,0},
                                    {1,1,1,1,1},
                                    {0,1,0,1,0},
                                    {1,1,1,1,1},
                                    {0,1,0,1,0},
                                    {0,1,0,1,0}},
    };
/*
# 35 2
$ 36 3
% 37 4
& 38 5
' 39 6
( 40 7
) 41 8
* 42 9
+ 43 10
, 44 11
- 45 12
. 46 13
/ 47 14
0 48 15
1 49 16
2 50 17
3 51 18
4 52 19
5 53 20
6 54 21
7 55 22
8 56 23
9 57 24
: 58 25
; 59 26
< 60 27
= 61 28
> 62 29
? 63 30
@ 64 31
A 65 32
B 66 33
C 67 34
D 68 35
E 69 36
F 70 37
G 71 38
H 72 39
I 73 40
J 74 41
K 75 42
L 76 43
M 77 44
N 78 45
O 79 46
P 80 47
Q 81 48
R 82 49
S 83 50
T 84 51
U 85 52
V 86 53
W 87 54
X 88 55
Y 89 56
Z 90 57
[ 91 58
\ 92 59
] 93 60
^ 94 61
_ 95 62
` 96 63
a 97 64
b 98 65
c 99 66
d 100 67
e 101 68
f 102 69
g 103 70
h 104 71
i 105 72
j 106 73
k 107 74
l 108 75
m 109 76
n 110 77
o 111 78
p 112 79
q 113 80
r 114 81
s 115 82
t 116 83
u 117 84
v 118 85
w 119 86
x 120 87
y 121 88
z 122 89
{ 123 90
| 124 91
} 125 92
~ 126 93
*/
    return charMap[static_cast<int>(::tolower(ch) - 33)];
}
}


#endif // ASII_H_
