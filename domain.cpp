#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <cassert>



class Domain {
public:
    // разработайте класс домена

    // конструктор должен позвол€ть конструирование из string, с сигнатурой определитесь сами
    Domain(std::string_view name) {
        std::string domain = '.' + std::string(name);

        std::reverse(domain.begin(), domain.end());

        reverse_domain_ = domain;
    }

    bool operator==(const Domain& other) const {

        return std::lexicographical_compare(reverse_domain_.begin(), reverse_domain_.end(),
            other.reverse_domain_.begin(),
            other.reverse_domain_.end());
    }

    bool IsSubdomain(const Domain& other) const {
        return other.reverse_domain_.size() <= reverse_domain_.size() &&
            std::equal(other.reverse_domain_.begin(), other.reverse_domain_.end(), reverse_domain_.begin());
    }

    std::string GetDomain() {
        return reverse_domain_;
    }
    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
private:
    std::string reverse_domain_;

};

class DomainChecker {
public:
    // конструктор должен принимать список запрещЄнных доменов через пару итераторов
    // Ќаписано дл€ —++17

    template <typename Iterator>
    DomainChecker(Iterator container_begin, Iterator container_end) {
        std::vector<Domain> forbidden_domains(container_begin, container_end);

        std::sort(forbidden_domains.begin(), forbidden_domains.end(),
            [](const Domain& rhs, const Domain& lhs) {
                return rhs == lhs;
            });

        Iterator unique_domains = std::unique(forbidden_domains.begin(), forbidden_domains.end(),
            [](const Domain& rhs, const Domain& lhs) {
                return lhs.IsSubdomain(rhs);
            }); // тут проблема.



        forbidden_domains.erase(unique_domains, forbidden_domains.end());
        forbidden_domains_ = std::move(forbidden_domains);
    }

    // разработайте метод IsForbidden, возвращающий true, если домен запрещЄ
    bool IsForbidden(const Domain& domain) const {
        return std::any_of(forbidden_domains_.begin(), forbidden_domains_.end(),
            [&domain](const Domain& forbidden_domain) {
                return domain.IsSubdomain(forbidden_domain);
            });
    }

    std::vector<Domain> GetForbiddenDomains() {
        return forbidden_domains_;
    }

private:
    std::vector<Domain> forbidden_domains_;
};

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
const std::vector<Domain> ReadDomains(std::istream& input, size_t count) {
    std::vector<Domain> domains;
    domains.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        std::string domain_name;
        std::getline(input, domain_name);
        domains.emplace_back(std::move(domain_name));
    }

    return domains;

}

template <typename Number>
Number ReadNumberOnLine(std::istream& input) {
    std::string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void TestDomainClass() {
    std::string test_name = "abc.org";
    std::string test_domain_name = ".abc.org";
    std::reverse(test_domain_name.begin(), test_domain_name.end());


    Domain domain1(test_name);
    Domain domain2(test_name);
    assert(domain1.GetDomain() == test_domain_name);
    assert(domain2.GetDomain() == test_domain_name);
    assert(!(domain1 == domain2)); // тут domain1 == domain2 долж вернуть false, так как используетс€ lexicographical_compare
    // который вернет false, если элементы двух диапазонов не отличаютс€

    Domain domain3("xyz.abc.ru");
    assert(domain3.IsSubdomain(Domain("abc.ru")));
}

void TestDomainChecker() {
    Domain gdz("gdz.ru");
    Domain com("com");
    Domain me("me");
    Domain biology("biology.gdz.ru");

    std::vector<Domain> forbidden_domains = { gdz, com, me, biology };
    std::vector<Domain> test_forbidden_domains = { gdz, com, me };
    std::sort(test_forbidden_domains.begin(), test_forbidden_domains.end(),
        [](const Domain& rhs, const Domain& lhs) {
            return rhs == lhs;
        });

    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    assert(checker.IsForbidden(gdz));
    assert(checker.IsForbidden(com));
    assert(checker.IsForbidden(me));
    auto temp = checker.GetForbiddenDomains();
    assert(temp.size() == test_forbidden_domains.size());

    // ѕоэлементное сравнение векторов: эталонного и того, что вернул метод GetForbiddenDomains
    for (size_t i = 0; i < temp.size(); ++i) {
        auto rhs = temp[i].GetDomain();
        auto lhs = test_forbidden_domains[i].GetDomain();
        assert(!(temp[i] == test_forbidden_domains[i]));
    }
    assert(checker.IsForbidden(biology));
}

int main() {
    using namespace std::literals;

    TestDomainClass();
    TestDomainChecker();
    const std::vector<Domain> forbidden_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    for (const Domain& domain : test_domains) {
        std::cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << std::endl;
    }
}
