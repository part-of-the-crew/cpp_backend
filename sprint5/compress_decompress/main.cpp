#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

using namespace std;
using namespace std::string_literals;
struct Nucleotide {
    char symbol;
    size_t position;
    int chromosome_num;
    int gene_num;
    bool is_marked;
    char service_info;
};

struct CompactNucleotide {
    uint32_t service_info:  8;
    uint32_t symbol:        2;
    uint32_t is_marked:     1;
    uint32_t chromosome_num:6;
    uint32_t gene_num:      15;
    uint32_t position;
};

CompactNucleotide Compress(const Nucleotide& n) {
    CompactNucleotide ret;
    ret.position = n.position;
    ret.chromosome_num = n.chromosome_num;
    ret.gene_num = n.gene_num;
    ret.is_marked = n.is_marked;
    ret.service_info = n.service_info;
    if (n.symbol == 'A')
        ret.symbol = 0;
    if (n.symbol == 'T')
        ret.symbol = 1;
    if (n.symbol == 'G')
        ret.symbol = 2;
    if (n.symbol == 'C')
        ret.symbol = 3;     
    return ret;
}
/*symbol — общеупотребимый буквенный код одного из четырёх нуклеотидов: A, T, G или С.
position — положение найденного нуклеотида в геноме. Может принимать значения от 0 до 3,3 млрд.
chromosome_num — номер хромосомы, в которой найден нуклеотид. Может принимать значения от 1 до 46.
gene_num — номер гена, в котором найден нуклеотид. Может принимать значения от 1 до 25 тыс., а также специальное значение 0, в случае если найденный нуклеотид лежит вне известных генов.
is_marked — флаг, содержащий информацию о том, был нуклеотид помечен ранее или нет.
service_info — служебная информация о результатах поиска. Может быть любым символом.*/
Nucleotide Decompress(const CompactNucleotide& n) {
    Nucleotide ret;
    ret.position = n.position;
    ret.chromosome_num = n.chromosome_num;
    ret.gene_num = n.gene_num;
    ret.is_marked = n.is_marked;
    ret.service_info = n.service_info;
    if (n.symbol == 0)
        ret.symbol = 'A';
    if (n.symbol == 1)
        ret.symbol = 'T';
    if (n.symbol == 2)
        ret.symbol = 'G';
    if (n.symbol == 3)
        ret.symbol = 'C'; 
    return ret;
}

static_assert(sizeof(CompactNucleotide) <= 8, "Your CompactNucleotide is not compact enough");
static_assert(alignof(CompactNucleotide) == 4, "Don't use '#pragma pack'!");
bool operator==(const Nucleotide& lhs, const Nucleotide& rhs) {
    return (lhs.symbol == rhs.symbol) && (lhs.position == rhs.position) && (lhs.chromosome_num == rhs.chromosome_num)
    && (lhs.gene_num == rhs.gene_num) && (lhs.is_marked == rhs.is_marked) && (lhs.service_info == rhs.service_info);
}
void TestSize() {
    assert(sizeof(CompactNucleotide) <= 8);
}
void TestCompressDecompress() {
    Nucleotide source;
    source.symbol = 'G';
    source.position = 3'300'000'000;
    source.chromosome_num = 48;
    source.gene_num = 25'000;
    source.is_marked = true;
    source.service_info = '!';

    CompactNucleotide compressedSource = Compress(source);
    Nucleotide decompressedSource = Decompress(compressedSource);

    assert(source == decompressedSource);
}

int main() {
    TestSize();
    TestCompressDecompress();
}
