#include "BattleJudge.h"
#include "utils/config.h"
#include "utils/debug.h"

namespace joker
{
    HitResult BattleJudge::time2HitResult(float dt)
    {
        static float perfectDt = Config::getInstance().getDoubleValue({ "Metronome", "perfectDt" });
        static float goodDt = Config::getInstance().getDoubleValue({ "Metronome", "goodDt" });
        static float okDt = Config::getInstance().getDoubleValue({ "Metronome", "okDt" });
        static float hitDeltaTime = Config::getInstance().getDoubleValue({ "Metronome", "hitDeltaTime" });
        DEBUGCHECK(okDt == hitDeltaTime, "okDt must equal hitDeltaTime");
        DEBUGCHECK(dt > 0, "negative dt");
        return
            dt < perfectDt ? HitResult::PERFECT :
            dt < goodDt ? HitResult::GOOD :
            HitResult::OK;
    }

    BattleJudge::BattleJudge()
    {
        static const int PlayerInitHp = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "hp" });
        _playerHp = PlayerInitHp;
        for (int & c : _resultCount)
        {
            c = 0;
        }
        _weight[(unsigned int)HitResult::PERFECT] = 6;
        _weight[(unsigned int)HitResult::GOOD] = 3;
        _weight[(unsigned int)HitResult::OK] = 1;
        _weight[(unsigned int)HitResult::MISS] = 0;
        _weight[(unsigned int)HitResult::BOMB] = 0;
    }

    void BattleJudge::applyResult(HitResult result)
    {
        setCount(result, getCount(result) + 1);
    }

    int BattleJudge::getScore() const
    {
        int score = 0;
        for (int i = 0; i < ResultCount; ++i)
        {
            int weight = _weight[i];
            int count = _resultCount[i];
            score += weight * count;
        }
        return score;
    }

    void BattleJudge::reducePlayerHp()
    {
        _playerHp = std::max(0, _playerHp - 1);
        CHECKNULL(hpChangedCallback);
        hpChangedCallback(_playerHp);
    }

    void BattleJudge::setCount(HitResult result, int count)
    {
        _resultCount[(unsigned int)result] = count;
        CHECKNULL(scoreChangedCallback);
        scoreChangedCallback(getScore());
    }

    int BattleJudge::getCount(HitResult result)
    {
        return _resultCount[(unsigned int)result];
    }

}
