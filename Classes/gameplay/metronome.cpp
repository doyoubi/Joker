#include <algorithm>

#include "utils/debug.h"
#include "metronome.h"

namespace joker
{
    using namespace cocos2d;

    // Metronome
    Metronome::Metronome(const std::vector<float> & rhythmPoints, float hitDeltaTime)
        : _rhythmPoints(rhythmPoints.size() + 2), _hitted(rhythmPoints.size() + 2),
        _missCallBack(nullptr), _hitCallBack(nullptr),
        _rhythmCallBack(nullptr), _wrongHitCallBack(nullptr),
        _hitDeltaTime(hitDeltaTime)
    {
        DEBUGCHECK(std::all_of(begin(rhythmPoints), end(rhythmPoints),
            // or t > 2 * hitDeltaTime, if you think the left and right interval should not intersect
            [hitDeltaTime](float t){ return t > hitDeltaTime; }),
            "some delta time between rhythmPoints is too small");
        DEBUGCHECK(rhythmPoints.size() > 0, "empty rhythmPoints");
        DEBUGCHECK(hitDeltaTime > 0.01666667,
            "warning!, the game loop update every 0.01666667 second, "
            "if hitDeltaTime is less then 0.01666667, "
            "this small hitDeltaTime may sometime be undetected.");

        _rhythmPoints[0] = 0.0f;
        _rhythmPoints[_rhythmPoints.size() - 1] = 1.0f + hitDeltaTime;
        std::copy(std::begin(rhythmPoints), std::end(rhythmPoints), ++std::begin(_rhythmPoints));

        reset();
    }

    void Metronome::reset()
    {
        _passedTime = 0.0f;
        _timeSinceLastPoint = 0.0f;
        _nextRhythmPointsIndex = 1;

        std::fill(begin(_hitted), end(_hitted), 0);
        // set it to true, then the first point will not invoke miss or hit callback
        _rhythmPointChecked = true;

        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->scheduleUpdate(this, 0, true);
    }

    void Metronome::start()
    {
        DEBUGCHECK(
            Director::getInstance()->getScheduler()->isTargetPaused(this),
            "target is not paused");
        Director::getInstance()->getScheduler()->resumeTarget(this);
    }

    void Metronome::update(float dt)
    {
        _passedTime += dt;
        _timeSinceLastPoint += dt;

        if (_timeSinceLastPoint >= _rhythmPoints[_nextRhythmPointsIndex])
        {
            if (_rhythmCallBack)
            {
                int scriptIndex = _nextRhythmPointsIndex - 1; // minus one, change to script index
                _rhythmCallBack(scriptIndex);
            }
            _timeSinceLastPoint -= _rhythmPoints[_nextRhythmPointsIndex];
            ++_nextRhythmPointsIndex;

            _rhythmPointChecked = false;
        }

        // miss or hit callback
        if (!_rhythmPointChecked
            && _timeSinceLastPoint > _hitDeltaTime)
        {
            int lastPointIndex = _nextRhythmPointsIndex - 1;
            int scriptIndex = lastPointIndex - 1;  // minus one, change to script index
            if (_hitted[lastPointIndex])
            {
                if (_hitCallBack)
                    _hitCallBack(scriptIndex, _timeSinceLastPoint);
            }
            else if (_missCallBack)
            {
                _missCallBack(scriptIndex); // minus one, change to script index
            }
            _rhythmPointChecked = true;
            if (_nextRhythmPointsIndex == _rhythmPoints.size() - 1)
                Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
        }
    }

    void Metronome::tab()
    {
        if (_nextRhythmPointsIndex >= _rhythmPoints.size()) return;
        float timeInterval = _rhythmPoints[_nextRhythmPointsIndex];
        float dt1 = _timeSinceLastPoint;
        float dt2 = timeInterval - _timeSinceLastPoint;

        bool leftClosest = dt1 <= dt2;
        if (_nextRhythmPointsIndex == 1) leftClosest = false;
        if (_nextRhythmPointsIndex == _rhythmPoints.size() - 1) leftClosest = true;
        int closestPointIndex = leftClosest ? _nextRhythmPointsIndex - 1 : _nextRhythmPointsIndex;
        float d = leftClosest ? dt1 : dt2;

        if (d <= _hitDeltaTime) // hit success
            _hitted[closestPointIndex] = 1;
        else
        {
            int scriptIndex = closestPointIndex - 1;
            if (_wrongHitCallBack)
                _wrongHitCallBack(scriptIndex, d);
        }
    }

    void Metronome::setMissCallBack(MissCallBack missCallBack)
    {
        _missCallBack = std::move(missCallBack);
    }

    void Metronome::setWrongHitCallBack(TabCallBack wrongHitCallBack)
    {
        _wrongHitCallBack = std::move(_wrongHitCallBack);
    }

    void Metronome::setHitCallBack(TabCallBack hitCallBack)
    {
        _hitCallBack = std::move(hitCallBack);
    }

    void Metronome::setRhythmCallBack(RhythmCallBack rhythmCallBack)
    {
        _rhythmCallBack = rhythmCallBack;
    }


}