#include <iostream>

template <class val>
class node
{
public:
    node *next;
    val value;
    node(val v, node *n = nullptr) : next(nullptr)
    {
        value = v;
        next = n;
    };
};

int main()
{
    const auto numNodes = 100;
    node<float> *head, *curr, *next;
    head = new node<float>(0);
    curr = head;

    for (int i = 1; i < numNodes; i++)
    {
        curr->next = new node<float>(i);
        curr = curr->next;
    }
    std::cout << "as created\n";
    curr = head;
    while (curr != nullptr)
    {
        std::cout << curr->value << " ";
        curr = curr->next;
    }

    std::cout << "\n";

    std::cout << "\nreversed\n";
    node<float> *prev = nullptr;
    curr = head;

    while (curr != nullptr)
    {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    curr = prev;
    while (curr != nullptr)
    {
        std::cout << curr->value << " ";
        curr = curr->next;
    }

    std::cout << "\n";
    return 0;
}