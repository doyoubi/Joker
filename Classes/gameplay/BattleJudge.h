#ifndef JOKER_BATTLE_JUDGE
#define JOKER_BATTLE_JUDGE

#include <unordered_map>
#include <functional>

namespace joker
{

    enum class HitResult : unsigned int
    {
        PERFECT = 0,
        GOOD,
        OK,
        MISS,
        BOMB,

        COUNT, // for knowing the number of items
    };

    class BattleJudge
    {
    public:
        static HitResult time2HitResult(float dt);

        BattleJudge();
        void applyResult(HitResult result);
        int getScore() const;
        int getPlayerHp() { return _playerHp; }
        void reducePlayerHp();
        typedef std::function<void(int hp)> HpChangedCallback;
        typedef std::function<void(int score)> ScoreChangedCallback;
        HpChangedCallback hpChangedCallback;
        ScoreChangedCallback scoreChangedCallback;
    private:
        int _playerHp;

        void setCount(HitResult result, int count);
        int getCount(HitResult result);
        static const unsigned int ResultCount = (unsigned int)HitResult::COUNT;
        int _resultCount[ResultCount]; // index is HitResult
        int _weight[ResultCount]; // index is HitResult
    };
}

#endif
