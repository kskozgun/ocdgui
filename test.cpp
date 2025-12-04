#include <iostream>

class Test {
private:
    int value;
    int value2;
public:
    explicit Test(int v, int v2) : value(v), value2(v2) {}
    
    void display() {
        std::cout << "Value: " << value << ", Value2: " << value2 << std::endl;
    }
};

int main() {
    Test obj = Test{10, 20};
    obj.display();
    return 0;
}