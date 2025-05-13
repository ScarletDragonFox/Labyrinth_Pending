#include <Labyrinth/Engine/Event/eventManager.hpp>

//Test to see if the event manager works correctly

int main()
{
    int l1 = 0;
    int l2 = 0;
    lp::EventManager em;

    auto lambda = [&l1](lp::Event& e)
    {
        //std::cout << "lamdda 1 = " << static_cast<std::uint16_t>(e.getEventType()) << "\n";
        //std::cout << "\t val = " << e.getData<int>() << "\n";
        l1 += e.getData<int>();
    };

    lp::Event evs(lp::EventTypes::WindowResize, 505);
    lp::Event evs2(lp::EventTypes::WindowResize, 101);

    auto tr = em.on(lp::EventTypes::WindowResize, lambda);

    auto tr2 = em.on(lp::EventTypes::WindowResize, [&l2](lp::Event& e)
    {
        //std::cout << "lamdda 2 = " << static_cast<std::uint16_t>(e.getEventType()) << "\n";
        //std::cout << "\t val = " << e.getData<int>() << "\n";
        l2 +=  e.getData<int>();
    });
    em.emit(evs);
    em.unregister(tr);
    //std::cout << "unreg!\n";
    em.emit(evs2);
    //std::cout << "l1 = " << l1 <<"\nl2 = " << l2 << "\n";
    if(l1 == 505 && l2 == 606) return 0;
    return -1;
}