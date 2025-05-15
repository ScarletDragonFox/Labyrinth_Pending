#include "Labyrinth/Engine/ECS/coreECS.hpp"
#include "Labyrinth/Engine/ECS/baseSystem.hpp"

#include <vector>

using namespace lp;

struct TestComponent
{
    int value = 0;
};

class TestSystem: public ecs::System
{
    public:
    int counter = 0;
    ecs::CoreECS* pECS = nullptr;
    void count()
    {
        for(const auto ent: this->mEntities)
        {
            const auto& comp = pECS->getComponent<TestComponent>(ent);
            counter += comp.value;
        }
    }
};



int main() 
{
    ecs::CoreECS core;
    core.registerComponent<TestComponent>();
    const ecs::Signature sys_Sign = core.getComponentSignature<TestComponent>();
    std::shared_ptr<TestSystem> ts = core.registerSystem<TestSystem>(sys_Sign);
    ts->pECS = &core;
    ts->count();
    if(ts->counter != 0)
    {
        std::cerr << "ts->counter != 0\n";
        return -1;
    }

    std::vector<ecs::Entity> entities; 
    entities.reserve(1'000'000);

    int counter_ext = 0;
    for(int i = 0; i < 1'000'000; ++i)
    {
        ecs::Entity ent = core.createEntity();
        TestComponent tc;
        tc.value = i;
        counter_ext += i;
        core.addComponent<TestComponent>(ent, tc);
        entities.push_back(ent);
    }

    ts->counter = 0;
    ts->count();
    if(ts->counter != counter_ext)
    {
        std::cerr << "ts->counter != counter_ext\n";
        return -1;
    }

    const std::size_t entities_half_way_point = entities.size() / 2;
    std::vector<ecs::Entity> entities_first_half(entities.begin(), entities.begin() + entities_half_way_point);
    std::vector<ecs::Entity> entities_second_half(entities.begin() + entities_half_way_point, entities.end());

    if(entities.size() != (entities_first_half.size() + entities_second_half.size()))
    {
        return -1;
    }

    for(const auto ent: entities_first_half)
    {
        core.destroyEntity(ent);
    }

    counter_ext = 0;
    for(const auto ent: entities_second_half)
    {
        counter_ext += core.getComponent<TestComponent>(ent).value;
    }

    ts->counter = 0;
    ts->count();
    if(ts->counter != counter_ext)
    {
        std::cerr << "ts->counter != counter_ext After Destruction!\n";
        return -1;
    }

    return 0;
}